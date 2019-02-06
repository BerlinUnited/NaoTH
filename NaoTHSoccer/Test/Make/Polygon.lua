-- NaoTH controller for the logfile based "simulator"
project "Polygon"
  kind "ConsoleApp"
  language "C++"
  
  includedirs {
    "../Source/Polygon/",
	}
  
  files {
    "../Source/Polygon/**.cpp",
    "../Source/Polygon/**.h",
    "../Source/Polygon/**.hpp"
	}
  
  links {
	}
	
  targetname "polygon"
