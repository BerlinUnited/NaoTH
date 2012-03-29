local extern_dir = "../../../Extern"

-- NaoTH controller for SimSpark
project "SPL_SimSpark"
  kind "ConsoleApp"
  language "C++"

  -- this is the only difference to SimSpark
  defines{ "PLATFORM_NAME=SPL_SimSpark" }  
  
  files{ "../Source/SimSpark/**.cpp", "../Source/SimSpark/**.h"}
  
  -- include core
  if(CORE_PATH == nil or CORE == nil) then
    print("WARNING: no path for the core is set")
	CORE_PATH = "."
	CORE = ""
  end

  includedirs {
	"../Source/",
	CORE_PATH,
	"../../NaoTH-Tools/Source/",
	extern_dir .. "/include/",
	extern_dir .. "/include/glib-2.0/",
	extern_dir .. "/lib/glib-2.0/include/"}

  
  links {
    CORE,
	  "NaoTH-Commons", 
	  "sfsexp",
	  "glib-2.0",
	  "gio-2.0",
	  "gobject-2.0",
	  "gmodule-2.0",
	  "gthread-2.0",
	  "protobuf",
	  "opencv_core",
    "opencv_imgproc",
    "opencv_ml"
   }

  targetname "naoth-simspark-spl"
  
-- removed by fh :) Why? OpenCV is always dynamically linked and we can only garantuee that there is one version in Extern (Thomas)
  configuration {"linux"}
    linkoptions {"-Wl,-rpath \"" .. path.getabsolute("../../../Extern/lib/") .. "\""}

-- END SPL_SimSpark