project "Interface"
  kind "StaticLib"
  language "C++"
  
  targetname "naointerface"
  
  includedirs {"../Include/"}
  
  files {"../Source/Interface/**.cpp"}
  
