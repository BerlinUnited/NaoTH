project "Commons"
  kind "StaticLib"
  language "C++"
  
  includedirs {
    "../Source/"
  }
  externalincludedirs {
    "../Source/Messages/"
  }
  
  files {
    "../Source/**.cpp",
    "../Source/**.cc",
    "../Source/**.c", 
    "../Source/**.h",
    "../Source/**.hpp"
  }
  
  targetname "commons"
  