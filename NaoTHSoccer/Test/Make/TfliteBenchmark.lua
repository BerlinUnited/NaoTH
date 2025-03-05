project "TfliteBenchmark"
  kind "ConsoleApp"
  language "C++"
  
  includedirs {
    FRAMEWORK_PATH .. "/Platforms/Source"
	}
  
  files {
    "../Source/TfliteBenchmark/**.cpp",
    "../Source/TfliteBenchmark/**.h"
	}
  
  links {
    "tensorflowlite_c"
	}
  
  cppdialect "c++14"
	
  targetname "tflite-benchmark"