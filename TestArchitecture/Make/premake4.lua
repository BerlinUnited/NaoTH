dofile "../../LuaScripts/netbeans.lua"

-- description of the solution

solution "TestArchitecture"	  
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
  
  
  -- add projects
  dofile "TestArchitecture.lua"
  dofile "../../NaoTH-Tools/Make/NaoTHTools.lua"
