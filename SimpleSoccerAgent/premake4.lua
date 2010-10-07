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

  dofile "../NaoTH-Tools/Make/NaoTHTools.lua"
  dofile "../Platforms/Make/SimSpark.lua"

  dofile "SimSpark.lua"
