-- define the extern directory
local extern_dir = "../../../Extern"

-- description of the solution
solution "NaoTH-Tools"
  kind "StaticLib"
  language "C++"
  targetdir "../dist/"
  
  platforms {"Native"}
  configurations {"Debug","Release"}
  
  if os.is("windows") then
    defines {"WIN32", "NOMINMAX"}
  end
  
  includedirs { 
    extern_dir .. "/include",
    extern_dir .. "/include/glib-2.0/",
    extern_dir .. "/lib/glib-2.0/include/"
  }
  
   -- debug configuration
  configuration { "Debug" }
    defines { "DEBUG" }
    flags { "Symbols" }
	
  -- store the makefiles the Make/ directory
  location "."

  -- include the actual project
  dofile "NaoTHTools.lua"
