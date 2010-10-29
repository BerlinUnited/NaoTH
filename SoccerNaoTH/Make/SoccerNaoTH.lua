project "SoccerNaoTH"
  kind "ConsoleApp"
  language "C++"
  
  files {"../Source/**.h","../Source/**.cpp"}
  
  includedirs {
	"../Source/", 
	"../../Platforms/Source/",
	"../../NaoTH-Tools/Source/", 
	"../../Extern/include/",
	"../../Extern/include/glib-2.0/",
	"../../Extern/lib/glib-2.0/include/"}

  libdirs {
	"../../NaoRunner/dist/", 
	"../../NaoRunner/Lib/", 
	"../../Extern/lib/",  
	"../../NaoRunner/Lib/win32/lib/"}
  
  links {
	"SimSpark", 
	"NaoTH-Tools", 
	"sfsexp", 
	"glib-2.0",
	"gio-2.0",
	"gobject-2.0"}
  
  targetname "naoth-soccer"

