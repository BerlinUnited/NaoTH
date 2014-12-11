project "NaoTHSoccer"
  kind "StaticLib"
  language "C++"

  files { "../Source/**.h",
          "../Source/**.cpp",
          "../Source/**.cc"
        }
  
  includedirs { "../Source/" }

  links { "Commons" }
  
  targetname "naoth-soccer"

