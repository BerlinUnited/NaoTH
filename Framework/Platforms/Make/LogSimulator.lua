-- local EXTERN_PATH = "../../../Extern"

-- NaoTH controller for the logfile based "simulator"
project "LogSimulator"
  kind "ConsoleApp"
  language "C++"
   
  print("Generating files for logsimulator")
  includedirs {
	"../Source/",
	CORE_PATH,
	"../../NaoTH-Tools/Source/",
	EXTERN_PATH .. "/include/",
	EXTERN_PATH .. "/include/glib-2.0/",
	EXTERN_PATH .. "/lib/glib-2.0/include/"}
  
  files{"../Source/LogSimulator/**.cpp","../Source/LogSimulator/**.h"}
  
  links {CORE, 
	"NaoTH-Commons",
	"glib-2.0",
	"gio-2.0",
	"gobject-2.0",
	"gmodule-2.0",
	"gthread-2.0",
	"protobuf",
	"opencv_core",
	"opencv_ml",
	"opencv_highgui",
	"opencv_imgproc"
	}
	
  targetname "logsimulator"
  
  configuration {"linux"}
    linkoptions {"-Wl,-rpath \"" .. path.getabsolute(EXTERN_PATH .. "/lib/") .. "\""}

  -- END LogSimulator
