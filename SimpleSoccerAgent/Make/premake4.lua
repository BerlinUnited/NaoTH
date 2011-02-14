solution "SimpleSoccerAgent" 
  platforms {"Native"}
  configurations {"Debug", "Release"}
  targetdir "../"
  
  -- additional defines for windows
  if os.is("windows") then
	defines {"WIN32", "NOMINMAX"}
  end
  
  
  libdirs {"../../Extern/lib/"}
  includedirs {
    "../../Extern/include/glib-2.0/",
		"../../Extern/lib/glib-2.0/include/"
  }
  
  -- debug configuration
  configuration { "Debug" }
	defines { "DEBUG" }
	flags { "Symbols" }

  -- define the core
  CORE_PATH = path.getabsolute("../src/")
  CORE = "SimpleSoccerAgent"
  
  dofile "../../Framework/NaoTH-Tools/Make/NaoTHTools.lua"
  dofile "SimpleSoccerAgent.lua"
  dofile "../../Framework/Platforms/Make/SimSpark.lua"
