-- NaoTH controller for the logfile based "simulator"
project "WhistleSimulator"
  removeplatforms { "Nao" }

  kind "StaticLib"
  language "C++"
  
  includedirs {
    "../Source/"
  }
  
  files {
    "../Source/WhistleSimulator/**.cpp",
    "../Source/WhistleSimulator/**.h"
  }
  
  links {
    "glib-2.0",
    "gio-2.0",
    "gobject-2.0",
    "gmodule-2.0",
    "gthread-2.0",
    "protobuf",
    "fftw3",
    "jpeg"
  }
  
  targetname "whistlesimulator"
