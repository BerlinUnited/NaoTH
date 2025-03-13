project "TfliteCheck"
  kind "ConsoleApp"
  language "C++"
  
  includedirs {
    FRAMEWORK_PATH .. "/Platforms/Source"
	}
  
  files {
    "../Source/TfliteCheck/**.cpp",
    "../Source/TfliteCheck/**.h"
	}
  
  links {
    "tensorflowlite_c"
	}
  
  cppdialect "c++14"
	
  targetname "tflite-check"