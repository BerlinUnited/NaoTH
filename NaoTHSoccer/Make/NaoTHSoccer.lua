project "NaoTHSoccer"
  kind "StaticLib"
  language "C++"

  files { "../Source/**.h",
          "../Source/**.cpp",
          "../Source/**.cc"
        }
  
  includedirs { "../Source/" }

  links { "Commons",
		"opencv_core",
		"opencv_ml",
		"opencv_imgproc",
		"opencv_objdetect"
	}
  targetname "naoth-soccer"

