import os
import sys
import argparse
import csv
import cppyy
import PIL.Image
from typing import NamedTuple, Tuple, List
import numpy as np
import time
import ctypes
import xml.etree.ElementTree as ET
from cppyy import addressof, bind_object
import cppyy.ll

def get_naoth_dir():
    script_path = os.path.abspath(__file__)
    return os.path.abspath(os.path.join(script_path, "../../../../../"))


def get_toolchain_dir():
    toolchain_path = os.path.join(
        os.path.abspath(os.environ["NAO_CTC"]), "../")
    return toolchain_path


class Point2D(NamedTuple):
    x: float
    y: float


class Rectangle(NamedTuple):
    top_left: Point2D
    bottom_right: Point2D

    def intersection_over_union(self, xtl: float, ytl: float, xbr: float, ybr: float):

        # compute x and y coordinates of the intersection rectangle
        intersection_topleft = Point2D(
            max(self.top_left.x, xtl), max(self.top_left.y, ytl))
        intersection_bottomright = Point2D(
            min(self.bottom_right.x, xbr), min(self.bottom_right.y, ybr))
        intersection = Rectangle(intersection_topleft,
                                 intersection_bottomright)

        # compute the intersection, self and other area
        intersectionArea = max(0, intersection.bottom_right.x - intersection.top_left.x + 1) * \
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


def get_frames_for_dir(d, transform_to_squares=False):
    # parse the CVAT XML 1.1 for images file with the annotations, we assume it has the same name as the directory, but ends with ".xml"
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
            if m.attrib["occluded"] == None or m.attrib["occluded"] == '0':
                top_left = Point2D(
                    float(m.attrib["xtl"]), float(m.attrib["ytl"]))
                bottom_right = Point2D(
                    float(m.attrib["xbr"]), float(m.attrib["ybr"]))

                if transform_to_squares:
                    width = bottom_right.x - top_left.x
                    height = bottom_right.y - top_left.y
                    size = max(width, height)
                    bottom_right = Point2D(top_left.x + size, top_left.y + size)

                balls.append(Rectangle(top_left, bottom_right))

        frame = Frame(file, bottom, balls, cam_matrix_translation,
                      cam_matrix_rotation)

        result.append(frame)

    return result


def load_image(f: Frame):
    # don't import cv globally, because the dummy simulator shared library might need to load a non-system library
    # and we need to make sure loading the dummy simulator shared library happens first
    import cv2
    cv_img = cv2.imread(f)

    # convert image for bottom to yuv422
    cv_img = cv2.cvtColor(cv_img, cv2.COLOR_BGR2YUV).tobytes()
    yuv422 = np.ndarray(480*640*2, np.uint8)
    for i in range(0, 480 * 640, 2):
        yuv422[i*2] = cv_img[i*3]
        yuv422[i*2 + 1] = (cv_img[i*3 + 1] + cv_img[i*3 + 4]) / 2.0
        yuv422[i*2 + 2] = cv_img[i*3 + 3]
        yuv422[i*2 + 3] = (cv_img[i*3 + 2] + cv_img[i*3 + 5]) / 2.0
    return yuv422


def create_debug_image(file, balls, patches):
    import cv2

    img = cv2.imread(file)
    # draw all actual balls
    for b in balls:
        cv2.rectangle(img, (int(b.top_left.x), int(b.top_left.y)), (int(
            b.bottom_right.x), int(b.bottom_right.y)), (0, 165, 255))

    # draw the generated patches with their respective IOU (with first ball)
    for p in patches:
        cv2.rectangle(img, (p.min.x, p.min.y), (p.max.x, p.max.y), (0, 0, 255))
        if len(balls) > 0:
            iou = balls[0].intersection_over_union(
                p.min.x, p.min.y, p.max.x, p.max.y)
            if iou > 0.0:
                cv2.addText(img, "{:.2f}".format(iou), (p.min.x + 5, p.min.y+15),
                            "Serif", pointSize=8, color=(0, 0, 255, 128))

    return img


class Evaluator:

    def __init__(self):
        naoth_dir = get_naoth_dir()
        toolchain_dir = get_toolchain_dir()

        # load shared library: all depending libraries should be found automatically
        shared_lib_name = "libscriptsim.so"
        if sys.platform.startswith("win32"):
            shared_lib_name = "scriptsim.dll"
        elif sys.platform.startswith("darwin"):
            shared_lib_name = "libscriptsim.dylib"

        cppyy.load_library(os.path.join(
            naoth_dir, "NaoTHSoccer/dist/Native/" + shared_lib_name))

        # add relevant include paths to allow mapping our code
        cppyy.add_include_path(os.path.join(
            naoth_dir, "Framework/Commons/Source"))
        cppyy.add_include_path(os.path.join(naoth_dir, "NaoTHSoccer/Source"))
        cppyy.add_include_path(os.path.join(
            toolchain_dir, "toolchain_native/extern/include"))

        cppyy.add_include_path(os.path.join(
            toolchain_dir, "toolchain_native/extern/include/glib-2.0"))
        cppyy.add_include_path(os.path.join(
            toolchain_dir, "toolchain_native/extern/lib/glib-2.0/include"))

        # include platform
        cppyy.include(os.path.join(
            naoth_dir, "Framework/Commons/Source/PlatformInterface/Platform.h"))
        cppyy.include(os.path.join(
            naoth_dir, "Framework/Platforms/Source/DummySimulator/DummySimulator.h"))

        # change working directory so that the configuration is found
        orig_working_dir = os.getcwd()
        os.chdir(os.path.join(naoth_dir, "NaoTHSoccer"))

        # start dummy simulator
        cppyy.gbl.g_type_init()
        self.sim = cppyy.gbl.DummySimulator(False, 5401)
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

        cppyy.cppdef("""
               Pose3D* my_convert(CameraMatrix* m) { return static_cast<Pose3D*>(m); }
                """)

        # initialize the score object
        self.scores = dict()

        # restore original working directory
        os.chdir(orig_working_dir)

    def set_current_frame(self, frame):
        # get reference to the image input representation
        if frame.bottom:
            imageRepresentation = self.ball_detector.getRequire().at("Image")
            # set other image to black
            black = np.zeros(640*480*2, np.uint8)
            self.ball_detector.getRequire().at("ImageTop").copyImageDataYUV422(
                black.ctypes.data_as(ctypes.POINTER(ctypes.c_uint8)), black.size)
        else:
            imageRepresentation = self.ball_detector.getRequire().at("ImageTop")
            # set other image to black
            black = np.zeros(640*480*2, np.uint8)
            self.ball_detector.getRequire().at("Image").copyImageDataYUV422(
                black.ctypes.data_as(ctypes.POINTER(ctypes.c_uint8)), black.size)

        print(frame.file)

        # load image in YUV422 format
        yuv422 = load_image(frame.file)
        p_data = yuv422.ctypes.data_as(ctypes.POINTER(ctypes.c_uint8))

        # move image into representation
        imageRepresentation.copyImageDataYUV422(p_data, yuv422.size)
        # set camera matrix
        if frame.bottom:
            camMatrix = self.ball_detector.getRequire().at("CameraMatrix")
            # invalidate other camera matrix
            self.ball_detector.getRequire().at("CameraMatrixTop").valid = False
        else:
            camMatrix = self.ball_detector.getRequire().at("CameraMatrixTop")
            # invalidate other camera matrix
            self.ball_detector.getRequire().at("CameraMatrix").valid = False      

        p = cppyy.gbl.my_convert(camMatrix)
        p.translation.x = frame.cam_matrix_translation[0]
        p.translation.y = frame.cam_matrix_translation[1]
        p.translation.z = frame.cam_matrix_translation[2]

        for c in range(0, 3):
            for r in range(0, 3):
                p.rotation.c[c][r] = frame.cam_matrix_rotation[r, c]

    def evaluate_detection(self, frame: Frame, eval_functions, debug_threshold=None):
        import cv2

        # get the ball candidates from the module
        if frame.bottom:
            detected_balls = self.ball_detector.getProvide().at("BallCandidates")
        else:
            detected_balls = self.ball_detector.getProvide().at("BallCandidatesTop")

        debug = False
        for score_name, f in eval_functions.items():
            score = f(frame, detected_balls.patchesYUVClassified)
            if score is not None:
                self.scores[score_name].append(score)

                if debug_threshold is not None:
                    if score <= debug_threshold:
                        debug = True

                    title = "Ball Evaluator: press any key to continue or Q to exit"
                    if debug:
                        cv2.namedWindow(title)
                        img = create_debug_image(
                            frame.file, frame.balls, detected_balls.patchesYUVClassified)
                        cv2.imshow(title, img)
                        key = cv2.waitKey()
                    else:
                        img = np.ones((240, 320))*255
                        cv2.imshow(title, img)

                        key = cv2.waitKey(1)

                    if key == 113 or key == 27:
                        exit(0)
                    elif key == 114:
                        # repeat execution of the current frame, but do not count in statistics
                        print("Repeating frame")
                        return True
        # Continue with next frame
        return False

    def execute(self, directories, eval_functions, debug_threshold=None, transform_to_squares=False):

        # init the score lists
        for score_name, f in eval_functions.items():
            self.scores[score_name] = list()

        for d in directories:
            frames = get_frames_for_dir(d, transform_to_squares)
            for f in frames:
                self.set_current_frame(f)
                self.sim.executeFrame()
                while self.evaluate_detection(f, eval_functions, debug_threshold):
                    self.set_current_frame(f)
                    self.sim.executeFrame()

    def show_report(self):
        for score_name, score_values in self.scores.items():
            print()
            print(score_name)
            print("=" * len(score_name))
            # get percentile, average and other basic information
            scores = np.array(score_values)

            print("average: {}".format(np.average(scores)))
            print()

            for percentile in [0, 1, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95, 99]:
                if percentile == 50:
                    marker = "(median)"
                else:
                    marker = ""
                print("best {}% >= {} {}".format(100 - percentile,
                                                 np.percentile(scores, percentile), marker))

    def export_results(self, outputfile):
        # open CSV file for writing
        with open(outputfile, "w", newline='') as f:
            out = csv.writer(f)
            # write header
            out.writerow(["score_name", "score_value"])
            # output the score for all measures
            for score_name, score_values in self.scores.items():
                for v in score_values:
                    out.writerow([score_name, v])


def best_ball_patch_intersection(frame, patches):

    if len(frame.balls) == 0:
        # Don't penalize the patch detector for not finding a non-existent ball
        return None

    best = 0.0
    for p in patches:
        for b in frame.balls:
            iou = b.intersection_over_union(p.min.x, p.min.y, p.max.x, p.max.y)
            if iou > best:
                best = iou
    return best


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description='Evaluate ball detection on logfile images annotated with CVAT.')
    parser.add_argument('directory', nargs='+',
                        help="""A list of directories containing the images files (as png with the integrated camera matrix).
                        It is expected that an XML with the same name (but ending with .xml) is located in the parent folder of each given folder.""")

    parser.add_argument("--csv=F", type=str, dest="csv",
                        help="Output the scores to the given CSV file")

    parser.add_argument("--squares", action="store_true",
                        help="If given, transform the bounding boxes to squares before calculating the scores.")

    parser.add_argument('--debug-threshold=T', type=float, dest="debug_threshold",
                        help="If an image has a score with a worse than the given threshold, include this image in a debug view.")
    args = parser.parse_args()

    evaluator = Evaluator()
    with cppyy.ll.signals_as_exception():
        evaluator.execute(
            args.directory, {"Best IOU per image": best_ball_patch_intersection}, debug_threshold=args.debug_threshold, transform_to_squares=args.squares)
    
    if args.csv is not None:
        print("Exporting results to ", args.csv)
        evaluator.export_results(args.csv)
    evaluator.show_report()
