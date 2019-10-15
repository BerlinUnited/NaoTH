import cppyy

# load shared libraries
cppyy.load_library("../../LinuxToolchain/toolchain_native/extern/lib/libprotobuf.so")
cppyy.load_library("/usr/lib/libglib-2.0.so")
cppyy.load_library("dist/Native/libcommons.so")
cppyy.load_library("dist/Native/libnaoth-soccer.so")
cppyy.load_library("dist/Native/libdummysimulator.so")

# set include paths
cppyy.add_include_path("../Framework/Commons/Source")
cppyy.add_include_path("Source")
cppyy.add_include_path("/usr/include/glib-2.0")
cppyy.add_include_path("/usr/lib/glib-2.0/include")
cppyy.add_include_path("/home/steffen/NaoTH/LinuxToolchain/toolchain_native/extern/include")

# include platform
cppyy.include("../Framework/Commons/Source/PlatformInterface/Platform.h")
cppyy.include("../Framework/Platforms/Source/DummySimulator/DummySimulator.h")
cppyy.include("../Framework/Commons/Source/PlatformInterface/PlatformInterface.h")  # really required?

sim = cppyy.gbl.DummySimulator(False, False, 5401)
cppyy.gbl.naoth.Platform.getInstance().init(sim)

cog = cppyy.gbl.createCognition()
mo = cppyy.gbl.createMotion()

# cast to callable
from cppyy import addressof, bind_object
cog2 = bind_object(addressof(cog), cppyy.gbl.naoth.Callable)
mo2 = bind_object(addressof(mo), cppyy.gbl.naoth.Callable)

sim.registerCognition(cog2)
sim.registerMotion(mo2)

cppyy.include("../Framework/Commons/Source/ModuleFramework/ModuleManager.h")
cppyy.include("Source/Cognition/Cognition.h")
# using bind_object(addressof(cog), cppyy.gbl.naoth.ModuleManager) didn't work... why?
mm = cppyy.gbl.getModuleManager(cog)

# getting access to the blackboard through a module
cnnbd = mm.getModule("CNNBallDetector").getModule()
im = cnnbd.getRequire().at("Image")
imTop = cnnbd.getRequire().at("ImageTop")

import numpy as np
import cv2 as cv
import ctypes

try:
    import numba
    from numba import jit

    @jit(nopython=True)
    def convert_image(_in, _out):
        for i in range(0, 480 * 640, 2):
            _out[i*2] = _in[i*3]
            _out[i*2 + 1] = (_in[i*3 + 1] + _in[i*3 + 4]) / 2.0
            _out[i*2 + 2] = _in[i*3 + 3]
            _out[i*2 + 3] = (_in[i*3 + 2] + _in[i*3 + 5]) / 2.0

except ImportError:
    def convert_image(_in, _out):
        for i in range(0, 480 * 640, 2):
            _out[i*2] = _in[i*3]
            _out[i*2 + 1] = (_in[i*3 + 1] + _in[i*3 + 4]) / 2.0
            _out[i*2 + 2] = _in[i*3 + 3]
            _out[i*2 + 3] = (_in[i*3 + 2] + _in[i*3 + 5]) / 2.0


capture = cv.VideoCapture(0)
while True:
    # python cv2 stuff
    ret, image = capture.read()
    image = cv.resize(image, (640, 480))
    gray = cv.cvtColor(image, cv.COLOR_BGR2GRAY)
    ret, thresh = cv.threshold(gray, 0, 255, cv.THRESH_BINARY_INV+cv.THRESH_OTSU)
    thresh = cv.merge([thresh, thresh, thresh])

    # convert image for bottom to yuv422
    image = cv.cvtColor(image, cv.COLOR_BGR2YUV).tobytes()
    yuv422 = np.ndarray(480*640*2, np.uint8)
    convert_image(image, yuv422)

    # convert image for top to yuv422
    thresh = cv.cvtColor(thresh, cv.COLOR_BGR2YUV).tobytes()
    yuv_thresh = np.ndarray(480*640*2, np.uint8)
    convert_image(thresh, yuv_thresh)

    # copy into image representation
    p_data = yuv422.ctypes.data_as(ctypes.POINTER(ctypes.c_uint8))
    im.copyImageDataYUV422(p_data, yuv422.size)
    p_data = yuv_thresh.ctypes.data_as(ctypes.POINTER(ctypes.c_uint8))
    imTop.copyImageDataYUV422(p_data, yuv_thresh.size)

    # execute dummy simulator
    sim.executeFrame()
