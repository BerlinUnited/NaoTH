project "Interface"
  kind "StaticLib"
  language "C++"
  
  targetname "naointerface"
  
  includedirs {"include/", "src/"}
  
  
  files {"src/**.cpp"}
  
