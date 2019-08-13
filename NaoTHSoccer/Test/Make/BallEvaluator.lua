-- NaoTH controller for the logfile based "simulator"
project "BallEvaluator"
  kind "ConsoleApp"
  language "C++"
  
  includedirs {
    "../Source/BallEvaluator/",
    "../../Source/"
	}
  
  files {
    "../Source/BallEvaluator/**.cpp",
    "../Source/BallEvaluator/**.h",
    "../Source/BallEvaluator/**.hpp"
	}
  
  links {
    "NaoTHSoccer",
    "Commons",
    "glib-2.0",
    "gio-2.0",
    "gobject-2.0",
    "gmodule-2.0",
    "gthread-2.0",
    "protobuf",
    "opencv_core",
    "opencv_ml",
    "opencv_imgproc",
    "opencv_objdetect",
    "opencv_imgcodecs"
	}
	
  targetname "ballevaluator"
