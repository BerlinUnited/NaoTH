-- controller for SimSpark simulator
project "SimSpark"
  kind "StaticLib"
  language "C++"
        
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
	"protobuf"
   }
  
  targetname "naoth-simspark"

-- END SimSpark
