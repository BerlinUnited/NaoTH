-- NaoTH controller running in the real robot

  
  ------------------------- AL_DIR -----------------------------
  -- it's set in the global project config now
  -- local AL_DIR = os.getenv("AL_DIR")

  -- check if the directory can be opened by lua and warn if not
  if(AL_DIR == nil) then
    AL_DIR = "/opt/aldebaran/info/naoqi"
    print("Error: Enviroment variable AL_DIR not set. trying default: \"" .. AL_DIR .. "\"")
  end
-- if(AL_DIR == nil) then
--    print("WARN: Enviroment variale AL_DIR was not set or unable to access directory (" .. AL_DIR .. ")!")
--	print("(PROBABLY) no compilation for DCM possible.\n")
-- else
--    print("INFO: Configuring with NaoQi located in \"" .. AL_DIR .. "\"")
--  end
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

  libdirs { AL_DIR .. "/lib/"}

  includedirs {
	"../Source/DCM", 
	CORE_PATH,
	"../../NaoTH-Tools/Source/",
	AL_DIR .. "/include/",
	AL_DIR .. "/include/alfactory/"
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

  files{
	"../Source/DCM/libnaoth/**.cpp", 
	"../Source/DCM/libnaoth/**.h",
	"../Source/DCM/Tools/**.cpp", 
	"../Source/DCM/Tools/**.h"
	}
  targetname "naoth"
  
  
project "naoth"
  kind "ConsoleApp"
  language "C++"
  
  print("Generating files for naoth")

  f = io.popen ("bzr revno", "r");
  defines{ "BZR_REVISION=" .. f:read("*l") } 
  f:close ();

  -- f = io.popen ("bzr info", "r");
  -- defines{ "BZR_BRANCHINFO=" .. f:read("*l") } 
  -- f:close ();  

  libdirs { AL_DIR .. "/lib/"}

  includedirs {
     "../Source/DCM", 
     CORE_PATH,
     "../../NaoTH-Tools/Source/",
     AL_DIR .. "/include/",
     AL_DIR .. "/include/alfactory/"
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
	"opencv_core",
	"opencv_imgproc",
	"opencv_highgui",
	"opencv_ml",
	"asound"
	 -- "rttools"
	}

  files{
	"../Source/DCM/naoth/**.cpp", 
	"../Source/DCM/naoth/**.h",
	"../Source/DCM/Tools/**.cpp", 
	"../Source/DCM/Tools/**.h"
	}
  targetname "naoth"
  
-- END DCM
