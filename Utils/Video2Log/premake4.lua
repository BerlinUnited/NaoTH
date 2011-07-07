local extern_dir = "../../Extern"

dofile "helper/qtcreator.lua"

solution "Video2Log"

  configurations {"Debug","Release"}

  
  includedirs {
      "src",
      "../../Framework/NaoTH-Commons/Source",
      "../../NaoTHSoccer/Source/",
      extern_dir .. "/include/",
      extern_dir .. "/include/glib-2.0/",
      extern_dir .. "/lib/glib-2.0/include/"
   }
   
   libdirs {
      extern_dir .. "/lib",
   }

  dofile "../../Framework/NaoTH-Commons/Make/NaoTH-Commons.lua"
  dofile "../../NaoTHSoccer/Make/NaoTHSoccer.lua"

  project "Video2Log"
    kind "ConsoleApp"
    files {"src/**.cpp", "src/**.cc"}
    language "C++"
    
    links {"NaoTHSoccer", "NaoTH-Commons",
	    "opencv_core",
      "opencv_imgproc",
      "opencv_highgui",
	    "glib-2.0",
	    "gio-2.0",
	    "gobject-2.0",
	    "gmodule-2.0",
	    "gthread-2.0",
	    "gtk-x11-2.0",
	    "protobuf"
	  }
    
    
    targetname "video2log"
    
    configuration {"linux"}
      linkoptions {"-Wl,-rpath \"" .. path.getabsolute(extern_dir .. "/lib/") .. "\""}
    
  
