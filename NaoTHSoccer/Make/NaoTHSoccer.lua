project "NaoTHSoccer"
  kind "StaticLib"
  language "C++"
  
  files {"../Source/**.h","../Source/**.cpp"}
  
  includedirs {
	"../Source/",
	"../../NaoTH-Tools/Source/"}

  libdirs {"../../Extern/lib/"}
  
  links { "NaoTH-Tools" }
  
  targetname "naoth-soccer"

