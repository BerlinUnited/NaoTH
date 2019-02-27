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
	}
	
  targetname "lola"
