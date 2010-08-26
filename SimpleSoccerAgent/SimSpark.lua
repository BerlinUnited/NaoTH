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
		"../NaoRunner/Source/Interface/"}

	  libdirs {"../NaoRunner/dist/", "../NaoRunner/Lib/", "../Extern/lib/",  "../NaoRunner/Lib/win32/lib/"}
	  
	  links {"naoth-simspark", "naointerface", "sfsexp", "wsock32"}
	  
	  targetname "simple-soccer-agent"