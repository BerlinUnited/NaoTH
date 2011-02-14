-- define the extern directory
local extern_dir = "../../../Extern"

solution "DebugCommunication" 
  platforms {"Native"}
  configurations {"Debug", "Release"}

  includedirs {
    "../../NaoTH-Tools/Source/",
    extern_dir .. "/include",
    extern_dir .. "/include/glib-2.0/",
    extern_dir .. "/lib/glib-2.0/include/"
  }
  
  -- debug configuration
  configuration { "Debug" }
	defines { "DEBUG" }
	flags { "Symbols" }
  
  
  -- include the actual project
  dofile "DebugCommunication.lua"
   
   
  -- create a test project
  project "DebugCommunicationTest"
    kind "ConsoleApp"
    language "C++"
    files {"../Source/**.h","../Source/**.cpp"}
  
    libdirs {extern_dir .. "/lib/"}
	  
    links {"glib-2.0","gio-2.0","gobject-2.0","gthread-2.0"}
	  
    targetdir "../dist" 
    targetname "testdebugcom"


