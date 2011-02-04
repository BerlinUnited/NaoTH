-- NaoTH controller running in the Cyberbotics Webots simulator
project "DCM"
  kind "SharedLib"
  language "C++"
  
  
  ------------------------- AL_DIR -----------------------------
  local alDir = os.getenv("AL_DIR")

  -- check if the directory can be opened by lua and warn if not
  if(alDir == nil or not os.isdir(alDir)) then
    print("WARN: Enviroment variale AL_DIR was not set or unable to access directory (" .. msg .. ")!")
	print("(PROBABLY) no compilation for DCM possible.\n")
  else
    print("INFO: Configuring with NaoQi located in \"" .. alDir .. "\"")
  end
  ----------------------------------------------------------------
  
  -- include core
  if(CORE_PATH == nil or CORE == nil) then
    print("WARNING: no path for the core is set")
	CORE_PATH = "."
	CORE = ""
  end
  
  
  print("Generating files for DCM")
  
  if os.is("windows") then
    libdirs { alDir .. "/lib/"}

    includedirs {
    	"../Source/", 
			CORE_PATH,
			"../../NaoTH-Tools/Source/",
    	alDir .. "/include/",
    	alDir .. "/include/alfactory/"
    }

    links {commonlinks, "asound","rttools", "alcommon", "almemoryfastaccess"}
  else
    libdirs { alDir .. "lib/"}

    includedirs {
    	"../Source/", 
			CORE_PATH,
			"../../NaoTH-Tools/Source/",
      alDir .. "/include/",
      alDir .. "/include/glib-2.0/",
      alDir .. "/include/alfactory/"
    }

    links {CORE, 
		  "NaoTH-Tools",
			"almemoryfastaccess", 
			"alcommon", 
			"glib-2.0",
			"gio-2.0",
			"gobject-2.0",
			"protobuf", 
			"asound",
			"rttools"
		}
  end

  files{"../Source/DCM/**.cpp", "../Source/DCM/**.h"}
  targetname "naoth"
  
   
-- END Webots
