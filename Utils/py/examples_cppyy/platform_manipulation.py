import cppyy
import os
from cppyy import addressof, bind_object
import numpy as np
import cv2
import ctypes
import sys
from numba import jit


def get_naoth_dir():
    script_path = os.path.abspath(__file__)
    return os.path.abspath(os.path.join(script_path, "../../../../"))


def get_toolchain_dir():
    toolchain_path = os.path.join(
        os.path.abspath(os.environ["NAO_CTC"]), "../")
    return toolchain_path


@jit(nopython=True)
def convert_image(_in, _out):
    for i in range(0, 480 * 640, 2):
        _out[i * 2] = _in[i * 3]
        _out[i * 2 + 1] = (_in[i * 3 + 1] + _in[i * 3 + 4]) / 2.0
        _out[i * 2 + 2] = _in[i * 3 + 3]
        _out[i * 2 + 3] = (_in[i * 3 + 2] + _in[i * 3 + 5]) / 2.0


if __name__ == "__main__":
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
    sim = cppyy.gbl.DummySimulator(False, False, 5401)
    cppyy.gbl.naoth.Platform.getInstance().init(sim)

    cog = cppyy.gbl.createCognition()
    mo = cppyy.gbl.createMotion()

    # cast to callable

    cog2 = bind_object(addressof(cog), cppyy.gbl.naoth.Callable)
    mo2 = bind_object(addressof(mo), cppyy.gbl.naoth.Callable)

    sim.registerCognition(cog2)
    sim.registerMotion(mo2)

    cppyy.include(os.path.join(naoth_dir, "NaoTHSoccer/Source/Cognition/Cognition.h"))
    # using bind_object(addressof(cog), cppyy.gbl.naoth.ModuleManager) didn't work... why?
    mm = cppyy.gbl.getModuleManager(cog)

    # getting access to the blackboard through a module
    cnnbd = mm.getModule("CNNBallDetector").getModule()
    im = cnnbd.getRequire().at("Image")
    imTop = cnnbd.getRequire().at("ImageTop")

    # python cv2 stuff
    while True:
        image = np.random.rand(640, 480, 3).astype(np.float32)
        image = cv2.resize(image, (640, 480))
        gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
        thresh = cv2.merge([gray, gray, gray])

        # convert image for bottom to yuv422
        image = cv2.cvtColor(image, cv2.COLOR_BGR2YUV).tobytes()
        yuv422 = np.ndarray(480 * 640 * 2, np.uint8)
        convert_image(image, yuv422)

        # convert image for top to yuv422
        thresh = cv2.cvtColor(thresh, cv2.COLOR_BGR2YUV).tobytes()
        yuv_thresh = np.ndarray(480 * 640 * 2, np.uint8)
        convert_image(thresh, yuv_thresh)

        # copy into image representation
        p_data = yuv422.ctypes.data_as(ctypes.POINTER(ctypes.c_uint8))
        im.copyImageDataYUV422(p_data, yuv422.size)
        p_data = yuv_thresh.ctypes.data_as(ctypes.POINTER(ctypes.c_uint8))
        imTop.copyImageDataYUV422(p_data, yuv_thresh.size)

        # execute dummy simulator
        sim.executeFrame()
