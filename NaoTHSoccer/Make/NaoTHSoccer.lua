project "NaoTHSoccer"
  kind "StaticLib"
  language "C++"

  files {
    "../Source/**.h",
    "../Source/**.cpp",
    "../Source/**.cc"
  }
  
  sysincludedirs { "../Source/Messages/" }
  includedirs { "../Source/" }
  
  links { "Commons"	}
  
  targetname "naoth-soccer"
