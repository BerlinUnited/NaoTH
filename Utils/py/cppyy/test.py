import cppyy

# load shared libraries
cppyy.load_library("../LinuxToolchain/toolchain_native/extern/lib/libprotobuf.so")
cppyy.load_library("/usr/lib/libglib-2.0.so")
cppyy.load_library("NaoTHSoccer/dist/Native/libcommons.so")
cppyy.load_library("NaoTHSoccer/dist/Native/libnaoth-soccer.so")

# set include paths
cppyy.add_include_path("Framework/Commons/Source")

# example for manipulating a representation object (not on blackboard)
# cppyy.include("NaoTHSoccer/Source/Representations/Modeling/IMUData.h")
#
# from cppyy.gbl import naoth
# IMUData = naoth.IMUData
# x = IMUData()
# x.location.x = 5
# out = cppyy.gbl.std.cout
# x.print(out)

# example for manipulating a module
cppyy.add_include_path("NaoTHSoccer/Source")
cppyy.add_include_path("/usr/include/glib-2.0")
cppyy.add_include_path("/usr/lib/glib-2.0/include")
cppyy.add_include_path("/home/steffen/NaoTH/LinuxToolchain/toolchain_native/extern/include")

cppyy.include("NaoTHSoccer/Source/Motion/SensorFilter/IMUModel.h")

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
