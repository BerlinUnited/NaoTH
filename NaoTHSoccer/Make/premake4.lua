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
   trigger     = "Test",
   description = "Generate test projects"
}

newoption {
   trigger     = "Wno-conversion",
   description = "Disable the -Wconversion warning for gcc"
}

newoption {
   trigger     = "Wno-misleading-indentation",
   description = "Disable the -Wmisleading-indentation warning/error for gcc (6.0+)"
}

newoption {
   trigger     = "Wno-ignored-attributes",
   description = "Disable the -Wignored-attributes warning/error for gcc (6.0+)"
}

-- definition of the solution
solution "NaoTHSoccer"
  platforms {"Native", "Nao"}
  configurations {"OptDebug", "Debug", "Release"}
  location "../build"
  
  
  print("INFO: generating solution NaoTHSoccer")
  print("  PLATFORM = " .. PLATFORM)
  print("  OS = " .. os.get())
  print("  ACTION = " .. (_ACTION or "NONE"))
  
  
  -- global lib path for all configurations
  -- additional includes
  libdirs (PATH["libs"])
  
  -- global include path for all projects and configurations
  includedirs (PATH["includes"])
  
  -- global links ( needed by NaoTHSoccer )
  links {
    "opencv_core",
    "opencv_ml",
    "opencv_imgproc",
    "opencv_objdetect"
	}
  
  -- set the repository information
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
    "../../Utils/py/naoth/naoth",
    {COMMONS_MESSAGES}
  )

  invokeprotoc(
    {"../Messages/Representations.proto"}, 
    "../Source/Messages/", 
    "../../RobotControl/RobotConnector/src/", 
    "../../Utils/py/naoth/naoth",
    {COMMONS_MESSAGES, "../Messages/"}
  )
  
  configuration { "Debug" }
    defines { "DEBUG" }
    flags { "Symbols", "FatalWarnings" }
  
  configuration { "OptDebug" }
    defines { "DEBUG" }
    flags { "OptimizeSpeed", "FatalWarnings" }
    
    
  configuration{"Native"}
    targetdir "../dist/Native"
    
  -- special defines for the Nao robot
  configuration {"Nao"}
    defines { "NAO" }
    targetdir "../dist/Nao"
    flags { "ExtraWarnings" }
    -- disable warning "comparison always true due to limited range of data type"
    -- this warning is caused by protobuf 2.4.1
    buildoptions {"-Wno-type-limits"}
    -- some of the protobuf messages are marked as deprecated but are still in use for legacy reasons
    buildoptions {"-Wno-deprecated-declarations"}
    buildoptions {"-Wconversion"}
    buildoptions {"-std=c++11"}

    
  -- additional defines for visual studio
  configuration {"windows", "vs*"}
    defines {"WIN32", "NOMINMAX", "NOGDI", "EIGEN_DONT_ALIGN"}
    buildoptions {"/wd4351"} -- disable warning: "...new behavior: elements of array..."
    buildoptions {"/wd4996"} -- disable warning: "...deprecated..."
    buildoptions {"/wd4290"} -- exception specification ignored (typed stecifications are ignored)
    links {"ws2_32"}
    debugdir "$(SolutionDir).."
  
  
  configuration {"Native", "linux", "gmake"}
    -- "position-independent code" needed to compile shared libraries.
    -- In our case it's only the NaoSMAL. So, we probably don't need this one.
    -- Premake4 automatically includes -fPIC if a project is declared as a SharedLib.
    -- http://www.akkadia.org/drepper/dsohowto.pdf
    buildoptions {"-fPIC"}
    
    -- may be needed for newer glib2 versions, remove if not needed
    buildoptions {"-Wno-deprecated-declarations"}
    buildoptions {"-Wno-deprecated"}
    -- Prohibit GCC to be clever and use undefined behavior for some optimizations
    -- (see http://www.airs.com/blog/archives/120 for some nice explanation)
    buildoptions {"-fno-strict-overflow"}
    buildoptions {"-std=c++11"}
    --flags { "ExtraWarnings" }
    links {"pthread"}
  
    if _OPTIONS["Wno-conversion"] == nil then
      buildoptions {"-Wconversion"}
    end

    if _OPTIONS["Wno-misleading-indentation"] ~= nil then
      buildoptions {"-Wno-misleading-indentation"}
    end

    if _OPTIONS["Wno-ignored-attributes"] ~= nil then
      buildoptions {"-Wno-ignored-attributes"}
    end
  
    -- Why? OpenCV is always dynamically linked and we can only garantuee that there is one version in Extern (Thomas)
    linkoptions {"-Wl,-rpath \"" .. path.getabsolute(EXTERN_PATH .. "/lib/") .. "\""}
  
  
  configuration {"macosx"}
    defines { "BOOST_SIGNALS_NO_DEPRECATION_WARNING", "EIGEN_DONT_ALIGN" }
    buildoptions {"-std=c++11"}
    -- disable some warnings
    buildoptions {"-Wno-deprecated-declarations"}
    buildoptions {"-Wno-deprecated-register"}
    buildoptions {"-Wno-logical-op-parentheses"}
    -- use clang on macOS
    -- NOTE: configuration doesn't affect these settings, they NEED to be in a if
    if (os.is("macosx") and _OPTIONS["platform"] ~= "Nao") then
      premake.gcc.cc = 'clang'
      premake.gcc.cxx = 'clang++'
    end


  -- commons
  dofile (FRAMEWORK_PATH .. "/Commons/Make/Commons.lua")
    vpaths { ["*"] = FRAMEWORK_PATH .. "/Commons/Source" }
  
  -- core
  dofile "NaoTHSoccer.lua"
    vpaths { ["*"] = "../Source" }
  
  -- set up platforms
  if _OPTIONS["platform"] == "Nao" then
    dofile (FRAMEWORK_PATH .. "/Platforms/Make/NaoSMAL.lua")
      vpaths { ["*"] = FRAMEWORK_PATH .. "/Platforms/Source/NaoSMAL" }
      -- HACK: boost from NaoQI SDK makes problems
      buildoptions {"-Wno-conversion"}
      -- these warning came in Windows with the toolchain 2013
      buildoptions {"-Wno-unused-parameter"}
      buildoptions {"-Wno-ignored-qualifiers"}
      buildoptions {"-Wno-extra"}
      defines { "BOOST_SIGNALS_NO_DEPRECATION_WARNING" }
      -- ACHTUNG: NaoSMAL doesn't build with the flag -std=c++11 (because of Boost)
      buildoptions {"-std=gnu++11"}
    dofile (FRAMEWORK_PATH .. "/Platforms/Make/NaoRobot.lua")
      kind "ConsoleApp"
      links { "NaoTHSoccer", "Commons" }
      vpaths { ["*"] = FRAMEWORK_PATH .. "/Platforms/Source/NaoRobot" }
  else
    dofile (FRAMEWORK_PATH .. "/Platforms/Make/SimSpark.lua")
      kind "ConsoleApp"
      links { "NaoTHSoccer", "Commons" }
      vpaths { ["*"] = FRAMEWORK_PATH .. "/Platforms/Source/SimSpark" }
      debugargs { "--sync" }
    dofile (FRAMEWORK_PATH .. "/Platforms/Make/LogSimulator.lua")
      kind "ConsoleApp"
      links { "NaoTHSoccer", "Commons" }
      vpaths { ["*"] = FRAMEWORK_PATH .. "/Platforms/Source/LogSimulator" }
    dofile (FRAMEWORK_PATH .. "/Platforms/Make/LogSimulatorJNI.lua")
      kind "SharedLib"
      links { "NaoTHSoccer", "Commons" }
      vpaths { ["*"] = FRAMEWORK_PATH .. "/Platforms/Source/LogSimulatorJNI" }
      
    -- generate tests if required
    if _OPTIONS["Test"] ~= nil then
      dofile ("../Test/Make/BallEvaluator.lua")
        kind "ConsoleApp"
        links { "NaoTHSoccer", "Commons" }
        vpaths { ["*"] = "../Test/Source/BallEvaluator" }
    end
  end
