project "TestArchitecture"
  kind "ConsoleApp"
  language "C++"
  targetdir "../dist/"
  files{ "../Source/**.cpp", "../Source/**.h"}

  includedirs {
	"../Source/"
  }

  links {
    "NaoTH-Commons", 
	"gthread-2.0",
    "glib-2.0",
	"gio-2.0",
	"gobject-2.0",
	"gmodule-2.0",
	"protobuf"}
	
  configuration "linux"
    links {"z"}
