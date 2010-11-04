-- NaoTH controller running in the Cyberbotics Webots simulator
project "Webots"
  kind "ConsoleApp"
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
  
  -- include core
  if(CORE_PATH == nil or CORE == nil) then
    print("WARNING: no path for the core is set")
	CORE_PATH = "."
	CORE = ""
  end
  
  
  print("Generating files for webots")
  includedirs {
    "../Source/", 
	CORE_PATH,
	"../../NaoTH-Tools/Source/",
    webotsHome .. "/include/controller/c/",
    "../Lib/win32/include/", 
    "../../Extern/include/"}
  
  libdirs {webotsHome .. "/lib/"}
  
  files{"../Source/Webots/**.cpp", "../Source/Webots/**.h"}
  
  links {CORE, "NaoTH-Tools", "Controller"}
  
  targetname "naowebots"
   
-- END Webots
