project "DebugCommunication"
   	  kind "StaticLib"
   	  language "C++"
	 	  files {"../Source/**.h","../Source/**.cpp"}
	  
     includedirs {
      "../../NaoTH-Tools/Source/",
      "../../Extern/include/",
      "../../Extern/include/glib-2.0/",
      "../../Extern/lib/glib-2.0/include/"
      }

   libdirs {"../../Extern/lib/"}
	  
   links {"NaoTH-Tools"}
	  
	 targetdir "../../Extern/lib"
   targetname "naothdebugcom"
