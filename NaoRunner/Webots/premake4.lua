-- NaoTH controller running in the Cyberbotics Webots simulator
project "Webots"
  kind "StaticLib"
  language "C++"
  
  local webotsHome = os.getenv("WEBOTS_HOME")
  if(webotsHome == nil) then
    print("WARN: Enviroment variale WEBOTS_HOME was not set. No compilation for Webots possible.")
    print("WARN: I will default to a nonsense-value")
    webotsHome = "<WEBOTS_HOME_not_set>";
  end

  includedirs {"WebotsSource/", "../Interface/include", webotsHome .. "/include/controller/c/"}
  
  libdirs {webotsHome .. "/lib/"}
  
  files{"WebotsSource/**.cpp"}
  
  links {"Interface"}
 
-- END Webots
