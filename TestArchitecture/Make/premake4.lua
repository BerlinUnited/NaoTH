--dofile "../../LuaScripts/netbeans.lua"

-- description of the solution

solution "TestArchitecture"	  
  platforms {"Native"}
  configurations {"Debug","Release"}
  
  if os.is("windows") then
	defines {"WIN32", "NOMINMAX"}
    buildoptions {"/wd4351", -- disable warning: "...new behavior: elements of array..."
				  "/wd4996", -- disable warning: "...deprecated..."
				  "/wd4290"} -- exception specification ignored (typed stecifications are ignored)
	links {"ws2_32"}
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
  dofile "../../Framework/NaoTH-Commons/Make/NaoTH-Commons.lua"
