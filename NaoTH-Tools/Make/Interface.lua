project "Interface"
  kind "StaticLib"
  language "C++"
  
  targetname "naointerface"
  
  includedirs {"../Source/","../Source/Interface/", "../../Extern/include",
  "../../Extern/include/glib-2.0/","../../Extern/lib/glib-2.0/include/"}
  
  files {"../Source/Interface/**.cpp", "../Source/Interface/**.h"}
  
  postbuildcommands{"premake4 install"}
