project "SimpleSoccerAgent"
	  kind "StaticLib"
	  language "C++"
	  
	  files {"src/**.h","src/**.cpp"}
	  
	  includedirs {
		"src/", 
		"../NaoTH-Tools/Source/"}

	  libdirs {"../../Extern/lib/"}
	  
	  links {"NaoTH-Tools"}
	  
	  targetname "simple-soccer-agent"
