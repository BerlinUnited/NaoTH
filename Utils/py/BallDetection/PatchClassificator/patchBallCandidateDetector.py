
import ctypes
import os
import sys
from pathlib import Path

import cppyy
import cppyy.ll
import numpy as np
from naoth.log import Parser
from naoth.log import Reader as LogReader
from PIL import Image, PngImagePlugin


def get_naoth_dir():
    script_path = os.path.abspath(__file__)
    return os.path.abspath(os.path.join(script_path, "../../../../../"))



def yuv422_image_from_proto(message):
    # read each channel of yuv422 separately
    yuv422 = np.fromstring(message.data, dtype=np.uint8)

    return yuv422


def gray_image_from_proto(message):
    import cv2

    # read each channel of yuv422 separately
    yuv422 = np.frombuffer(message.data, dtype=np.uint8)
    y = yuv422[0::2]
    u = yuv422[1::4]
    v = yuv422[3::4]

    # convert from yuv422 to yuv888
    yuv888 = np.zeros(message.height * message.width * 3, dtype=np.uint8)
    yuv888[0::3] = y
    yuv888[1::6] = u
    yuv888[2::6] = v
    yuv888[4::6] = u
    yuv888[5::6] = v

    gray_image = y.reshape(message.height, message.width)
    # gray_image = cv2.cvtColor(gray, cv2.COLOR_BGR2GRAY)
    
    return gray_image



class BallCandidateExporter:

    def __init__(self):
        naoth_dir = get_naoth_dir()

        # load shared library: all depending libraries should be found automatically
        shared_lib_name = "libscriptsim.so"
        if sys.platform.startswith("win32"):
            shared_lib_name = "scriptsim.dll"
        elif sys.platform.startswith("darwin"):
            shared_lib_name = "libscriptsim.dylib"

        cppyy.load_library(
            os.path.join(naoth_dir, "NaoTHSoccer/dist/Native/" + shared_lib_name)
        )

        # add relevant include paths to allow mapping our code
        cppyy.add_include_path(os.path.join(naoth_dir, "Framework/Commons/Source"))

        # add relevant include paths to allow mapping our code
        cppyy.add_include_path(os.path.join(naoth_dir, "Framework/Commons/Source"))
        cppyy.add_include_path(os.path.join(naoth_dir, "NaoTHSoccer/Source"))

        # add dependenvies installed thorugh the toolchain repo or natively

        cppyy.add_include_path("/opt/homebrew/Cellar/eigen/3.4.0_1/include/eigen3")
        cppyy.add_include_path("/opt/homebrew/Cellar/glib/2.78.1/include/glib-2.0")
        cppyy.add_include_path("/opt/homebrew/Cellar/glib/2.78.1/lib/glib-2.0/include")

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
        self.ball_detector = self.moduleManager.getModule("CNNBallDetector").getModule()

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
            # # set other image to black
            black = np.zeros(640*480*2, np.uint8)
            self.ball_detector.getRequire().at("Image").copyImageDataYUV422(
                black.ctypes.data_as(ctypes.POINTER(ctypes.c_uint8)), black.size)
        
        # load image in YUV422 format
        yuv422 = yuv422_image_from_proto(frame["Image"])  if frame.bottom else yuv422_image_from_proto(frame["ImageTop"])
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

        # p = cppyy.gbl.my_convert(camMatrix)
        # p.translation.x = frame.cam_matrix_translation[0]
        # p.translation.y = frame.cam_matrix_translation[1]
        # p.translation.z = frame.cam_matrix_translation[2]

        # for c in range(0, 3):
        #     for r in range(0, 3):
        #         p.rotation.c[c][r] = frame.cam_matrix_rotation[r, c]

    def get_ball_candidates(self, frame):
        if frame.bottom:
            return self.ball_detector.getProvide().at("BallCandidates")   

        else:
            return self.ball_detector.getProvide().at("BallCandidatesTop")



    def execute(self, file_name, output_folder):
        import cv2 
        my_parser = Parser()

        # register the protobuf message names which are not defined in the log file by default
        my_parser.register("ImageTop", "Image")
        my_parser.register("BallCandidatesTop", "BallCandidates")
        my_parser.register("CameraMatrixTop", "CameraMatrix")


        for frame in LogReader(file_name, my_parser):
            try:
                type(frame["ImageTop"])
                frame.bottom = False
                frame.cam_id = 1
            except:
                frame.bottom = True
                frame.cam_id = 0

            # print(frame.number)
            # print("bottom" if frame.bottom else "top")

            self.set_current_frame(frame)
            self.sim.executeFrame()

            ball_candidates = self.get_ball_candidates(frame)
            patches = ball_candidates.patchesYUVClassified

            if len(patches) > 0:
                print(f"{frame.number} - {frame.cam_id} - {len(patches)}")

                subfolder = "bottom" if frame.bottom else "top"
                patch_folder = Path(output_folder) / subfolder

                Path(patch_folder).mkdir(exist_ok=True, parents=True)

                img = gray_image_from_proto(frame["Image"]) if frame.bottom else gray_image_from_proto(frame["ImageTop"])

                for idx, p in enumerate(patches):
                    crop_img = img[p.min.y : p.max.y, p.min.x : p.max.x]

                    patch_file_name = patch_folder / f"{frame.number}_{idx}.png"
                    cv2.imwrite(str(patch_file_name), crop_img)

                    # section for writing meta data
                    meta = PngImagePlugin.PngInfo()
                    meta.add_text("CameraID", str(frame.cam_id))

                    with Image.open(str(patch_file_name)) as im_pill:
                        im_pill.save(str(patch_file_name), pnginfo=meta)

            # print(f"found {len(ball_candidates.patches)} patches")
            # print(f"found {len(ball_candidates.patchesYUV)} patchesYUV")
            # print(f"found {len(ball_candidates.patchesYUVClassified)} patchesYUVClassified")
            # print()




if __name__ == "__main__":
    exporter = BallCandidateExporter()
    exporter.execute("/Users/max/Projects/naoth-max/combined.log", "/Users/max/Projects/naoth-max/test")
