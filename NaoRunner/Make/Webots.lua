-- NaoTH controller running in the Cyberbotics Webots simulator
project "Webots"
  kind "StaticLib"
  language "C++"
  
  local webotsHome = os.getenv("WEBOTS_HOME")
  local file, msg;
  local compileWebots = true;
  if(webotsHome == nil) then
    webotsHome = "/usr/local/webots";
  end
	file, msg = io.open(webotsHome .. "/webots", "r")
	if not file then 
	  compileWebots = false;
	  print("WARN: Enviroment variale WEBOTS_HOME was not set or unable to access directory (" .. msg .. ")! No compilation for Webots possible.")
	end
  if compileWebots then
    print("Generating files for webots")
    includedirs {
	    "../Source/", 
	    webotsHome .. "/include/controller/c/",
	    "../Lib/win32/include/", 
	    "../Source/Interface/"}
  
    libdirs {webotsHome .. "/lib/"}
  
    files{"../Source/Webots/**.cpp", "../Source/Webots/**.h"}
  
    links {"Interface", "Controller"}
  
   targetname "naowebots"
 end
-- END Webots
