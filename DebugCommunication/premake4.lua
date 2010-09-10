solution "DebugCommunication" 
  platforms {"Native"}
  configurations {"Debug", "Release"}
  
  
  -- debug configuration
  configuration { "Debug" }
	defines { "DEBUG" }
	flags { "Symbols" }

  project "DebugCommunication"
   	  kind "StaticLib"
   	  language "C++"
	 	  files {"Source/**.h","Source/**.cpp"}
	  
     includedirs {"../Extern/include","../Extern/include/glib-2.0/","../Extern/lib/glib-2.0/include/"}

   libdirs {"../Extern/lib/"}
	  
   links {}
	  
   targetname "naothdebugcom"
   
  project "DebugCommunicationTest"
   	  kind "ConsoleApp"
   	  language "C++"
	 	  files {"Source/**.h","Source/**.cpp"}
	  
     includedirs {"../Extern/include","../Extern/include/glib-2.0/","../Extern/lib/glib-2.0/include/"}

   libdirs {"../Extern/lib/"}
	  
   links {"glib-2.0","gio-2.0"}
	  
   targetname "testdebugcom"


