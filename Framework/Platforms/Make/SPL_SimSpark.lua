-- NaoTH controller for SimSpark
project "SPL_SimSpark"
  kind "StaticLib"
  language "C++"

  -- this is the only difference to SimSpark
  defines{ "PLATFORM_NAME=SPL_SimSpark" }  
  
  files{ "../Source/SimSpark/**.cpp", "../Source/SimSpark/**.h"}
  
  includedirs {
	"../Source/",
	"../../NaoTH-Tools/Source/"
	}
  
  links {
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

-- END SPL_SimSpark