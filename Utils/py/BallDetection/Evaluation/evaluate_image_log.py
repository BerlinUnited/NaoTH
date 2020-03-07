import os
import argparse
import cppyy
import PIL.Image
from typing import NamedTuple, Tuple, List
import numpy as np
import time
import ctypes
import xml.etree.ElementTree as ET


def get_naoth_dir():
    script_path = os.path.abspath(__file__)
    return os.path.abspath(os.path.join(script_path, "../../../../../"))


def get_toolchain_dir():
    return os.path.abspath(os.environ["NAOTH_TOOLCHAIN_PATH"])


class Point2D(NamedTuple):
    x: float
    y: float


class Rectangle(NamedTuple):
    top_left: Point2D
    bottom_right: Point2D

    def intersection_over_union(self, xtl: float, ytl: float, xbr: float, ybr: float):

        # compute x and y coordinates of the intersection rectangle
        intersection_topleft = Point2D(
            min(self.top_left.x, xtl), min(self.top_left.y, ytl))
        intersection_bottomright = Point2D(
            max(self.bottom_right.x, xbr), max(self.bottom_right.y, ybr))
        intersection = Rectangle(intersection_topleft,
                                 intersection_bottomright)

        # compute the intersection, self and other area
        intersectionArea = max(0, intersection.bottom_right.x - intersection.top_left.x + 1) + \
            max(0, intersection.bottom_right.y - intersection.top_left.y + 1)

        selfArea = (self.bottom_right.x - self.top_left.x + 1) * \
            (self.bottom_right.y - self.top_left.y + 1)
        otherArea = (xbr - xtl + 1) * (ybr - ytl + 1)

        # return the intersecton over union
        return intersectionArea / float(selfArea + otherArea - intersectionArea)


class Frame(NamedTuple):
    file: str
    bottom: bool
    balls: List[Rectangle]
    cam_matrix_translation: Tuple[float, float, float]
    cam_matrix_rotation: np.array


def get_frames_for_dir(d):
    # parse the CVAT XML 1.1 file with the annotations, we assume it has the same name as the directory, but ends with ".xml"
    annos_file = d.rstrip("/\\") + ".xml"
    if os.path.isfile(annos_file):
        annos = ET.parse(annos_file)
    else:
        raise "annotation file " + annos_file + " was not found!"

    file_names = os.listdir(d)
    file_names.sort()
    imageFiles = [os.path.join(d, f) for f in file_names if os.path.isfile(
        os.path.join(d, f)) and f.endswith(".png")]

    result = list()
    for file in imageFiles:

        # open image to get the metadata
        img = PIL.Image.open(file)
        bottom = img.info["CameraID"] == "1"
        # parse camera matrix using metadata in the PNG file
        cam_matrix_translation = (float(img.info["t_x"]), float(
            img.info["t_y"]), float(img.info["t_z"]))

        cam_matrix_rotation = np.array(
            [
                [float(img.info["r_11"]), float(
                    img.info["r_12"]), float(img.info["r_13"])],
                [float(img.info["r_21"]), float(
                    img.info["r_22"]), float(img.info["r_23"])],
                [float(img.info["r_31"]), float(
                    img.info["r_32"]), float(img.info["r_33"])]
            ])
        # add ground truth (all actual balls) to the frame
        balls = list()
        # Search for XML tag which name attribute has the filename as value
        for m in annos.findall(".//image[@name='{}']/box[@label='ball']".format(os.path.basename(file))):
            top_left = Point2D(float(m.attrib["xtl"]), float(m.attrib["ytl"]))
            bottom_right = Point2D(float(m.attrib["xbr"]), float(m.attrib["ybr"]))
            balls.append(Rectangle(top_left, bottom_right))
        frame = Frame(file, bottom, balls, cam_matrix_translation,
                      cam_matrix_rotation)

        result.append(frame)

    return result


def load_image(f: Frame):
    # don't import cv globally, because the dummy simulator shared library might need to load a non-system library
    # and we need to make sure loading the dummy simulator shared library happens first
    import cv2 as cv

    cv_img = cv.imread(f)

    # convert image for bottom to yuv422
    cv_img = cv.cvtColor(cv_img, cv.COLOR_BGR2YUV).tobytes()
    yuv422 = np.ndarray(480*640*2, np.uint8)
    for i in range(0, 480 * 640, 2):
        yuv422[i*2] = cv_img[i*3]
        yuv422[i*2 + 1] = (cv_img[i*3 + 1] + cv_img[i*3 + 4]) / 2.0
        yuv422[i*2 + 2] = cv_img[i*3 + 3]
        yuv422[i*2 + 3] = (cv_img[i*3 + 2] + cv_img[i*3 + 5]) / 2.0
    return yuv422


class Evaluator:

    def __init__(self):
        naoth_dir = get_naoth_dir()
        toolchain_dir = get_toolchain_dir()

        # load shared library: all depending libraries should be found automatically
        cppyy.load_library(os.path.join(
            naoth_dir, "NaoTHSoccer/dist/Native/libdummysimulator.so"))

        # add relevant include paths to allow mapping our code
        cppyy.add_include_path(os.path.join(
            naoth_dir, "Framework/Commons/Source"))
        cppyy.add_include_path(os.path.join(naoth_dir, "NaoTHSoccer/Source"))
        cppyy.add_include_path(os.path.join(
            toolchain_dir, "toolchain_native/extern/include"))

        # include platform
        cppyy.include(os.path.join(
            naoth_dir, "Framework/Commons/Source/PlatformInterface/Platform.h"))
        cppyy.include(os.path.join(
            naoth_dir, "Framework/Platforms/Source/DummySimulator/DummySimulator.h"))

        # change working directory so that the configuration is found
        os.chdir(os.path.join(naoth_dir, "NaoTHSoccer"))

        # start dummy simulator
        self.sim = cppyy.gbl.DummySimulator(False, False, 5401)
        cppyy.gbl.naoth.Platform.getInstance().init(self.sim)

        # create the cognition and motion objects
        cog = cppyy.gbl.createCognition()
        mo = cppyy.gbl.createMotion()

        # cast to callable
        callable_cog = cppyy.bind_object(
            cppyy.addressof(cog), cppyy.gbl.naoth.Callable)
        callable_mo = cppyy.bind_object(
            cppyy.addressof(mo), cppyy.gbl.naoth.Callable)

        self.sim.registerCognition(callable_cog)
        self.sim.registerMotion(callable_mo)

        # make more representations available to cppyy
        cppyy.include(os.path.join(
            naoth_dir, "Framework/Commons/Source/ModuleFramework/ModuleManager.h"))
        cppyy.include(os.path.join(
            naoth_dir, "NaoTHSoccer/Source/Cognition/Cognition.h"))
        cppyy.include(os.path.join(
            naoth_dir, "NaoTHSoccer/Source/Representations/Perception/BallCandidates.h"))
        cppyy.include(os.path.join(
            naoth_dir, "NaoTHSoccer/Source/Representations/Perception/CameraMatrix.h"))

        # get access to the module manager and return it to the calling function
        self.moduleManager = cppyy.gbl.getModuleManager(cog)

        # get the ball detector module
        self.ball_detector = self.moduleManager.getModule(
            "CNNBallDetector").getModule()

    def set_current_frame(self, frame):
        # get reference to the image input representation
        if frame.bottom:
            imageRepresentation = self.ball_detector.getRequire().at("Image")
        else:
            imageRepresentation = self.ball_detector.getRequire().at("ImageTop")

        print(frame.file)

        # load image in YUV422 format
        yuv422 = load_image(frame.file)
        p_data = yuv422.ctypes.data_as(ctypes.POINTER(ctypes.c_uint8))
        # move image into representation
        imageRepresentation.copyImageDataYUV422(p_data, yuv422.size)

        # set camera matrix
        if frame.bottom:
            camMatrix = self.ball_detector.getRequire().at("CameraMatrix")
        else:
            camMatrix = self.ball_detector.getRequire().at("CameraMatrixTop")

        # TODO: invalidate other camera matrix

        camMatrix.valid = True
        camMatrix.translation.x = frame.cam_matrix_translation[0]
        camMatrix.translation.y = frame.cam_matrix_translation[1]
        camMatrix.translation.z = frame.cam_matrix_translation[2]

        for c in range(0, 3):
            for r in range(0, 3):
                camMatrix.rotation.c[c][r] = frame.cam_matrix_rotation[r, c]

    def evaluate_detection(self, frame: Frame, eval_functions):
        # get the ball candidates from the module
        if frame.bottom:
            detected_balls = self.ball_detector.getProvide().at("BallCandidates")
        else:
            detected_balls = self.ball_detector.getProvide().at("BallCandidatesTop")

        for f in eval_functions:
            score = f(frame, detected_balls.patchesYUVClassified)
            print("score:", score)

    def execute(self, directories, eval_functions):
        for d in directories:
            frames = get_frames_for_dir(d)
            for f in frames:
                self.set_current_frame(f)
                self.sim.executeFrame()
                self.evaluate_detection(f, eval_functions)


def best_ball_patch_intersection(frame, patches):
    best = 0.0
    for p in patches:
        for b in frame.balls:
            iuo = b.intersection_over_union(p.min.x, p.min.y, p.max.x, p.max.y)
            if iuo > best:
                best = iuo
    return best


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description='Evaluate ball detection on logfile images annotated with CVAT.')
    parser.add_argument('directory', nargs='+',
                        help='A list of directories containing the images files (as png with the integrated camera matrix)')

    args = parser.parse_args()

    evaluator = Evaluator()
    evaluator.execute(args.directory, [best_ball_patch_intersection])
