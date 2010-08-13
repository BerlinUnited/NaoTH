solution "TestBehavior"
  project "WebotsTest"

  kind "ConsoleApp"
  language "C++"
  
  local webotsHome = os.getenv("WEBOTS_HOME")
  if(webotsHome == nil) then
    print("WARN: Enviroment variale WEBOTS_HOME was not set. No compilation for Webots possible.")
    print("WARN: I will default to a nonsense-value")
    webotsHome = "<WEBOTS_HOME_not_set>";
  end
  
  platforms {"Native"}
  configurations {"Debug", "Release"}
  
  if os.is("windows") then
	defines {"WIN32", "NOMINMAX"}
  end
  
  files {"src/**.cpp"}
  
  includedirs {"src/", "../NaoRunner/Include/", webotsHome .. "/include/controller/c/"}

  libdirs {"../NaoRunner/Lib/",  webotsHome .. "/lib/"}
  
  links {"naowebots", "Controller"}
  
  targetname "nao_soccer_player_red"


