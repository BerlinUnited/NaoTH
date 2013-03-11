-- NaoTH controller running in the Cyberbotics Webots simulator
project "Webots"
  kind "StaticLib"
  language "C++"
  
print("Generating files for webots")

  includedirs {
    "../Source/"
	}

  files{"../Source/Webots/**.cpp", "../Source/Webots/**.h"}

  links {
	"Controller", -- provided by Webots
	"glib-2.0",
	"gio-2.0",
	"gobject-2.0",
	"gmodule-2.0",
	"gthread-2.0",
	"protobuf"
  }

  targetname "nao_team_0"
  
  
  -- todo: put it somewhere else
  targetdir "../nao_robocup/controllers/nao_team_0/"

  postbuildcommands {
    "premake4 webots_copy"
  }
-- END Webots
