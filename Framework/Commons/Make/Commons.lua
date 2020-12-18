project "Commons"
  kind "StaticLib"
  language "C++"
  
  targetname "commons"
  
  includedirs {
    "../Source/"
  }
  sysincludedirs {
    "../Source/Messages/"
  }
  
  files {
    "../Source/**.cpp",
    "../Source/**.cc",
    "../Source/**.c", 
    "../Source/**.h",
    "../Source/**.hpp"
  }
  

  
