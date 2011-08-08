solution "SimpleSoccerAgent" 
  platforms {"Native"}
  configurations {"Debug", "Release"}
  targetdir "../"
  
  
  naoth_path = {
	extern = path.getabsolute("../../Extern"), 
	framework = path.getabsolute("../../Framework")
	}
	
  -- additional defines for windows
  if os.is("windows") then
	defines {"WIN32", "NOMINMAX"}
    buildoptions {"/wd4351", -- disable warning: "...new behavior: elements of array..."
				  "/wd4996", -- disable warning: "...deprecated..."
				  "/wd4290"} -- exception specification ignored (typed stecifications are ignored)
	links {"ws2_32"}
  end
  
  libdirs {
	naoth_path.extern .. "/lib/"
  }
  
  includedirs {
    naoth_path.extern .. "/include/",
    naoth_path.extern .. "/include/glib-2.0/",
	naoth_path.extern .. "/lib/glib-2.0/include/",
	naoth_path.framework .. "/NaoTH-Commons/Source/"
  }
  
  -- debug configuration
  configuration { "Debug" }
	defines { "DEBUG" }
	flags { "Symbols" }

  -- define the core
  CORE_PATH = path.getabsolute("../src/")
  CORE = "SimpleSoccerAgent"
  
  dofile (naoth_path.framework .. "/NaoTH-Commons/Make/NaoTH-Commons.lua")
  dofile (naoth_path.framework .. "/Platforms/Make/SimSpark.lua")
  dofile "SimpleSoccerAgent.lua"
