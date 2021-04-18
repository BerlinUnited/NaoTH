import math
import os
import sys
import cppyy
from cppyy import addressof, bind_object
import numpy as np
import matplotlib.pyplot as plt


def get_naoth_dir():
    script_path = os.path.abspath(__file__)
    return os.path.abspath(os.path.join(script_path, "../../../../"))


def get_toolchain_dir():
    toolchain_path = os.path.join(
        os.path.abspath(os.environ["NAO_CTC"]), "../")
    return toolchain_path


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
    cppyy.add_include_path(os.path.join(naoth_dir, "Framework/Commons/Source"))
    cppyy.add_include_path(os.path.join(naoth_dir, "Framework/Commons/Source/Messages"))
    cppyy.add_include_path(os.path.join(naoth_dir, "NaoTHSoccer/Source"))
    cppyy.add_include_path(os.path.join(toolchain_dir, "toolchain_native/extern/include"))
    cppyy.add_include_path(os.path.join(toolchain_dir, "toolchain_native/extern/include/glib-2.0"))
    cppyy.add_include_path(os.path.join(toolchain_dir, "toolchain_native/extern/lib/glib-2.0/include"))

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
    # enable required modules
    mm.setModuleEnabled("MultiKalmanBallLocator", True)
    mm.setModuleEnabled("FakeBallDetector", True)
    mm.setModuleEnabled("FakeCameraMatrixFinder", True)

    # include header of modules which will be manipulated
    cppyy.include("Cognition/Modules/VisualCortex/FakeBallDetector/FakeBallDetector.h")
    cppyy.include("Cognition/Modules/Modeling/BallLocator/MultiKalmanBallLocator/MultiKalmanBallLocator.h")

    # get MultiKalmanBallLocator- and FakeBallDetector-Instances
    mkbl = bind_object(addressof(mm.getModule("MultiKalmanBallLocator").getModule()),
                       cppyy.gbl.MultiKalmanBallLocator)
    fbd = bind_object(addressof(mm.getModule("FakeBallDetector").getModule()),
                      cppyy.gbl.FakeBallDetector)

    # add some fake balls for the FakeBallDetector to "detect"
    fakeBall = cppyy.gbl.FakeBallDetector.FakeBall()
    for i in range(500, 10000, 500):
        fakeBall.position.x = i
        fbd.addFakeBall(fakeBall)

    # Hack: iterate some frames to let MKBL's models converge
    for i in range(0, 1000):
        sim.executeFrame()

    # get initial state of distance and 95%-probability-ellipse-area
    filters = sorted(mkbl.get_filter(), key=lambda m: m.getState()(0))
    initial_area = np.array(list(map(lambda m: math.pi * m.getEllipseLocation().minor * m.getEllipseLocation().major, filters))) / 1000 / 1000
    initial_dist = np.array(list(map(lambda m: math.sqrt(m.getState()(0) * m.getState()(0) + m.getState()(2) * m.getState()(2)), filters)))

    # calculate current threshold radius function
    r = math.sqrt(2) * (initial_dist + 125) / 1000

    # start some plotting
    fig, ax = plt.subplots()
    ax.set_ylim([0, 70])
    ax.xaxis.set_ticks(np.arange(500, 10000, 500))
    ax.set_xlabel('Distance [mm]')
    ax.set_ylabel('Area [m²]')
    # plot area threshold depending on distance
    ax.plot(initial_dist, r * r * math.pi, color='black')
    # plot initial 95%-probability-ellipse-area if ball is seen perfectly
    ax.plot(initial_dist, initial_area, color=plt.cm.coolwarm(mkbl.get_filter()[0].ballSeenFilter.floatValue()))
    # add a horizontal line for the half and the full field area
    field_area = 9 * 6
    ax.axhline(field_area, 0, initial_dist[-1], linestyle='--')
    ax.text(400, field_area, 'the field')
    ax.axhline(field_area/2, 0, initial_dist[-1], linestyle='--')
    ax.text(400, field_area/2, 'half the field')

    # remove all FakeBalls to see how the uncertainty is changing
    fbd.clearFakeBalls()

    # plot state after each 5th frame
    for i in range(1, 31):
        sim.executeFrame()
        if i % 5 == 0:
            filters = sorted(mkbl.get_filter(), key=lambda m: m.getState()(0))
            area = np.array(list(map(lambda m: math.pi * m.getEllipseLocation().minor * m.getEllipseLocation().major, filters))) / 1000 / 1000
            dist = np.array(list(map(lambda m: math.sqrt(m.getState()(0) * m.getState()(0) + m.getState()(2) * m.getState()(2)), filters)))

            ax.plot(dist, area, color=plt.cm.coolwarm(mkbl.get_filter()[0].ballSeenFilter.floatValue()))

    plt.show()
