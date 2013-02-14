-- load the global default settings
dofile "projectconfig.lua"

-- load local user settings if alailable
if os.isfile("projectconfig.user.lua") then
	print("loading local user path settings")
	dofile "projectconfig.user.lua"
end

print("EXTERN_PATH = " .. EXTERN_PATH)

-- include the Nao platform
include (COMPILER_PATH_NAO)

-- description of the solution
solution "TestArchitecture"	  
  platforms {"Native"}
  configurations {"Debug","Release"}
  
  configuration "vs*"
	defines {"WIN32", "NOMINMAX"}
    buildoptions {"/wd4351", -- disable warning: "...new behavior: elements of array..."
				  "/wd4996", -- disable warning: "...deprecated..."
				  "/wd4290"} -- exception specification ignored (typed stecifications are ignored)
	links {"ws2_32"}

  includedirs {
     FRAMEWORK_PATH .. "/NaoTH-Commons/Source/",
     EXTERN_PATH .. "/include/",
     EXTERN_PATH .. "/include/glib-2.0/",
     EXTERN_PATH .. "/lib/glib-2.0/include/"
  }
  
  libdirs {
      EXTERN_PATH .. "/lib"
    }
  
   -- debug configuration
  configuration { "Debug" }
	defines { "DEBUG" }
	flags { "Symbols" }
	
  -- store the makefiles the Make/ directory
  location "."
  
  
  -- add projects
  dofile "TestArchitecture.lua"
  dofile (FRAMEWORK_PATH .. "/NaoTH-Commons/Make/NaoTH-Commons.lua")
