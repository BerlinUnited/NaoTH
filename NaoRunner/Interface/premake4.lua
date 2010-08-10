project "Interface"
  kind "StaticLib"
  language "C++"
  
  targetdir "dist/"
  
  includedirs {"include/", "src/"}
  
  files {"src/**.cpp"}
  
