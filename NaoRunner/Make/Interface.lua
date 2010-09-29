project "Interface"
  kind "StaticLib"
  language "C++"
  
  targetname "naointerface"
  
  includedirs {"../Source/","../Source/Interface/"}
  
  files {"../Source/Interface/**.cpp", "../Source/Interface/**.h"}
  
  postbuildcommands{"premake4 install"}
