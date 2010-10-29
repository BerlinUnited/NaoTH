solution "SimpleSoccerAgent" 
  platforms {"Native"}
  configurations {"Debug", "Release"}
  targetdir "./dist/"
  
  -- additional defines for windows
  if os.is("windows") then
	defines {"WIN32", "NOMINMAX"}
  end
  
  -- debug configuration
  configuration { "Debug" }
	defines { "DEBUG" }
	flags { "Symbols" }

	
  -- base
  dofile "../../NaoTH-Tools/Make/NaoTHTools.lua"
  
  -- platforms
  dofile "../../Platforms/Make/SimSpark.lua"

  dofile "SoccerNaoTH.lua"
