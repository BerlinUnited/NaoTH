-- NaoTH controller for the logfile based "simulator"
project "LoLa"
  kind "ConsoleApp"
  language "C++"
  
  includedirs {
    "../Source/LoLa/",
	}
  
  files {
    "../Source/LoLa/**.cpp",
    "../Source/LoLa/**.h"
	}
  
  links {
	}
	
  targetname "lola"
