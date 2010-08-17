solution "TestBehavior" 
  platforms {"Native"}
  configurations {"Debug", "Release"}
  
  -- additional defines for windows
  if os.is("windows") then
	defines {"WIN32", "NOMINMAX"}
  end
  
  -- debug configuration
  configuration { "Debug" }
	defines { "DEBUG" }
	flags { "Symbols" }
		
  project "WebotsTest"
	  kind "ConsoleApp"
	  language "C++"
	  
	  local webotsHome = os.getenv("WEBOTS_HOME")
	  if(webotsHome == nil) then
		print("WARN: Enviroment variale WEBOTS_HOME was not set. No compilation for Webots possible.")
		print("WARN: I will default to a nonsense-value")
		webotsHome = "<WEBOTS_HOME_not_set>";
	  end
	  
	  files {"src/**.h","src/**.cpp"}
	  
	  includedirs {
		"src/", 
		"../NaoRunner/Source/", 
		webotsHome .. "/include/controller/c/", 
		"../NaoRunner/Source",
		"../NaoRunner/Lib/", 
		"../NaoRunner/Lib/win32/include/", 
		"../NaoRunner/Source/Interface/"}

	  libdirs {"../NaoRunner/Lib/",  webotsHome .. "/lib/", "../NaoRunner/Lib/win32/lib/"}
	  
	  links {"naoth-simspark", "sfsexp", "pthread", "Controller", "ws2_32"}
	  
	  targetname "nao_soccer_player_red"


