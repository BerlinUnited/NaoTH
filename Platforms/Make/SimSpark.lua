-- NaoTH controller for SimSpark
project "SimSpark"
    kind "StaticLib"
  language "C++"
        
  files{ "../Source/SimSpark/**.cpp", "../Source/SimSpark/**.h"}

  includedirs {
	"../Source/",   
	"../../Extern/include/",
	"../../Extern/include/glib-2.0/","../../Extern/lib/glib-2.0/include/"}


  libdirs {"../../Extern/lib/"}
  
  links {"Interface", "sfsexp"}

  targetname "naoth-simspark"

  -- postbuildcommands {
    -- copy Config
    -- os.getcopycmd(os.getcwd() .. "/../../Config/", os.getcwd() .. "/dist/", true)
  -- }
  -- END SimSpark
