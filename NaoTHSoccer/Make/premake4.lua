if os.is("windows") then
  dofile "helper/naocrosscompile_windows.lua"
 else
  dofile "helper/naocrosscompile.lua"
end

solution "NaoTHSoccer" 
  platforms {"Native"}
  configurations {"Debug", "Release"}
  targetdir "../dist"
  
  -- additional defines for windows
  if os.is("windows") then
	defines {"WIN32", "NOMINMAX"}
  end
  
  -- debug configuration
  configuration { "Debug" }
	defines { "DEBUG" }
	flags { "Symbols" }

	
  CORE_PATH = {
	path.getabsolute("../Source/Core/Cognition/"), 
	path.getabsolute("../Source/Core/Motion/") }
	
  CORE = {"NaoTHSoccer", "DebugCommunication"}
		
  -- base
  dofile "../../NaoTH-Tools/Make/NaoTHTools.lua"
  dofile "../../DebugCommunication/Make/DebugCommunication.lua"
  
  -- platforms
  if(_OPTIONS["platform"] == "Nao") then
  	dofile "../../Platforms/Make/DCM.lua"
  else
		dofile "../../Platforms/Make/SimSpark.lua"
		dofile "../../Platforms/Make/Webots.lua"
		dofile "../../Platforms/Make/LogSimulator.lua"		
    dofile "Tests.lua"
  end
  -- the core
  dofile "NaoTHSoccer.lua"
