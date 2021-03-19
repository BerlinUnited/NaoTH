import cppyy
import os
import sys


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
    cppyy.add_include_path(os.path.join(
        naoth_dir, "Framework/Commons/Source"))

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

    # include module
    cppyy.include("Motion/SensorFilter/IMUModel.h")

    out = cppyy.gbl.std.cout

    from cppyy.gbl import IMUModel

    x = IMUModel()
    imu_data = x.getProvide().at("IMUData")
    robot_info = x.getRequire().at("RobotInfo")
    print("RobotInfo.basicTimeStep: {}".format(robot_info.basicTimeStep))
    print("oooops basicTimeStep isn't initialized... do it now")
    robot_info.basicTimeStep = 20
    print("RobotInfo.basicTimeStep: {}".format(x.getRequire().at("RobotInfo").basicTimeStep))
    print("IMUData before execution 1")
    imu_data.print(out)
    print("excuting")
    x.execute()
    print("IMUData after execution 1")
    imu_data.print(out)
    print("excuting")
    x.execute()
    print("IMUData after execution 2")
    imu_data.print(out)
