dofile "helper/naocrosscompile.lua"


solution "NaoTHSoccer"
  platforms {"Native", "Nao"}
  configurations {"Debug", "Release"}
  targetdir "../dist"
  
  -- debug configuration
  configuration { "Debug" }
	defines { "DEBUG" }
	flags { "Symbols" }
	
  configuration{"Native"}
	includedirs {
		"../../Extern/include/glib-2.0/",
		"../../Extern/lib/glib-2.0/include/"}
		
	-- additional defines for windows
	if os.is("windows") then
		defines {"WIN32", "NOMINMAX"}
	end

	
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
