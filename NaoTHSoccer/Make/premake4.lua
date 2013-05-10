-- set the default global platform
PLATFORM = _OPTIONS["platform"]
if PLATFORM == nil then
  PLATFORM = "Native"
end

-- load the global default settings
dofile "projectconfig.lua"

-- load some helpers
dofile (FRAMEWORK_PATH .. "/LuaTools/info.lua")
--dofile (FRAMEWORK_PATH .. "/LuaTools/ilpath.lua")
dofile (FRAMEWORK_PATH .. "/LuaTools/qtcreator.lua")
dofile (FRAMEWORK_PATH .. "/LuaTools/extract_todos.lua")
dofile (FRAMEWORK_PATH .. "/LuaTools/protoc.lua")

-- include the Nao platform
if COMPILER_PATH_NAO ~= nil then
	include (COMPILER_PATH_NAO)
end

-- test
-- print("INFO:" .. (os.findlib("Controller") or "couldn't fined the lib Controller"))


-- definition of the solution
solution "NaoTHSoccer"
  platforms {"Native", "Nao"}
  configurations {"OptDebug", "Debug", "Release"}
  location "../build"
  
  print("generating solution NaoTHSoccer for platform " .. PLATFORM)
  
  -- global lib path for all configurations
  -- additional includes
  libdirs (PATH["libs"])
  
  -- global include path for all projects and configurations
  includedirs (PATH["includes"])
  
  -- global links ( needed by NaoTHSoccer )
  links {
    "opencv_core",
    "opencv_ml",
    "opencv_imgproc"
	}
  
  -- set the remository information
  defines {
	"REVISION=\"" .. REVISION .. "\"",
	"USER_NAME=\"" .. USER_NAME .. "\"",
	"BRANCH_PATH=\"" .. BRANCH_PATH .. "\""
	}
  

 -- TODO: howto compile the framework representations properly *inside* the project?
  invokeprotoc(
    {FRAMEWORK_PATH .. "/NaoTH-Commons/Messages/CommonTypes.proto", 
	 FRAMEWORK_PATH .. "/NaoTH-Commons/Messages/Framework-Representations.proto", FRAMEWORK_PATH .. "/NaoTH-Commons/Messages/Messages.proto"}, 
    FRAMEWORK_PATH .. "/NaoTH-Commons/Source/Messages/", 
    "../../RobotControl/RobotConnector/src/", 
    {FRAMEWORK_PATH .. "/NaoTH-Commons/Messages/"}
  )

  invokeprotoc(
    {"../Messages/Representations.proto"}, 
    "../Source/Core/Messages/", 
    "../../RobotControl/RobotConnector/src/", 
    {FRAMEWORK_PATH .. "/NaoTH-Commons/Messages/", "../Messages/"}
  )

  
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
	debugdir ".."
  end
  
  configuration {"linux"}
   if(_ACTION == "gmake") then
     -- "position-independent code" needed to compile shared libraries.
	 -- In our case it's only the NaoSMAL. So, we probably don't need this one.
	 -- Premake4 automatically includes -fPIC if a project is declared as a SharedLib.
	 -- http://www.akkadia.org/drepper/dsohowto.pdf
    buildoptions {"-fPIC"}
    -- may be needed for newer glib2 versions, remove if not needed
    buildoptions {"-Wno-deprecated-declarations"}
    flags { "ExtraWarnings" }
    links {"pthread"}
	
	-- Why? OpenCV is always dynamically linked and we can only garantuee that there is one version in Extern (Thomas)
	linkoptions {"-Wl,-rpath \"" .. path.getabsolute(EXTERN_PATH .. "/lib/") .. "\""}
   end
   
  -- base
  dofile (FRAMEWORK_PATH .. "/NaoTH-Commons/Make/NaoTH-Commons.lua")
  -- core
  dofile "NaoTHSoccer.lua"
  
  -- set up platforms
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
	  debugargs { "--sync" }
    dofile (FRAMEWORK_PATH .. "/Platforms/Make/Webots.lua")
	  kind "ConsoleApp"
	  links { "NaoTHSoccer", "NaoTH-Commons" }
	  postbuildcommands { "premake4 webots_copy" }
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
  
  
