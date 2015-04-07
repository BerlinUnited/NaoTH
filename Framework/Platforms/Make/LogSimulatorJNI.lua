-- NaoTH controller for the logfile based "simulator"
project "LogSimulatorJNI"
  kind "SharedLib"
  language "C++"
  
  includedirs {
	"../Source/"
	}
  
  files {
    "../Source/LogSimulator/**.cpp",
	"../Source/LogSimulator/**.h",
	"../Source/LogSimulatorJNI/**.cpp",
	"../Source/LogSimulatorJNI/**.h"
	}
  
  links {
	"glib-2.0",
	"gio-2.0",
	"gobject-2.0",
	"gmodule-2.0",
	"gthread-2.0",
	"protobuf"
	}
	
  targetname "logsimulator"
