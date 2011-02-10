local extern_dir = "../../../Extern"

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
	extern_dir .. "/include/",
	extern_dir .. "/include/glib-2.0/",
	extern_dir .. "/lib/glib-2.0/include/"}


  libdirs {"../../Extern/lib/"}
  
  links {
    CORE,
	"NaoTH-Tools", 
	"sfsexp",
  "b64",
	"glib-2.0",
	"gio-2.0",
	"gobject-2.0",
	"protobuf"}

  targetname "naoth-simspark"

-- END SimSpark
