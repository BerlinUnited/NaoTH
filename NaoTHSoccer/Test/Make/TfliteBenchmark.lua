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
    "tensorflow-lite"
	}
  
  cppdialect "c++14"
	
  targetname "tflite-benchmark"