-- those contain additional pathes for includes and libs
EXTERN_INCLUDES = { ["Nao"] = {}, ["Native"] = {} }
EXTERN_LIBDIRS = { ["Nao"] = {}, ["Native"] = {}}

PLATFORM = _OPTIONS["platform"]
if PLATFORM == nil then
  PLATFORM = "Native"
end

-- load local user settings if available
if os.isfile("projectconfig.user.lua") then
	print("loading local user path settings")
	dofile "projectconfig.user.lua"
end

-- load the global default settings
dofile "projectconfig.lua"



-- include the Nao platform
include (COMPILER_PATH_NAO)


-- load some helpers for cross compilation etc.
--dofile "helper/naocrosscompile.lua"
dofile "helper/extract_todos.lua"
dofile "helper/qtcreator.lua"

-- definition of the solution
solution "NaoTHSoccer"
  platforms {"Native", "Nao"}
  configurations {"OptDebug", "Debug", "Release"}
  
  print("generating solution NaoTHSoccer for platform " .. PLATFORM)
  
  local f = io.popen ("bzr revno", "r");
  local rev = f:read("*l")
  if(rev == nil) then 
    defines {"BZR_REVISION=-1"}
  else 
    defines{ "BZR_REVISION=" .. rev } 
    f:close ();
  end
-- f = io.popen ("bzr info", "r");
-- defines{ "BZR_BRANCHINFO=" .. f:read("*l") } 
-- f:close ();

  
  -- global lib path for all configurations
  -- additional includes
  libdirs {
    EXTERN_PATH .. "/lib"
  }
  libdirs {
    EXTERN_LIBDIRS[PLATFORM]
  }
  
  -- global include path for all projects and configurations
  includedirs {
	FRAMEWORK_PATH .. "/NaoTH-Commons/Source/",
	EXTERN_PATH .. "/include/",
	EXTERN_PATH .. "/include/glib-2.0/",
	EXTERN_PATH .. "/lib/glib-2.0/include/"
  }
  -- additional includes
  includedirs { 
	EXTERN_INCLUDES[PLATFORM] 
  }
  
  
  -- global links ( needed by NaoTHSoccer )
  links {
    "opencv_core",
	"opencv_ml",
	"opencv_highgui",
	"opencv_imgproc"
	}
  
  -- debug configuration
  configuration { "Debug" }
    defines { "DEBUG" }
    flags { "Symbols", "FatalWarnings" }
  
  configuration { "OptDebug" }
    defines { "DEBUG" }
    -- flags { "Optimize", "FatalWarnings" }
    flags { "Optimize" }
         
  
  configuration{"Native"}
    targetdir "../dist/Native"
    
  configuration {"Nao"}
    defines { "NAO" }
    targetdir "../dist/Nao"

  -- additional defines for windows
  if(_OPTIONS["platform"] ~= "Nao" and _ACTION ~= "gmake") then
  configuration {"windows"}
    defines {"WIN32", "NOMINMAX"}
    buildoptions {"/wd4351", -- disable warning: "...new behavior: elements of array..."
				  "/wd4996", -- disable warning: "...deprecated..."
				  "/wd4290"} -- exception specification ignored (typed stecifications are ignored)
	links {"ws2_32"}
	--debugdir ".."
  end
  
  -- configuration {"linux"}
  if(_ACTION == "gmake") then
    buildoptions {"-fPIC"}
    flags { "ExtraWarnings" }
  end
  
  -- Why? OpenCV is always dynamically linked and we can only garantuee that there is one version in Extern (Thomas)
  configuration {"linux"}
    linkoptions {"-Wl,-rpath \"" .. path.getabsolute(EXTERN_PATH .. "/lib/") .. "\""}
      
	  
  -- base
  dofile (FRAMEWORK_PATH .. "/NaoTH-Commons/Make/NaoTH-Commons.lua")
  -- core
  dofile "NaoTHSoccer.lua"
  
  -- platforms
  if _OPTIONS["platform"] == "Nao" then
    dofile (FRAMEWORK_PATH .. "/Platforms/Make/NaoSMAL.lua")
    dofile (FRAMEWORK_PATH .. "/Platforms/Make/NaoRobot.lua")
	  kind "ConsoleApp"
	  links { "NaoTHSoccer", "NaoTH-Commons" }
  else
    dofile (FRAMEWORK_PATH .. "/Platforms/Make/SimSpark.lua")
	  kind "ConsoleApp"
	  links { "NaoTHSoccer", "NaoTH-Commons" }
	dofile (FRAMEWORK_PATH .. "/Platforms/Make/SPL_SimSpark.lua")
	  kind "ConsoleApp"
	  links { "NaoTHSoccer", "NaoTH-Commons" }
	  --debugargs { "--sync" }
    dofile (FRAMEWORK_PATH .. "/Platforms/Make/Webots.lua")
	  kind "ConsoleApp"
	  links { "NaoTHSoccer", "NaoTH-Commons" }
    dofile (FRAMEWORK_PATH .. "/Platforms/Make/LogSimulator.lua")
	  kind "ConsoleApp"
	  links { "NaoTHSoccer", "NaoTH-Commons" }
	dofile (FRAMEWORK_PATH .. "/Platforms/Make/OpenCVImageLoader.lua")
	  kind "ConsoleApp"
	  links { "NaoTHSoccer", "NaoTH-Commons" }
	dofile (FRAMEWORK_PATH .. "/Platforms/Make/OpenCVWebCam.lua")
	  kind "ConsoleApp"
	  links { "NaoTHSoccer", "NaoTH-Commons" }
  end
  
  
  -- tests
  if(_OPTIONS["platform"] ~= "Nao") then
	dofile (FRAMEWORK_PATH .. "/NaoTH-Commons/Make/Tests.lua")
	dofile "Tests.lua"
  end
  
  
