project "DebugCommunication"
   	  kind "StaticLib"
   	  language "C++"
	 	  files {"../Source/**.h","../Source/**.cpp"}
	  
     includedirs {
      "../../NaoTH-Tools/Source/"
      }

   libdirs {"../../Extern/lib/"}
	  
   links {"NaoTH-Tools"}
	  
	 targetdir "../../Extern/lib"
   targetname "naothdebugcom"
