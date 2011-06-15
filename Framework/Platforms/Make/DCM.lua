-- NaoTH controller running in the real robot

  
------------------------- AL_DIR -----------------------------
local alDir = os.getenv("AL_DIR")

-- check if the directory can be opened by lua and warn if not
if(alDir == nil) then
   print("WARN: Enviroment variale AL_DIR was not set or unable to access directory (" .. alDir .. ")!")
   print("(PROBABLY) no compilation for DCM possible.\n")
else
   print("INFO: Configuring with NaoQi located in \"" .. alDir .. "\"")
end
----------------------------------------------------------------

if(CORE_PATH == nil or CORE == nil) then
   print("WARNING: no path for the core is set")
   CORE_PATH = "."
   CORE = ""
end

project "libnaoth"
  kind "SharedLib"
  language "C++"
  
  print("Generating files for libnaoth")

  libdirs { alDir .. "/lib/"}

  includedirs {
     "../Source/DCM", 
     CORE_PATH,
     "../../NaoTH-Tools/Source/",
     alDir .. "/include/",
     alDir .. "/include/alfactory/"
  }

  links {CORE, 
	 "NaoTH-Commons",
	 "almemoryfastaccess", 
	 "alcommon",
	 "gthread-2.0", 
	 "glib-2.0",
	 "gio-2.0",
	 "gobject-2.0",
	 "protobuf"
      }

  files{"../Source/DCM/libnaoth/**.cpp", "../Source/DCM/libnaoth/**.h",
	"../Source/DCM/Tools/**.cpp", "../Source/DCM/Tools/**.h"
     }
  targetname "naoth"
  
project "naoth"
  kind "ConsoleApp"
  language "C++"
  
  print("Generating files for naoth")

  libdirs { alDir .. "/lib/"}

  includedirs {
     "../Source/DCM", 
     CORE_PATH,
     "../../NaoTH-Tools/Source/",
     alDir .. "/include/",
     alDir .. "/include/alfactory/"
  }

  links {CORE, 
	 "NaoTH-Commons",
	 -- "almemoryfastaccess", 
	 -- "alcommon",
	 "gthread-2.0", 
	 "glib-2.0",
	 "gio-2.0",
	 "z",
	 "gmodule-2.0",
	 "gobject-2.0",
	 "protobuf", 
	 "asound",
	 -- "rttools"
      }

  files{"../Source/DCM/naoth/**.cpp", "../Source/DCM/naoth/**.h",
	"../Source/DCM/Tools/**.cpp", "../Source/DCM/Tools/**.h"}
  targetname "naoth"
-- END DCM
