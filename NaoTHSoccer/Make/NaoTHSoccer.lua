project "NaoTHSoccer"
  kind "StaticLib"
  language "C++"

  files {
    "../Source/**.h",
    "../Source/**.cpp",
    "../Source/**.cc"
  }
  
  -- HACK: rebuild main.cpp everytime in order to update compile time for gamelog stick info
  naorobot_main = path.getabsolute(FRAMEWORK_PATH) .. "/Platforms/Source/DCM/NaoRobot/main.cpp"
  prebuildcommands { "touch " .. naorobot_main } -- it updates this multiple times
  
  sysincludedirs { "../Source/Messages/" }
  includedirs { "../Source/" }
  
  links { "Commons"	}
  
  targetname "naoth-soccer"
