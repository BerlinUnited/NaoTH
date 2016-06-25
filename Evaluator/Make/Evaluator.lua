-- NaoTH controller for the logfile based "simulator"
project "Evaluator"
  kind "ConsoleApp"
  language "C++"
  
  includedirs {
	"../Source/",
	"../../NaoTHSoccer/Source/",
	"../../Framework/Commons/Source/"
	}
  
  files {
	"../Source/**.cpp",
	"../Source/**.h",
	"../Source/**.hpp"
	}
  
  links {
	"naoth-soccer",
	"commons",
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
	"opencv_imgcodecs",
	
	}
	
  targetname "evaluator"
