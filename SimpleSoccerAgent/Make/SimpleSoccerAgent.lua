project "SimpleSoccerAgent"
	  kind "StaticLib"
	  language "C++"
	  
	  files {"../src/**.h","../src/**.cpp"}
	  
	  includedirs {
		  "../src/"
		}
	  
	  links {"NaoTH-Commons"}
	  
	  targetname "simple-soccer-agent"
