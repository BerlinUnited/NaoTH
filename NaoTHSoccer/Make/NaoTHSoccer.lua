project "NaoTHSoccer"
  kind "StaticLib"
  language "C++"
  
  files {"../Source/**.h","../Source/**.cpp"}
  
  includedirs {
	  "../Source/"
	}
  
  links { "NaoTH-Commons" }
  
  targetname "naoth-soccer"

