project "NaoTHSoccer"
  kind "StaticLib"
  language "C++"
  
  files {"../Source/**.h","../Source/**.cpp"}
  
  includedirs {
	"../Source/",
	"../../NaoTH-Tools/Source/",
	"../../DebugCommunication/Source/"}

  libdirs {"../../Extern/lib/"}
  
  links { "NaoTH-Tools", "DebugCommunication" }
  
  targetname "naoth-soccer"

