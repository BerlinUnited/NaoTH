-- LolaAdaptor is the replacement for NaoSMAL on Nao >= v6

project "LolaAdaptor"
  kind "StaticLib"
  language "C++"
  
  includedirs {
    "../Source/DCM"
  }
  
  files {
    "../Source/DCM/LolaAdaptor/**.cpp", 
    "../Source/DCM/LolaAdaptor/**.h",
    "../Source/DCM/Tools/**.cpp", 
    "../Source/DCM/Tools/**.h"  
  }
  
  links {
    "rt",
    "z",
    "gthread-2.0",
    "glib-2.0",
    "gio-2.0",
    "gmodule-2.0",
    "gobject-2.0",
    "protobuf",
    "pthread"
  }

  targetname "lola_adaptor"
