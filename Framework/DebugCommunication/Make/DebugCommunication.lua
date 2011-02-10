project "DebugCommunication"
   	  kind "StaticLib"
   	  language "C++"
	 	  files {"../Source/**.h","../Source/**.cpp"}
	  
	-- define the extern directory
	extern_dir = "../../../Extern"
  
	includedirs {
		"../../NaoTH-Tools/Source/"
	}

   libdirs {extern_dir .. "/lib/"}
	  
   links {"NaoTH-Tools"}
	  
   targetname "naothdebugcom"
