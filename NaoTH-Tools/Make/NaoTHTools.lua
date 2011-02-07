dofile "install.lua"

project "NaoTH-Tools"
  kind "StaticLib"
  language "C++"
  
  targetname "naothtools"
  
  includedirs {
	"../Source/",
	"../Source/Interface/"}
  
  files {
    "../Source/**.cpp",
    "../Source/**.cc", 
    "../Source/**.h"
  }
  
  --postbuildcommands{"premake4 install"}
