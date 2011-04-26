--dofile "../../LuaScripts/netbeans.lua"

-- description of the solution

solution "TestArchitecture"	  
  platforms {"Native"}
  configurations {"Debug","Release"}
  
  if os.is("windows") then
	defines {"WIN32", "NOMINMAX"}
  end

  includedirs {
     "../../Extern/include/",
     "../../Extern/include/glib-2.0/",
     "../../Extern/lib/glib-2.0/include/"
  }
  
   -- debug configuration
  configuration { "Debug" }
	defines { "DEBUG" }
	flags { "Symbols" }
	
  -- store the makefiles the Make/ directory
  location "."
  
  
  -- add projects
  dofile "TestArchitecture.lua"
  dofile "../../Framework/NaoTH-Tools/Make/NaoTHTools.lua"
