project "NaoTHSoccer"
  kind "StaticLib"
  language "C++"
  
  files {"../Source/**.h","../Source/**.cpp"}
  
  includedirs {
	"../Source/",
	"../../NaoTH-Tools/Source/",
	"../../DebugCommunication/Source/",
	"../../Extern/include/glib-2.0/",
	"../../Extern/lib/glib-2.0/include/"}

  libdirs {"../../Extern/lib/"}
  
  links { "NaoTH-Tools", "DebugCommunication" }
  
  targetname "naoth-soccer"

