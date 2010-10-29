dofile "install.lua"

project "NaoTH-Tools"
  kind "StaticLib"
  language "C++"
  
  targetname "naothtools"
  
  includedirs {"../Source/","../Source/Interface/", "../../Extern/include",
  "../../Extern/include/glib-2.0/","../../Extern/lib/glib-2.0/include/"}
  
  files {
    "../Source/**.cpp", 
    "../Source/**.h"
  }
  
  postbuildcommands{"premake4 install"}
