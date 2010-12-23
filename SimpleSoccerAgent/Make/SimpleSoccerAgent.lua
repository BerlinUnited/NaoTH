project "SimpleSoccerAgent"
	  kind "StaticLib"
	  language "C++"
	  
	  files {"../src/**.h","../src/**.cpp"}
	  
	  includedirs {
		"../src/", 
		"../../NaoTH-Tools/Source/",
		"../../Extern/include/glib-2.0/",
		"../../Extern/lib/glib-2.0/include/"}

	  libdirs {"../../Extern/lib/"}
	  
	  links {"NaoTH-Tools"}
	  
	  targetname "simple-soccer-agent"
