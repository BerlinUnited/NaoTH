project "NaoTHSoccer"
  kind "StaticLib"
  language "C++"
  
  files {"../Source/**.h","../Source/**.cpp"}
  
  includedirs {
	  "../Source/Core/"
	}
  
  links { "NaoTH-Commons" }
  
  targetname "naoth-soccer"

