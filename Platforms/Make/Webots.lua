-- NaoTH controller running in the Cyberbotics Webots simulator
project "Webots"
  kind "StaticLib"
  language "C++"
  
  
  ---------------------- WEBOTS_HOME -----------------------------
  local webotsHome = os.getenv("WEBOTS_HOME")

  -- default value for linux
  if webotsHome == nil and not os.is("windows") then
	webotsHome = "/usr/local/webots";
  end
  
  -- check if the directory can be opened by lua and warn if not
  if not os.isdir(webotsHome) then
    print("WARN: Enviroment variale WEBOTS_HOME was not set or unable to access directory (" .. msg .. ")!")
	print("(PROBABLY) no compilation for Webots possible.\n")
  end
  ----------------------------------------------------------------
  
  
  
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
   
-- END Webots
