import os
import cppyy


def get_naoth_dir():
    script_path = os.path.abspath(__file__)
    return os.path.abspath(os.path.join(script_path, "../../../../../"))

def get_toolchain_dir():
    return os.path.abspath(os.environ["NAOTH_TOOLCHAIN_PATH"])

def init_simulator():
    naoth_dir = get_naoth_dir()
    toolchain_dir = get_toolchain_dir()

    # load shared library: all depending libraries should be found automatically
    cppyy.load_library(os.path.join(
        naoth_dir, "NaoTHSoccer/dist/Native/libdummysimulator.so"))

    # add relevant include paths to allow mapping our code
    cppyy.add_include_path(os.path.join(naoth_dir, "Framework/Commons/Source"))
    cppyy.add_include_path(os.path.join(naoth_dir, "NaoTHSoccer/Source"))
    cppyy.add_include_path(os.path.join(toolchain_dir, "toolchain_native/extern/include"))
    

    # include platform
    cppyy.include(os.path.join(
        naoth_dir, "Framework/Commons/Source/PlatformInterface/Platform.h"))
    cppyy.include(os.path.join(
        naoth_dir, "Framework/Platforms/Source/DummySimulator/DummySimulator.h"))

    # change working directory so that the configuration is found
    os.chdir(os.path.join(naoth_dir, "NaoTHSoccer"))

    # start dummy simulator
    sim = cppyy.gbl.DummySimulator(False, False, 5401)
    cppyy.gbl.naoth.Platform.getInstance().init(sim)

    # create the cognition and motion objects
    cog = cppyy.gbl.createCognition()
    mo = cppyy.gbl.createMotion()

    # cast to callable
    callable_cog = cppyy.bind_object(
        cppyy.addressof(cog), cppyy.gbl.naoth.Callable)
    callable_mo = cppyy.bind_object(
        cppyy.addressof(mo), cppyy.gbl.naoth.Callable)

    sim.registerCognition(callable_cog)
    sim.registerMotion(callable_mo)

    # get access to the module manager and return it to the calling function
    cppyy.include(os.path.join(naoth_dir, "Framework/Commons/Source/ModuleFramework/ModuleManager.h"))
    cppyy.include(os.path.join(naoth_dir, "NaoTHSoccer/Source/Cognition/Cognition.h"))
    
    mm = cppyy.gbl.getModuleManager(cog)

    return (sim, mm)

if __name__ == "__main__":

    sim, mm = init_simulator()

    # get the ball detector module
    ball_detector = mm.getModule("CNNBallDetector").getModule()

    # get references to the image input representations
    imageBottom = ball_detector.getRequire().at("Image")
    imageTop = ball_detector.getRequire().at("ImageTop")

    while True:
        sim.executeFrame()