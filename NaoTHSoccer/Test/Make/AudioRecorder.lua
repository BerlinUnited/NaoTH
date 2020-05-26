-- NaoTH controller for the logfile based "simulator"
project "AudioRecorder"
  kind "ConsoleApp"
  language "C++"
  
  includedirs {
    FRAMEWORK_PATH .. "/Platforms/Source"
	}
  
  files {
    FRAMEWORK_PATH .. "/Platforms/Source/DCM/NaoRobot/AudioRecorder.cpp",
    "../Source/AudioRecorder/**.cpp",
    "../Source/AudioRecorder/**.h"
	}
  
  links {
    "Commons",
    "pthread",
    "protobuf",
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
    "wrap",
    "z",
    "rt"
	}
  
  cppdialect "c++14"
	
  targetname "naorec"
