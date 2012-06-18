project "NaoTHSoccer"
  kind "StaticLib"
  language "C++"
  
  flags {"FatalWarnings"}
  
  files {"../Source/**.h","../Source/**.cpp"}
  
  includedirs {
	"../Source/Core/"
	}
  
  --links { "NaoTH-Commons" }
  
  targetname "naoth-soccer"

