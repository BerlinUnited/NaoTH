-- Nao Shared Memory Abstraction Layer (NaoSMAL) is a shared library which 
-- is loaded as a naoqi module and provides access to the HAL functionality of 
-- naoqi through shared memory

print("Generating files for NaoSMAL")

project "NaoSMAL"
  kind "SharedLib"
  language "C++"
  
  libdirs { AL_DIR .. "/lib/" }

  includedirs {
	"../Source/DCM"
	}

  links {
	"NaoTH-Commons",
	"almemoryfastaccess", -- Aldebaran NaoQi
	"alcommon", -- Aldebaran NaoQi
	"gthread-2.0", 
	"glib-2.0",
	"gio-2.0",
	"gobject-2.0",
	"protobuf"
	}

  files{
	"../Source/DCM/NaoSMAL/**.cpp", 
	"../Source/DCM/NaoSMAL/**.h",
	"../Source/DCM/Tools/**.cpp", 
	"../Source/DCM/Tools/**.h"
	}
	
  targetname "naoth"