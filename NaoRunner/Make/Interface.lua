project "Interface"
  kind "StaticLib"
  language "C++"
  
  targetname "naointerface"
  
  includedirs {"../Source/","../Source/Interface/"}
  
  files {"../Source/Interface/**.cpp"}
  
