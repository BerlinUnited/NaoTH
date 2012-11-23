dofile "helper/qtcreator.lua"

solution "ExampleSoccerAgent" 
  platforms {"Native"}
  configurations {"Debug", "Release"}
  targetdir "../"
  
  FRAMEWORK_PATH = path.getabsolute("../../Framework")
  EXTERN_PATH = path.getabsolute("../../Extern")
	
  -- additional defines for windows
  if os.is("windows") then
	defines {"WIN32", "NOMINMAX"}
    buildoptions {"/wd4351", -- disable warning: "...new behavior: elements of array..."
				  "/wd4996", -- disable warning: "...deprecated..."
				  "/wd4290"} -- exception specification ignored (typed stecifications are ignored)
	links {"ws2_32"}
  end
  
  libdirs {
	  EXTERN_PATH .. "/lib/"
  }
  
  includedirs {
    EXTERN_PATH .. "/include/",
    EXTERN_PATH .. "/include/glib-2.0/",
	  EXTERN_PATH .. "/lib/glib-2.0/include/",
	  FRAMEWORK_PATH .. "/NaoTH-Commons/Source/"
  }
  
  -- debug configuration
  configuration { "Debug" }
	defines { "DEBUG" }
	flags { "Symbols" }

  -- define the core
  CORE_PATH = {
    path.getabsolute("../Source/Cognition"),
    path.getabsolute("../Source/Motion"),
    path.getabsolute("../Source/")
  }
  CORE = "ExampleSoccerAgent"
  
  dofile (FRAMEWORK_PATH .. "/NaoTH-Commons/Make/NaoTH-Commons.lua")
  dofile (FRAMEWORK_PATH .. "/Platforms/Make/SimSpark.lua")
  dofile "ExampleSoccerAgent.lua"
