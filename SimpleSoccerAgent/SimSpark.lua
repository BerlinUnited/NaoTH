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
	  
	  links {"naoth-simspark", "naointerface", "sfsexp"}
	  -- additional libraries for windows
	  if os.is("windows") then
	     links {"wsock32"}
	  end
	  
	  targetname "simple-soccer-agent"