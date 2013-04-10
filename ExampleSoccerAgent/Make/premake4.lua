FRAMEWORK_PATH = path.getabsolute("../../Framework")
EXTERN_PATH = "E:/RoboCup/Robot/toolchain_native/extern"


dofile "helper/qtcreator.lua"

solution "ExampleSoccerAgent" 
  platforms {"Native"}
  configurations {"Debug", "Release"}
  targetdir "../"
  
  libdirs {
	EXTERN_PATH .. "/lib/"
  }
  
  includedirs {
    EXTERN_PATH .. "/include/",
    EXTERN_PATH .. "/include/glib-2.0/",
	EXTERN_PATH .. "/lib/glib-2.0/include/",
	FRAMEWORK_PATH .. "/NaoTH-Commons/Source/"
  }

  -- additional defines for windows
  if os.is("windows") then
	defines {"WIN32", "NOMINMAX"}
    buildoptions {"/wd4351", -- disable warning: "...new behavior: elements of array..."
				  "/wd4996", -- disable warning: "...deprecated..."
				  "/wd4290"} -- exception specification ignored (typed stecifications are ignored)
	links {"ws2_32"}
  end
  
  -- debug configuration
  configuration { "Debug" }
	defines { "DEBUG" }
	flags { "Symbols" }

  -- include the projects:
  -- Commons
  dofile (FRAMEWORK_PATH .. "/NaoTH-Commons/Make/NaoTH-Commons.lua")
  -- Agent
  dofile "ExampleSoccerAgent.lua"
  -- Platform
  dofile (FRAMEWORK_PATH .. "/Platforms/Make/SPL_SimSpark.lua")
	  kind "ConsoleApp"
	  links { "ExampleSoccerAgent", "NaoTH-Commons" }
  
