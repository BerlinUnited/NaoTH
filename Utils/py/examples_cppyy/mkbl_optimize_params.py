"""
    determine process noise covariance based on a certain optimality criteria
"""

import os
import sys
import itertools
import cppyy
from cppyy import addressof, bind_object
import numpy as np
import scipy.optimize as so
import matplotlib.pyplot as plt


def get_naoth_dir():
    script_path = os.path.abspath(__file__)
    return os.path.abspath(os.path.join(script_path, "../../../../"))


def get_toolchain_dir():
    toolchain_path = os.path.join(
        os.path.abspath(os.environ["NAO_CTC"]), "../")
    return toolchain_path


def f(x, fakeballs, fbd, mkbl, dpl, command, arguments):
    residual = np.zeros(len(fakeballs))
    for i, fb in enumerate(fakeballs):
        was_seen = False

        # set process noise
        arguments["processNoiseStdQ00"] = str(x[0])
        arguments["processNoiseStdQ11"] = str(x[1])
        dpl.executeDebugCommand(command, arguments, cppyy.gbl.std.cout)

        # add fake ball
        initialFakeBall = cppyy.gbl.FakeBallDetector.FakeBall()
        initialFakeBall.position.x = fb[0]
        initialFakeBall.position.y = fb[1]
        initialFakeBall.velocity.x = fb[2]
        initialFakeBall.velocity.y = fb[3]
        initialFakeBall.const_velocity = True
        fbd.addFakeBall(initialFakeBall)

        # execute one frame to create a model in the mkbl
        while True:
            sim.executeFrame()
            if not (was_seen or fbd.getProvide().at("MultiBallPercept").wasSeen()):
                continue  # ball was not seen

            # TODO: better break condition?
            if (was_seen and not fbd.getProvide().at("MultiBallPercept").wasSeen()):
                break

            was_seen = True
            currentFakeBall = fbd.getFakeBalls()[0]
            currentModel = mkbl.get_filter()[0]

            e = (np.matrix((currentFakeBall.position.x,
                            currentFakeBall.velocity.x,
                            currentFakeBall.position.y,
                            currentFakeBall.velocity.y))
                 - np.matrix((currentModel.getState()(0),
                              currentModel.getState()(1),
                              currentModel.getState()(2),
                              currentModel.getState()(3))))

            residual[i] += np.linalg.norm(e)

        fbd.clearFakeBalls()
        mkbl.clear_filter()
        print(".")
    return residual


if __name__ == "__main__":
    # naoth_dir = get_naoth_dir()
    naoth_dir = "/home/steffen/NaoTH/NaoTH-2020"
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
    cppyy.include("Tools/Debug/DebugParameterList.h")

    # get MultiKalmanBallLocator-, FakeBallDetector-Instances and DebugParameterList
    mkbl = bind_object(addressof(mm.getModule("MultiKalmanBallLocator").getModule()),
                       cppyy.gbl.MultiKalmanBallLocator)
    fbd = bind_object(addressof(mm.getModule("FakeBallDetector").getModule()),
                      cppyy.gbl.FakeBallDetector)
    dpl = mkbl.getProvide().at("DebugParameterList")

    command = "ParameterList:set"
    arguments = cppyy.gbl.std.map[str, str]()
    arguments["<name>"] = "MultiKalmanBallLocator"

    # initalize FakeBalls
    fakeballs = list()

    # vertical trajectories
    start_loc_y = [-3000]
    start_loc_x = range(500, 5000, 500)
    start_vel_x = [0]
    start_vel_y = [500]
    fakeballs += [x for x in itertools.product(start_loc_x, start_loc_y, start_vel_x, start_vel_y)]

    # horizontal trajectories
    start_loc_y = range(-1500, 2000, 500)
    start_loc_x = [4500]
    start_vel_x = [-500]
    start_vel_y = [0]
    fakeballs += [x for x in itertools.product(start_loc_x, start_loc_y, start_vel_x, start_vel_y)]

    # TODO: radial trajectories?

    # Note: diag([x0, x0]) = Q
    x0 = [15, 500]

    result = so.least_squares(f, x0, method='lm', args=(fakeballs, fbd, mkbl, dpl, command, arguments))
    # result = so.least_squares(f, x0)

    print(result)
