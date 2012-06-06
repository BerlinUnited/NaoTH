project "NaoTH-Commons"
  kind "StaticLib"
  language "C++"
  
  flags {"FatalWarnings"}
  targetname "naothtools"
  
  includedirs {
    "../Source/",
    "../Source/Interface/"
  }
  
  files {
    "../Source/**.cpp",
    "../Source/**.cc",
    "../Source/**.c", 
    "../Source/**.h"
  }
