dofile "helper/naocrosscompile.lua"

-- some local helper variables
local isGCC = false; 
-- check if using gcc as compiler
if premake.action.current().valid_tools ~= nil and premake.action.current().valid_tools.cc[1] == "gcc" then
  isGCC = true
end

-- definition of the solution
solution "NaoTHSoccer"
  platforms {"Native", "Nao"}
  configurations {"Debug", "Release"}
  
  
  -- debug configuration
  configuration { "Debug" }
  defines { "DEBUG" }
  flags { "Symbols" }
  
  -- additional defines for windows
  if os.is("windows") then
    defines {"WIN32", "NOMINMAX"}
  end
    
  if isGCC then    
    print "ADDING FPIC"
    buildoptions {"-fPIC"}
  end
  
  configuration{"Native"}
    includedirs {
      "../../Extern/include/",
      "../../Extern/include/glib-2.0/",
      "../../Extern/lib/glib-2.0/include/"}
    libdirs {
      "../../Extern/lib"
    }
    targetdir "../dist/Native"
    
  configuration {"Nao"}
    targetdir "../dist/Nao"
  
  
  CORE_PATH = {
  path.getabsolute("../Source/Core/Cognition/"), 
  path.getabsolute("../Source/Core/Motion/") }
  
  CORE = {"NaoTHSoccer", "DebugCommunication"}
    
  -- base
  dofile "../../Framework/NaoTH-Tools/Make/NaoTHTools.lua"
  dofile "../../Framework/DebugCommunication/Make/DebugCommunication.lua"
  
  -- platforms
  if(_OPTIONS["platform"] == "Nao") then
    dofile "../../Framework/Platforms/Make/DCM.lua"
  else
    dofile "../../Framework/Platforms/Make/SimSpark.lua"
    dofile "../../Framework/Platforms/Make/Webots.lua"
    dofile "../../Framework/Platforms/Make/LogSimulator.lua"    
    dofile "Tests.lua"
  end
  -- the core
  dofile "NaoTHSoccer.lua"
