-- NaoTH controller for the webcam based "simulator"
project "OpenCVWebCam"
  kind "ConsoleApp"
  language "C++"
   
  print("Generating files for OpenCVWebCam")
  includedirs {
	"../Source/",
	"../../NaoTH-Tools/Source/",
	EXTERN_PATH .. "/include/",
	EXTERN_PATH .. "/include/glib-2.0/",
	EXTERN_PATH .. "/lib/glib-2.0/include/",
	EXTERN_PATH .. "/include/opencv2/core/include/",
	EXTERN_PATH .. "/include/opencv2/imgproc/include/",
	EXTERN_PATH .. "/include/opencv2/highgui/include/",
	}
  
  files{"../Source/OpenCVWebCam/**.cpp","../Source/OpenCVWebCam/**.h"}
  
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
	
  targetname "OpenCVWebCam"
  
  configuration {"linux"}
    linkoptions {"-Wl,-rpath \"" .. path.getabsolute(EXTERN_PATH .. "/lib/") .. "\""}

  -- END OpenCVWebCam
