-- NaoTH controller for the logfile based "simulator"
project "LoLa"
  kind "ConsoleApp"
  language "C++"
  
  includedirs {
    "../Source/LoLa/",
    FRAMEWORK_PATH .. "/Platforms/Source"
	}
  
  files {
    "../Source/LoLa/**.cpp",
    "../Source/LoLa/**.h"
	}
  
  links {
    --"NaoRobot"
    "gio-2.0",
    "glib-2.0",
    "gobject-2.0"
	}
	
  targetname "lola"
