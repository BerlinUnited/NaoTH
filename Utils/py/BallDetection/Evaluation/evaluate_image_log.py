import os
import argparse
import cppyy

def get_naoth_dir():
    script_path = os.path.abspath(__file__)
    return os.path.abspath(os.path.join(script_path, "../../../../../"))

def get_toolchain_dir():
    return os.path.abspath(os.environ["NAOTH_TOOLCHAIN_PATH"])

def get_files_of_dir(d):
    imageFiles = [os.path.join(d,f) for f in os.listdir(d) if os.path.isfile(os.path.join(d,f)) and f.endswith(".png")]

    # TODO: add camera matrix to result
    # TODO: parse XML and add ground truth to result

    return imageFiles


class Evaluator:

    def __init__(self):
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
        self.sim = cppyy.gbl.DummySimulator(False, False, 5401)
        cppyy.gbl.naoth.Platform.getInstance().init(self.sim)

        # create the cognition and motion objects
        cog = cppyy.gbl.createCognition()
        mo = cppyy.gbl.createMotion()

        # cast to callable
        callable_cog = cppyy.bind_object(
            cppyy.addressof(cog), cppyy.gbl.naoth.Callable)
        callable_mo = cppyy.bind_object(
            cppyy.addressof(mo), cppyy.gbl.naoth.Callable)

        self.sim.registerCognition(callable_cog)
        self.sim.registerMotion(callable_mo)

        # get access to the module manager and return it to the calling function
        cppyy.include(os.path.join(naoth_dir, "Framework/Commons/Source/ModuleFramework/ModuleManager.h"))
        cppyy.include(os.path.join(naoth_dir, "NaoTHSoccer/Source/Cognition/Cognition.h"))
        
        self.moduleManager = cppyy.gbl.getModuleManager(cog)

        # get the ball detector module
        self.ball_detector = self.moduleManager.getModule("CNNBallDetector").getModule()

    def execute(self, directories):
        for d in directories:
            imageFiles = get_files_of_dir(d)
            for f in imageFiles:
                self.set_images_from_file(f)
                self.sim.executeFrame()        
            

    def set_images_from_file(self, file, bottom=True):
        # get reference to the image input representation
        if bottom:
            image = self.ball_detector.getRequire().at("Image")
        else:
            image = self.ball_detector.getRequire().at("ImageTop")

        print(file)
        # TODO: read  image from file with opencv
        # TODO: convert image
        # TODO: move image into representation


if __name__ == "__main__":

    parser = argparse.ArgumentParser(description='Evaluate ball detection on logfile images annotated with CVAT.')
    parser.add_argument('directory', nargs='+',
                        help='A list of directories containing the images files (as png with the integrated camera matrix)')

    args = parser.parse_args()

    evaluator = Evaluator()
    evaluator.execute(args.directory)
