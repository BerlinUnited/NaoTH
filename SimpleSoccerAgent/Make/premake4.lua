solution "SimpleSoccerAgent" 
  platforms {"Native"}
  configurations {"Debug", "Release"}
  targetdir "../"
  
  -- extern
  EXTERN_PATH = "../../Extern"
  -- framework
  FRAMEWORK_PATH = "../../Framework"
  
  
  -- additional defines for windows
  if os.is("windows") then
	defines {"WIN32", "NOMINMAX"}
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
  CORE_PATH = path.getabsolute("../src/")
  CORE = "SimpleSoccerAgent"
  
  dofile (FRAMEWORK_PATH .. "/NaoTH-Commons/Make/NaoTH-Commons.lua")
  dofile (FRAMEWORK_PATH .. "/Platforms/Make/SimSpark.lua")
  dofile "SimpleSoccerAgent.lua"
