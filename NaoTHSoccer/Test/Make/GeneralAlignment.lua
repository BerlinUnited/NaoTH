-- NaoTH controller for the logfile based "simulator"
project "GeneralAlignment"
  kind "ConsoleApp"
  language "C++"
  
  includedirs {
    "../Source/GeneralAlignment/",
	}
  
  files {
    "../Source/GeneralAlignment/**.cpp",
    "../Source/GeneralAlignment/**.h",
    "../Source/GeneralAlignment/**.hpp"
	}
  
  links {
	}
	
  targetname "generalalignment"
