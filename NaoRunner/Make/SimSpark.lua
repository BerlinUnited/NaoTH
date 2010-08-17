-- NaoTH controller for SimSpark
project "SimSpark"
  kind "StaticLib"
  language "C++"
        
  files{ 
	"../Source/SimSpark/**.cpp", 
	"../Source/SimSpark/**.h",
	"../Source/Interface/**.cpp",
	"../Source/Interface/**.h"}

  includedirs {
	"../Include/", 
	"../Lib/", 
	"../Lib/win32/include/", 
	"../Include/naorunner/", 
	"../Source/Interface/"}


  libdirs {"../Lib/"}
  
  links {"sfsexp"}

  targetname "naoth-simspark"

  -- postbuildcommands {
    -- copy Config
    -- os.getcopycmd(os.getcwd() .. "/../../Config/", os.getcwd() .. "/dist/", true)
  -- }
  -- END SimSpark
