-- Nao Shared Memory Abstraction Layer (NaoSMAL) is a shared library which 
-- is loaded as a naoqi module and provides access to the HAL functionality of 
-- naoqi through shared memory

------------------------- AL_DIR -----------------------------
-- it's set in the global project config
-- check if the directory can be opened by lua and warn if not
if(AL_DIR == nil) then
  print("Warning: path variable AL_DIR not set, compilation of 'NaoSMAL' not possible.")
else
  print("INFO: Configuring with NaoQi located in \"" .. AL_DIR .. "\"")
end
--------------------------------------------------------------

-- Nao Shared Memory Abstraction Layer
project "NaoSMAL"
  kind "SharedLib"
  language "C++"
  
  print("Generating files for NaoSMAL")

  libdirs { AL_DIR .. "/lib/" }

  includedirs {
	"../Source/DCM",
	"../../NaoTH-Tools/Source/",
	AL_DIR .. "/include/",
	AL_DIR .. "/include/alfactory/"
	}

  links {
	"NaoTH-Commons",
	"almemoryfastaccess", 
	"alcommon",
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