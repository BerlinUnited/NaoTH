project "rc-commons"
  kind "StaticLib"
  language "C++"
  
  targetname "rc-commons"
  
  includedirs { "." }
  
  files {
    "./**.cpp",
    "./**.h"
  }
