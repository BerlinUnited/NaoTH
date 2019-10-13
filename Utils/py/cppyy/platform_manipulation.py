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
mm = cppyy.gbl.getModuleManager(cog)  # using bind_object(addressof(cog), cppyy.gbl.naoth.ModuleManager) didn't work... why?

cnnbd = mm.getModule("CNNBallDetector").getModule()
im = cnnbd.getRequire().at("Image")

import numpy as np
import cv2 as cv

capture = cv.VideoCapture(0)

while True:
    # python cv2 stuff
    ret, image = capture.read()
    image = cv.resize(image, (640, 480))
    gray = cv.cvtColor(image, cv.COLOR_BGR2GRAY)
    ret, thresh = cv.threshold(gray, 0, 255, cv.THRESH_BINARY_INV+cv.THRESH_OTSU)
    tresh = cv.merge([thresh, thresh, thresh])

    image = cv.cvtColor(image, cv.COLOR_BGR2YUV).tobytes()
    for i in range(im.width() * im.height()):
        x = i % im.width()
        y = i // im.width()

        im.set(x, y, image[i * 3], image[i * 3 + 1], image[i * 3 + 2])

    sim.executeFrame()
