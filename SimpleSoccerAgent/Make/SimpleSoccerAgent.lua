project "SimpleSoccerAgent"
	  kind "StaticLib"
	  language "C++"
	  
	  files {"../src/**.h","../src/**.cpp"}
	  
	  includedirs {
		  "../src/", 
		  "../../Framework/NaoTH-Tools/Source/"
		}
	  
	  links {"NaoTH-Tools"}
	  
	  targetname "simple-soccer-agent"
