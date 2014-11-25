project "Commons"
  kind "StaticLib"
  language "C++"
  
  flags {"FatalWarnings"}
  targetname "commons"
  
  includedirs {
    "../Source/",
    "../Source/Messages/"
  }
  
  files {
    "../Source/**.cpp",
    "../Source/**.cc",
    "../Source/**.c", 
    "../Source/**.h"
  }
  

  
