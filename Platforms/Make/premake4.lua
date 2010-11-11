-- description of the solution

solution "NaoPlatform"
  kind "StaticLib"
  language "C++"
  targetdir "../dist/"
  
  platforms {"Native"}
  configurations {"Debug","Release"}
  
  if os.is("windows") then
	defines {"WIN32", "NOMINMAX"}
  end
  
      -- debug configuration
  configuration { "Debug" }
    defines { "DEBUG" }
    flags { "Symbols" }
	
  -- store the makefiles the Make/ directory
  location "."
  
  dofile "Webots.lua"
  dofile "SimSpark.lua"
  dofile "../../NaoTH-Tools/Make/NaoTHTools.lua"
