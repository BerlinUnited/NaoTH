local hasWebots = true

 ---------------------- WEBOTS_HOME -----------------------------
local webotsHome = os.getenv("WEBOTS_HOME")

-- default value for linux
if webotsHome == nil then
  if os.is("windows") then
    -- TODO: check this path on real Webots installations
    webotsHome = "C:\Program Files\Webots"
  else
    webotsHome = "/usr/local/webots"
  end
end

-- check if the directory can be opened by lua and warn if not
if not os.isdir(webotsHome) then
  print("WARN: Enviroment variale WEBOTS_HOME was not set or unable to access directory (" .. webotsHome .. ")!")
  print("no compilation for Webots possible.\n")
  hasWebots = false
end
 ----------------------------------------------------------------
 

if hasWebots then

-- NaoTH controller running in the Cyberbotics Webots simulator
project "Webots"
  kind "StaticLib"
  language "C++"
  
print("Generating files for webots")

  includedirs {
    "../Source/", 
    "../../NaoTH-Tools/Source/",
    webotsHome .. "/include/controller/c/"
	}

  libdirs {webotsHome .. "/lib/"}

  files{"../Source/Webots/**.cpp", "../Source/Webots/**.h"}

  links {
	"Controller",
	"glib-2.0",
	"gio-2.0",
	"gobject-2.0",
	"gmodule-2.0",
	"gthread-2.0",
	"protobuf"
  }

  targetname "nao_team_0"
  targetdir "../nao_robocup/controllers/nao_team_0/"

  postbuildcommands {
    "premake4 webots_copy"
  }
    
end -- hasWebots 
-- END Webots
