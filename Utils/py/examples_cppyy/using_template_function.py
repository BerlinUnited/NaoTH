import cppyy;

cppyy.load_library("../LinuxToolchain/toolchain_native/extern/lib/libprotobuf.so")
cppyy.load_library("/usr/lib/libglib-2.0.so")
cppyy.load_library("NaoTHSoccer/dist/Native/libcommons.so")

cppyy.add_include_path("Framework/Commons/Source")

cppyy.include("Tools/Math/ConvexHull.h")
cppyy.include("Tools/Math/Vector2.h")

from cppyy.gbl import Vector2d
from cppyy.gbl.std import vector

x = vector[cppyy.gbl.Vector2d]()
x.push_back(Vector2d(0.0,0.0))
x.push_back(Vector2d(1.0,1.0))
x.push_back(Vector2d(-1.0,1.0))
x.push_back(Vector2d(0.0,0.5))
y = cppyy.gbl.ConvexHull.convexHull[Vector2d](x)

