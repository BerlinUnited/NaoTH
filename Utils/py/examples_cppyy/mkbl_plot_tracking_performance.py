import math
import os
import sys
import cppyy
from cppyy import addressof, bind_object
import numpy as np
import matplotlib.pyplot as plt

import select


def get_naoth_dir():
    script_path = os.path.abspath(__file__)
    return os.path.abspath(os.path.join(script_path, "../../../../"))


def get_toolchain_dir():
    toolchain_path = os.path.join(
        os.path.abspath(os.environ["NAO_CTC"]), "../")
    return toolchain_path


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

    # while sys.stdin not in select.select([sys.stdin], [], [], 0)[0]:
    #     sim.executeFrame()

    # initalize FakeBall
    initialFakeBall = cppyy.gbl.FakeBallDetector.FakeBall()
    initialFakeBall.position.x = 2000
    initialFakeBall.position.y = -1000
    initialFakeBall.velocity.x = 0
    initialFakeBall.velocity.y = 500
    initialFakeBall.const_velocity = True

    # paramPairs = [("15", "20"), ("15", "100"), ("15", "250"), ("15", "500"), ("15", "1000"), ("15", "2000")]
    paramPairs = [("1", "500"), ("15", "500"), ("100", "500"), ("250", "500"), ("500", "500"), ("1000", "500")]
    data = dict()
    for q00, q11 in paramPairs:
        ground_truth = list()
        model = list()
        arguments["processNoiseStdQ00"] = q00
        arguments["processNoiseStdQ11"] = q11
        dpl.executeDebugCommand(command, arguments, cppyy.gbl.std.cout)
        fbd.addFakeBall(initialFakeBall)

        # execute one frame to create a model in the mkbl
        sim.executeFrame()
        currentFakeBall = fbd.getFakeBalls()[0]
        currentModel = mkbl.get_filter()[0]
        ground_truth.append((currentFakeBall.position.x,
                             currentFakeBall.velocity.x,
                             currentFakeBall.position.y,
                             currentFakeBall.velocity.y))
        model.append((currentModel.getState()(0),
                      currentModel.getState()(1),
                      currentModel.getState()(2),
                      currentModel.getState()(3)))

        # Assumption: there is only one model
        while currentFakeBall.position.y < 1000:
            sim.executeFrame()
            currentFakeBall = fbd.getFakeBalls()[0]
            currentModel = mkbl.get_filter()[0]
            ground_truth.append((currentFakeBall.position.x,
                                 currentFakeBall.velocity.x,
                                 currentFakeBall.position.y,
                                 currentFakeBall.velocity.y))
            model.append((currentModel.getState()(0),
                          currentModel.getState()(1),
                          currentModel.getState()(2),
                          currentModel.getState()(3)))

        data["("+q00+","+q11+")"] = {'ground_truth': np.array(ground_truth),
                                     'model': np.array(model)}
        fbd.clearFakeBalls()
        mkbl.clear_filter()

    # start some plotting
    titles = ['Error in x-Location', 'Error in x-Velocity', 'Error in y-Location', 'Error in y-Velocity']
    fig, ax = plt.subplots(nrows=2, ncols=2)
    ax = np.reshape(ax, ax.size)
    for i in range(0, 4):
        # ax[i].plot(data["(15,500)"]['ground_truth'][:, i], label='ground_truth', color='black')
        for k, v in data.items():
            ax[i].plot(v['ground_truth'][:, i] - v['model'][:, i], label=k)
        ax[i].set_title(titles[i])
        ax[i].legend()

    plt.show()
