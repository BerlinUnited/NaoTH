-- controller for SimSpark simulator
project "Webots"
  kind "StaticLib"
  language "C++"
        
  includedirs {
    "../Source/"
  }
  
  files{
    "../Source/Webots/**.cpp", 
    "../Source/Webots/**.h"
  }
  
  links {
    "sfsexp",
    "glib-2.0",
    "gio-2.0",
    "gobject-2.0",
    "gmodule-2.0",
    "gthread-2.0",
    "protobuf",
    "fftw3",
    "jpeg"
  }
  
  targetname "naoth-webots"
