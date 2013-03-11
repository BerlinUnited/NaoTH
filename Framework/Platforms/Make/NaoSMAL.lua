-- Nao Shared Memory Abstraction Layer (NaoSMAL) is a shared library which 
-- is loaded as a naoqi module and provides access to the HAL functionality of 
-- naoqi through shared memory

print("Generating files for NaoSMAL")

-- ld throws the following warning when linking NaoSMAL:
-- 'warning: creating a DT_TEXTREL in object.'
--
-- I believe it is because some of AL stuff is compiled without the option -fpic/-fPIC.
-- The flag -fpic/-fPIC - "position-independent code" is needed to compile shared libraries.
-- premake4 automatically includes -fPIC if a project is declared as a SharedLib (we don't need to do it extra). 
-- Actually, we would only need -fpic (instead of -fPIC) which may make our code faster.
-- http://www.akkadia.org/drepper/dsohowto.pdf

project "NaoSMAL"
  kind "SharedLib"
  language "C++"
  
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
	
  targetname "naosmal"
