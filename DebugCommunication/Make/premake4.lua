solution "DebugCommunication" 
  platforms {"Native"}
  configurations {"Debug", "Release"}
  
  
  -- debug configuration
  configuration { "Debug" }
	defines { "DEBUG" }
	flags { "Symbols" }

  dofile "DebugCommunication.lua"
   
  project "DebugCommunicationTest"
   	  kind "ConsoleApp"
   	  language "C++"
	 	  files {"../Source/**.h","../Source/**.cpp"}
	  
     includedirs {
      "../../NaoTH-Tools/Source/",
      "../../Extern/include",
      "../../Extern/include/glib-2.0/",
      "../../Extern/lib/glib-2.0/include/"
     }

   libdirs {"../../Extern/lib/"}
	  
   links {"glib-2.0","gio-2.0","gobject-2.0","gthread-2.0"}
	  
	 targetdir "../Dist" 
   targetname "testdebugcom"


