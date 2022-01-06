-- NaoRobot is the platform for the physical Nao >= v4

project "NaoRobot"
  kind "StaticLib"
  language "C++"
  
  includedirs {
    "../Source/DCM"
  }
  
  files {
    "../Source/DCM/NaoRobot/**.cpp", 
    "../Source/DCM/NaoRobot/**.h",
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
    "asound",
    "pthread",
    "jpeg",
    -- Whistledetector stuff
    "fftw3",
    "pulse-simple",
    "pulse",
    "pulsecommon-3.99",
    "json",
    "dbus-1",
    "sndfile",
    "asyncns",
    "FLAC",
    "gdbm",
    "vorbis",
    "vorbisenc",
    "ogg",
    "cap",
    "attr",
    "wrap"

    -- at the moment we run flite as a separate binary
    -- in hte future is would be good to include it as a lib
    --"flite",
    --"flite_cmulex",
    --"flite_cmu_us_slt",
    --"flite_usenglish"
  }

  targetname "naoth"
