project "NaoTHSoccer"
  kind "StaticLib"
  language "C++"
  
  files {"../Source/**.h","../Source/**.cpp"}
  
  includedirs {
	"../Source/",
	"../../Framework/NaoTH-Tools/Source/",
	"../../Framework/DebugCommunication/Source/"}
  
  links { "NaoTH-Tools", "DebugCommunication", "pthread" }
  
  targetname "naoth-soccer"

