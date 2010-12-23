solution "SimpleSoccerAgent" 
  platforms {"Native"}
  configurations {"Debug", "Release"}
  
  -- additional defines for windows
  if os.is("windows") then
	defines {"WIN32", "NOMINMAX"}
  end
  
  -- debug configuration
  configuration { "Debug" }
	defines { "DEBUG" }
	flags { "Symbols" }

  -- define the core
  CORE_PATH = path.getabsolute("src/")
  CORE = "SimpleSoccerAgent"
  
  dofile "../NaoTH-Tools/Make/NaoTHTools.lua"
  dofile "SimpleSoccerAgent.lua"
  dofile "../Platforms/Make/SimSpark.lua"
