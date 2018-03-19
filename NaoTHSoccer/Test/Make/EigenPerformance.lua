-- NaoTH controller for the logfile based "simulator"
project "EigenPerformance"
  kind "ConsoleApp"
  language "C++"
  
  includedirs {
    "../Source/EigenPerformance/",
	}
  
  files {
    "../Source/EigenPerformance/**.cpp",
    "../Source/EigenPerformance/**.h",
    "../Source/EigenPerformance/**.hpp"
	}
  
  links {
	}
	
  targetname "eigenperformance"
