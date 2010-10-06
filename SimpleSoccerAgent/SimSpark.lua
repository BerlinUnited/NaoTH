project "SimpleSoccerAgent"
	  kind "ConsoleApp"
	  language "C++"
	  
	  files {"src/**.h","src/**.cpp"}
	  
	  includedirs {
		"src/", 
		"../NaoRunner/Source/", 
		"../Extern/include/",
		"../NaoRunner/Source",
		"../NaoRunner/Lib/", 
		"../NaoRunner/Lib/win32/include/", 
		"../NaoRunner/Source/Interface/",
		"../Extern/include/glib-2.0/",
		"../Extern/lib/glib-2.0/include/"}

	  libdirs {"../NaoRunner/dist/", "../NaoRunner/Lib/", "../Extern/lib/",  "../NaoRunner/Lib/win32/lib/"}
	  
	  links {"naoth-simspark", "naointerface", "sfsexp", "glib-2.0","gio-2.0","gobject-2.0"}
	  
	  targetname "simple-soccer-agent"
