-- load the global default settings
dofile "projectconfig.lua"

-- load local user settings if alailable
if os.isfile("projectconfig.user.lua") then
	print("loading local user path settings")
	dofile "projectconfig.user.lua"
end

print("EXTERN_PATH = " .. EXTERN_PATH)

--
include (NAO_CTC)

-- load some helpers for cross compilation etc.
--dofile "helper/naocrosscompile.lua"
dofile "helper/extract_todos.lua"
dofile "helper/qtcreator.lua"

-- definition of the solution
solution "NaoTHSoccer"
  platforms {"Native", "Nao"}
  configurations {"OptDebug", "Debug", "Release"}
  
  
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

  -- general lib path for all configurations
  libdirs {
    EXTERN_PATH .. "/lib"
  }
  -- todo remove
  includedirs {
	FRAMEWORK_PATH.."/NaoTH-Commons/Source/"
  }
  
  -- general include path for all projects and configurations
  includedirs {
	EXTERN_PATH .. "/include/",
	EXTERN_PATH .. "/include/glib-2.0/",
	EXTERN_PATH .. "/lib/glib-2.0/include/"
  }
  
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
    includedirs { "/usr/include/" }
    libdirs { EXTERN_PATH .. "/lib" }
    
    if os.get() == "linux" then      
      -- try to get the pkg-config include-instructions
      local f = io.popen("pkg-config --cflags glib-2.0")
      local l = f:read("*a")
      buildoptions { l }
    end

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
  if(_OPTIONS["platform"] == "Nao") then
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
  
  
