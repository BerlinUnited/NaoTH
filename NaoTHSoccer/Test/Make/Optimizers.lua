-- NaoTH controller for the logfile based "simulator"
project "Optimizers"
  kind "ConsoleApp"
  language "C++"
  
  includedirs {
    "../Source/Optimizers/",
	}
  
  files {
    "../Source/Optimizers/**.cpp",
    "../Source/Optimizers/**.h",
    "../Source/Optimizers/**.hpp"
	}
  
  links {
	}
	
  targetname "optimizers"
