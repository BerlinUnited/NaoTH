import os
import cppyy

def get_naoth_dir():
    script_path = os.path.abspath(__file__)
    return os.path.abspath(os.path.join(script_path, "../../../../../"))

def init_simulator():
    naoth_dir = get_naoth_dir()

    # load shared library: all depending libraries should be found automatically
    cppyy.load_library(os.path.join(naoth_dir, "NaoTHSoccer/dist/Native/libdummysimulator.so"))

    # add relevant include paths to allow mapping our code
    cppyy.add_include_path(os.path.join(naoth_dir, "Framework/Commons/Source"))
    cppyy.add_include_path(os.path.join(naoth_dir, "NaoTHSoccer/Source"))

    # include platform
    cppyy.include(os.path.join(naoth_dir, "Framework/Commons/Source/PlatformInterface/Platform.h"))
    cppyy.include(os.path.join(naoth_dir, "Framework/Platforms/Source/DummySimulator/DummySimulator.h"))

    # change working directory so that the configuration is found
    os.chdir(os.path.join(naoth_dir, "NaoTHSoccer"))

    # start dummy simulator
    sim = cppyy.gbl.DummySimulator(False, False, 5401)
    cppyy.gbl.naoth.Platform.getInstance().init(sim)

if __name__ == "__main__":

    init_simulator()


