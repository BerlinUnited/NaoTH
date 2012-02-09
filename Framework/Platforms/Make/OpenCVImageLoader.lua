local extern_dir = "../../../Extern"

-- NaoTH controller for the webcam based "simulator"
project "OpenCVImageLoader"
  kind "ConsoleApp"
  language "C++"
   
  print("Generating files for OpenCVImageLoader")
  includedirs {
	"../Source/",
	CORE_PATH,
	"../../NaoTH-Tools/Source/",
	extern_dir .. "/include/",
	extern_dir .. "/include/glib-2.0/",
	extern_dir .. "/lib/glib-2.0/include/",
	extern_dir .. "/include/opencv2/core/include/",
	extern_dir .. "/include/opencv2/imgproc/include/",
	extern_dir .. "/include/opencv2/highgui/include/",
	}
  
  files{"../Source/OpenCVImageLoader/**.cpp","../Source/OpenCVImageLoader/**.h"}
  
  links {CORE, 
	"NaoTH-Commons",
	"glib-2.0",
	"gio-2.0",
	"gobject-2.0",
	"gmodule-2.0",
	"gthread-2.0",
	"protobuf",
	"opencv_core",
	"opencv_ml",
	"opencv_imgproc",
	"opencv_highgui"
	}
	
  targetname "OpenCVImageLoader"
  
  configuration{'vs*'}
	debugdir "$(SolutionDir).."
  
  configuration {"linux"}
    linkoptions {"-Wl,-rpath \"" .. path.getabsolute("../../../Extern/lib/") .. "\""}

  -- END OpenCVImageLoader
