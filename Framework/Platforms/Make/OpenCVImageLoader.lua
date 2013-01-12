-- NaoTH controller for the webcam based "simulator"
project "OpenCVImageLoader"
  kind "StaticLib"
  language "C++"
   
  print("Generating files for OpenCVImageLoader")
  
  includedirs {
	"../Source/",
	"../../NaoTH-Tools/Source/"
	}
  
  files{
	"../Source/OpenCVImageLoader/**.cpp",
	"../Source/OpenCVImageLoader/**.h"}
  
  links {
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
  
  -- END OpenCVImageLoader
