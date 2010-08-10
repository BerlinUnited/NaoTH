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
  
  files {"src/**.cpp"}
  
  includedirs {"src/", "../NaoRunner/Interface/include", "../NaoRunner/Webots/WebotsSource", webotsHome .. "/include/controller/c/"}

  libdirs {"../NaoRunner/Make/dist",  webotsHome .. "/lib/"}
  
  links {"naointerface","naowebots", "Controller"}


