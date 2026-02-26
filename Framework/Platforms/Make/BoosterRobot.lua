-- BoosterRobot is the platform for the physical robot Booster K1 

project "BoosterRobot"
  kind "StaticLib"
  language "C++"
  
  includedirs {
    "../Source/Booster"
  }
  
  files {
    "../Source/Booster/BoosterRobot/**.cpp", 
    "../Source/Booster/BoosterRobot/**.h"
  }
  
  links {
    "gthread-2.0",
    "glib-2.0",
    "gio-2.0",
    "gmodule-2.0",
    "gobject-2.0",
    "protobuf",
    "jpeg",
    "fftw3"
    -- at the moment we run flite as a separate binary
    -- in the future it would be good to include it as a lib
    --"flite",
    --"flite_cmulex",
    --"flite_cmu_us_slt",
    --"flite_usenglish"
  }
  
  -- needed for nao
  if _OPTIONS["platform"] == "Nao" then
    links {
      "rt",
      "z",
      "pthread",
      "asound",
      "pulse",
      "pulse-simple"
    }
  end
  
  -- additional links needed for the old cross compiler
  if _OPTIONS["platform"] == "Nao" and (ROBOT_OS == nil or ROBOT_OS == "naoos") then
    links {
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
    }
  end

  targetname "naoth-booster"
