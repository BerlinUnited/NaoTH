local extern_dir = "../../../Extern"

-- NaoTH controller for the webcam based "simulator"
project "OpenCVWebCam"
  kind "ConsoleApp"
  language "C++"
   
  print("Generating files for OpenCVWebCam")
  
  if( _ACTION == gmake) then
    print("OpenCV WebCam Linux")
    includedirs {
	"../Source/",
	CORE_PATH,
	"../../NaoTH-Tools/Source/",
	"/usr/include/glib-2.0/",
	"/usr/lib/glib-2.0/include/",
	"/usr/include/opencv2/core/include/",
	"/usr/include/opencv2/imgproc/include/",
	"/usr/include/opencv2/highgui/include/",
	extern_dir .. "/include/",
    }
  else
    print("OpenCV WebCam Windows")
    includedirs {
	"../Source/",
	CORE_PATH,
	"../../NaoTH-Tools/Source/",
	extern_dir .. "/include/",
	extern_dir .. "/include/glib-2.0/",
	extern_dir .. "/lib/glib-2.0/include/",
	extern_dir .. "/include/opencv2/core/include/",
	extern_dir .. "/include/opencv2/imgproc/include/",
	extern_dir .. "/include/opencv2/highgui/include/"
    }
  end
  
  
  files{"../Source/OpenCVWebCam/**.cpp","../Source/OpenCVWebCam/**.h"}
  
  links {CORE, "NaoTH-Commons",
	  "glib-2.0",
	  "gio-2.0",
	  "gobject-2.0",
	  "gmodule-2.0",
	  "gthread-2.0",
	  "pthread",
	  "protobuf",
	  "opencv_core",
	  "opencv_ml",
	  "opencv_imgproc",
	  "opencv_highgui"
	}

  targetname "OpenCVWebCam"
  
  configuration {"linux"}
    linkoptions {"-Wl,-rpath \"" .. path.getabsolute("../../../Extern/lib/") .. "\""}

  -- END OpenCVWebCam
