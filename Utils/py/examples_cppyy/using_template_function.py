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

    cppyy.include("Tools/Math/ConvexHull.h")
    cppyy.include("Tools/Math/Vector2.h")

    x = cppyy.gbl.std.vector[cppyy.gbl.Vector2d]()
    x.push_back(cppyy.gbl.Vector2d(0.0, 0.0))
    x.push_back(cppyy.gbl.Vector2d(1.0, 1.0))
    x.push_back(cppyy.gbl.Vector2d(-1.0, 1.0))
    x.push_back(cppyy.gbl.Vector2d(0.0, 0.5))
    y = cppyy.gbl.ConvexHull.convexHull[cppyy.gbl.Vector2d](x)

    y = list(y)
    for elem in y:
        print(elem.x, elem.y)
