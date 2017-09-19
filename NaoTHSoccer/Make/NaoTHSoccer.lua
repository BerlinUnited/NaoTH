project "NaoTHSoccer"
  kind "StaticLib"
  language "C++"

  files {
    "../Source/**.h",
    "../Source/**.cpp",
    "../Source/**.cc"
  }
  
  includedirs { "../Source/" }
  
  targetname "naoth-soccer"
