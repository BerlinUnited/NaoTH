-- NaoTH controller for the logfile based "simulator"
project "Evaluator"
  kind "ConsoleApp"
  language "C++"
  
  includedirs {
	"../Source/"
	}
  
  files {
	"../Source/Evaluator/**.cpp",
	"../Source/Evaluator/**.h"
	}
  
  links {
	"glib-2.0",
	"gio-2.0",
	"gobject-2.0",
	"gmodule-2.0",
	"gthread-2.0",
	"protobuf"
	}
	
  targetname "evaluator"
