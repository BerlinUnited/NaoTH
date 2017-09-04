project "Commons"
  kind "StaticLib"
  language "C++"
  
  -- FatalWarnings treats compiler/linker warnings as errors
  -- in premake4 linker warnings are not enabled
  flags {"FatalCompileWarnings"}
  targetname "commons"
  targetextension ("")
  
  includedirs {
    "../Source/",
    "../Source/Messages/"
  }
  
  files {
    "../Source/**.cpp",
    "../Source/**.cc",
    "../Source/**.c", 
    "../Source/**.h",
    "../Source/**.hpp"
  }
  

  
