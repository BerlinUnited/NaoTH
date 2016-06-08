-- set the default global platform
PLATFORM = _OPTIONS["platform"]
if PLATFORM == nil then
  PLATFORM = "Native"
end

-- load the global default settings
dofile "projectconfig.lua"

-- load some helpers
dofile (FRAMEWORK_PATH .. "/BuildTools/info.lua")
dofile (FRAMEWORK_PATH .. "/BuildTools/protoc.lua")
--dofile (FRAMEWORK_PATH .. "/BuildTools/ilpath.lua")
dofile (FRAMEWORK_PATH .. "/BuildTools/qtcreator.lua")
dofile (FRAMEWORK_PATH .. "/BuildTools/qtcreator_2.7+.lua")
--dofile (FRAMEWORK_PATH .. "/BuildTools/extract_todos.lua")

-- include the Nao platform
if COMPILER_PATH_NAO ~= nil then
	include (COMPILER_PATH_NAO)
end

-- test
-- print("INFO:" .. (os.findlib("Controller") or "couldn't fined the lib Controller"))

newoption {
   trigger     = "Wno-conversion",
   description = "Disable te -Wconversion warnin for gCC"
}

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
  local COMMONS_MESSAGES = FRAMEWORK_PATH .. "/Commons/Messages/"
  invokeprotoc(
    { COMMONS_MESSAGES .. "CommonTypes.proto", 
      COMMONS_MESSAGES .. "Framework-Representations.proto", 
      COMMONS_MESSAGES .. "Messages.proto"
    },
    FRAMEWORK_PATH .. "/Commons/Source/Messages/", 
    "../../RobotControl/RobotConnector/src/", 
    "../../Utils/pyLogEvaluator",
    {COMMONS_MESSAGES}
  )

  invokeprotoc(
    {"../Messages/Representations.proto"}, 
    "../Source/Messages/", 
    "../../RobotControl/RobotConnector/src/", 
    "../../Utils/pyLogEvaluator",
    {COMMONS_MESSAGES, "../Messages/"}
  )

  
  -- debug configuration
  configuration { "Debug" }
    defines { "DEBUG" }
    defines { "BOOST_SIGNALS_NO_DEPRECATION_WARNING" }
    flags { "Symbols", "FatalWarnings" }
  
  configuration { "OptDebug" }
    defines { "DEBUG" }
    defines { "BOOST_SIGNALS_NO_DEPRECATION_WARNING" }
    flags { "Optimize", "FatalWarnings" }
  
  configuration{"Native"}
    targetdir "../dist/Native"
    
  configuration {"Nao"}
    defines { "NAO" }
    targetdir "../dist/Nao"
    flags { "ExtraWarnings" }
    -- disable warning "comparison always true due to limited range of data type"
    -- this warning is caused by protobuf 2.4.1
    buildoptions {"-Wno-type-limits"}
    -- some of the protobuf messages are marked as deprecated but are still in use for legacy reasons
    buildoptions {"-Wno-deprecated-declarations"}
    if _OPTIONS["Wno-conversion"] == nil then
		  buildoptions {"-Wconversion"}
		  defines { "_NAOTH_CHECK_CONVERSION_" }
    end

  -- additional defines for windows
  if(_OPTIONS["platform"] ~= "Nao" and _ACTION ~= "gmake") then
    configuration {"windows"}
    defines {"WIN32", "NOMINMAX", "EIGEN_DONT_ALIGN"}
    buildoptions {"/wd4351", -- disable warning: "...new behavior: elements of array..."
                  "/wd4996", -- disable warning: "...deprecated..."
                  "/wd4290"} -- exception specification ignored (typed stecifications are ignored)
    links {"ws2_32"}
    debugdir "$(SolutionDir).."
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
      buildoptions {"-Wno-deprecated"}
      buildoptions {"-std=c++11"}
      flags { "ExtraWarnings" }
      links {"pthread"}
    
      if _OPTIONS["Wno-conversion"] == nil then
        buildoptions {"-Wconversion"}
        defines { "_NAOTH_CHECK_CONVERSION_" }
      end
    
      -- Why? OpenCV is always dynamically linked and we can only garantuee that there is one version in Extern (Thomas)
      linkoptions {"-Wl,-rpath \"" .. path.getabsolute(EXTERN_PATH .. "/lib/") .. "\""}
    end
   
  -- base
  dofile (FRAMEWORK_PATH .. "/Commons/Make/Commons.lua")
    configuration {"Nao"}
      buildoptions {"-std=c++11"}
  
  -- core
  dofile "NaoTHSoccer.lua"
    configuration {"Nao"}
      buildoptions {"-std=c++11"}
  
  -- set up platforms
  if _OPTIONS["platform"] == "Nao" then
    dofile (FRAMEWORK_PATH .. "/Platforms/Make/NaoSMAL.lua")
      -- HACK: boost from NaoQI SDK makes problems
      buildoptions {"-Wno-conversion"}
     -- ACHTUNG: NaoSMAL doesn't build with the flag -std=c++11 (because of Boost)
      buildoptions {"-std=gnu++11"}
    dofile (FRAMEWORK_PATH .. "/Platforms/Make/NaoRobot.lua")
      kind "ConsoleApp"
      links { "NaoTHSoccer", "Commons" }
      buildoptions {"-std=c++11"}
  else
    dofile (FRAMEWORK_PATH .. "/Platforms/Make/SimSpark.lua")
      kind "ConsoleApp"
      links { "NaoTHSoccer", "Commons" }
      debugargs { "--sync" }
    dofile (FRAMEWORK_PATH .. "/Platforms/Make/LogSimulator.lua")
      kind "ConsoleApp"
      links { "NaoTHSoccer", "Commons" }
    dofile (FRAMEWORK_PATH .. "/Platforms/Make/LogSimulatorJNI.lua")
      kind "SharedLib"
      links { "NaoTHSoccer", "Commons" }
  end
  
  
  -- tests
  --if(_OPTIONS["platform"] ~= "Nao") then
	--dofile (FRAMEWORK_PATH .. "/NaoTH-Commons/Make/Tests.lua")
	--dofile "Tests.lua"
  --end
  
  
