-- NaoTH controller for SimSpark
project "SimSpark"
  kind "ConsoleApp"
  language "C++"
        
  files{ "../Source/SimSpark/**.cpp", "../Source/SimSpark/**.h"}
  
  -- include core
  if(CORE_PATH == nil or CORE == nil) then
    print("WARNING: no path for the core is set")
	CORE_PATH = "."
	CORE = ""
  end

  includedirs {
	"../Source/",
	CORE_PATH,
	"../../NaoTH-Tools/Source/",
	"../../Extern/include/",
	"../../Extern/include/glib-2.0/",
	"../../Extern/lib/glib-2.0/include/"}


  libdirs {"../../Extern/lib/"}
  
  links {
    CORE,
	"NaoTH-Tools", 
	"sfsexp",
	"glib-2.0",
	"gio-2.0",
	"gobject-2.0"}

  targetname "naoth-simspark"

  -- postbuildcommands {
    -- copy Config
    -- os.getcopycmd(os.getcwd() .. "/../../Config/", os.getcwd() .. "/dist/", true)
  -- }
  -- END SimSpark
