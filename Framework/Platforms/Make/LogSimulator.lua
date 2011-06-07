local extern_dir = "../../../Extern"

-- NaoTH controller for the logfile based "simulator"
project "LogSimulator"
  kind "ConsoleApp"
  language "C++"
   
  print("Generating files for logsimulator")
  includedirs {
	"../Source/",
	CORE_PATH,
	"../../NaoTH-Tools/Source/",
	extern_dir .. "/include/",
	extern_dir .. "/include/glib-2.0/",
	extern_dir .. "/lib/glib-2.0/include/"}
  
  files{"../Source/LogSimulator/**.cpp","../Source/LogSimulator/**.h"}
  
  links {CORE, "NaoTH-Commons",
	  "glib-2.0",
	  "gio-2.0",
	  "gobject-2.0",
	  "gmodule-2.0",
	  "gthread-2.0",
	  "protobuf"
	}
	
  targetname "logsimulator"
  
  configuration {"linux"}
    linkoptions {"-Wl,-rpath \"" .. path.getabsolute("../../../Extern/lib/") .. "\""}

  -- END LogSimulator
