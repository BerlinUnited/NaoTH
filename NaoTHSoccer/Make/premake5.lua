-- Test for checking the premake version
if not premake.checkVersion(premake._VERSION, ">=5.0.0-alpha12") then
  print("ERROR: Premake Version" .. premake._VERSION .. " is to old. Use at least 5.0.0-alpha12")
  os.exit()
end


require "tools/clean_action" -- get custom clean action
require "tools/tools"
dofile "tools/custom_options.lua" -- define custom options
require "tools/help"
dofile "projectconfig.lua" -- load the global default settings

-- load some helpers
dofile (FRAMEWORK_PATH .. "/BuildTools/info.lua")
dofile (FRAMEWORK_PATH .. "/BuildTools/protoc.lua")
dofile (FRAMEWORK_PATH .. "/BuildTools/qtcreator_2.7+.lua")

dofile (FRAMEWORK_PATH .. "/BuildTools/extract_modules.lua")


print("INFO: generating solution NaoTHSoccer")
print("  PLATFORM = " .. PLATFORM)
print("  OS = " .. os.target())
print("  ACTION = " .. (_ACTION or "NONE"))

-- generate the project directory
-- or "": dont create a directory for the action if only protobuf files are generated 
local project_dir = "../build/" .. (_OPTIONS["platform"] or _ACTION or "")
print("  LOCATION = " .. project_dir)

print("") -- empty line :)


-- touch main.cpp to make sure that the repository defines have effect
if _OPTIONS["platform"] == "Nao" then
  touchfile(path.join(FRAMEWORK_PATH,"Platforms/Source/DCM/NaoRobot/main.cpp"))
end

workspace "NaoTHSoccer"
  platforms {"Native", "Nao"}
  configurations {"OptDebug", "Debug"}
  location (project_dir)

  -- add general paths
  -- this mainly reflects the internal structure of the extern directory
  sysincludedirs {
    FRAMEWORK_PATH .. "/Commons/Source/Messages",
    
    EXTERN_PATH .. "/include",
    EXTERN_PATH .. "/include/glib-2.0",
    EXTERN_PATH .. "/include/gio-unix-2.0",
    EXTERN_PATH .. "/lib/glib-2.0/include",
    EXTERN_PATH .. "/lib64/glib-2.0/include", -- for os that differentiate between lib and lib64
  }

  includedirs { 
    FRAMEWORK_PATH .. "/Commons/Source" 
  }

  -- for os that differentiate between lib and lib64
  syslibdirs { EXTERN_PATH .. "/lib", EXTERN_PATH .. "/lib64"} 

  -- this function can be adjusted in projectconfig.user.lua
  if set_user_defined_paths ~= nil then 
    set_user_defined_paths() 
  end
  
  -- global dependencies ( needed by NaoTHSoccer )
  -- these dependencies are included in the link lists of the binary projects
  naoth_links = {
      "opencv_core",
      "opencv_ml",
      "opencv_imgproc",
      "opencv_objdetect"
  }
  
  -- this is on by default in premake4 stuff
  functionlevellinking "on"
  
  -- set the repository information
  defines {
    "REVISION=\"" .. REVISION .. "\"",
    "USER_NAME=\"" .. USER_NAME .. "\"",
    "BRANCH_PATH=\"" .. BRANCH_PATH .. "\""
  }
  
  -- TODO: howto compile the framework representations properly *inside* the project?
  -- We dont want to assume the location of the NaoTH Make folder for debug purposes
  local NAOTH_PROJECT = path.join(FRAMEWORK_PATH, "..")
  local COMMONS_MESSAGES = path.join(FRAMEWORK_PATH, "Commons/Messages/")
  
  makeprotoc 
  {
    inputFiles  = os.matchfiles(path.join(COMMONS_MESSAGES, "*.proto")),
    cppOut      = path.join(FRAMEWORK_PATH,"Commons/Source/Messages/"),
    javaOut     = path.join(NAOTH_PROJECT, "RobotControl/src/"),
    pythonOut   = path.join(NAOTH_PROJECT, "Utils/py/naoth/naoth/pb/"),
    includeDirs = {COMMONS_MESSAGES}
  }
  
  -- relative to naoth Make folder
  makeprotoc 
  {
    inputFiles  = os.matchfiles(path.join(NAOTH_PROJECT, "NaoTHSoccer/Messages/*.proto")),
    cppOut      = path.join(NAOTH_PROJECT, "NaoTHSoccer/Source/Messages/"),
    javaOut     = path.join(NAOTH_PROJECT, "RobotControl/src/"),
    pythonOut   = path.join(NAOTH_PROJECT, "Utils/py/naoth/naoth/pb/"),
    includeDirs = {COMMONS_MESSAGES, path.join(NAOTH_PROJECT, "NaoTHSoccer/Messages/")}
  }
  

  filter "configurations:Debug"
    defines { "DEBUG" }
    -- FatalWarnings treats compiler/linker warnings as errors
    -- in premake4 linker warnings are not enabled
    flags { "FatalCompileWarnings"}
    symbols "On"

  filter "configurations:OptDebug"
    defines { "DEBUG" }
    -- FatalWarnings treats compiler/linker warnings as errors
    -- in premake4 linker warnings are not enabled
    flags { "FatalCompileWarnings" } --"LinkTimeOptimization"
    optimize "Speed"

  filter { "platforms:Native" }
    targetdir "../dist/Native"

  -- special defines for the Nao robot
  filter { "platforms:Nao" }
    targetdir "../dist/Nao"
    defines { "NAO" }
    system ("linux")
    
    -- HACK: system() doesn't set the target system properly => set the target system manually
    if _OPTIONS["platform"] == "Nao" then
      -- include the Nao platform
      if COMPILER_PATH_NAO ~= nil then
        include (COMPILER_PATH_NAO)
      end
      _TARGET_OS = "linux"
      print("NOTE: set the target OS to " .. os.target())
    end
    
    cppdialect "c++14"
    
    warnings "Extra"
    -- Wconversion is not included in Wall and Wextra
    buildoptions {"-Wconversion"}
    -- These are a lot of warnings that should be fixed, but currently this is not the highest priority
    buildoptions {"-Wno-sign-conversion"}
    -- clang - allow unused functions in cpp files
    buildoptions {"-Wno-unused-function"}
    
    -- for debugging:
    -- buildoptions {"-time"}

    filter "files:../Source/Tools/DataStructures/Spline.cpp"
      warnings "Off"

    filter "files:../Source/Cognition/Modules/VisualCortex/BallDetector/Classifier/*"
      warnings "Off"
    
  -- additional defines for visual studio   
  filter {"system:windows", "action:vs*"}
    defines {"WIN32", "NOMINMAX", "NOGDI", "EIGEN_DONT_ALIGN"}
    buildoptions {"/wd4351"} -- disable warning: "...new behavior: elements of array..."
    buildoptions {"/wd4996"} -- disable warning: "...deprecated..."
    buildoptions {"/wd4290"} -- exception specification ignored (typed specifications are ignored)
    buildoptions {"/wd4800"} -- protobuf 3.4.1 forcing value to bool 'true' or 'false' (performance warning)
    buildoptions {"/wd4503"} -- disable decorated name length exceeded warning
    buildoptions {"/experimental:external  /external:W0"}
    -- TODO those arguments are a copy of the values given to sysincludedirs. Figure out a way to make it less redundant
    buildoptions {"/external:I " .. FRAMEWORK_PATH .. "/Commons/Source/Messages"}
    buildoptions {"/external:I " .. EXTERN_PATH .. "/include"}
    buildoptions {"/external:I " .. EXTERN_PATH .. "/include/glib-2.0"}
    buildoptions {"/external:I " .. EXTERN_PATH .. "/include/gio-unix-2.0"}
    buildoptions {"/external:I " .. EXTERN_PATH .. "/lib/glib-2.0/include"}
    
    cppdialect "c++14"
    links {"ws2_32"}
    
    ignoredefaultlibraries { "MSVCRT" }

    -- this is needed to supress the linker warning in VS2013 if gloabal links are used 
    -- linkoptions { "/ignore:4221" } -- LNK4221: This object file does not define any previously undefined public symbols, so it will not be used by any link operation that consumes this library

    -- from docu:
    -- "path to the working directory, relative to the currently executing script file."
    debugdir ".."
    
    -- for speed up: inline functions. "Auto" => "/Ob2", "Explicit" => "/Ob1"
    inlining ("Auto")
    
    -- for speed up: "Off" => Program Database /Zi. "On" => Program Database foe Edit and Continue /ZI
    editandcontinue "Off" 

    -- remove the nao platform if action is vs*
    removeplatforms { "Nao" }
    
  -- TODO test this on a mac  
  filter "system:macosx"
    defines { "BOOST_SIGNALS_NO_DEPRECATION_WARNING", "EIGEN_DONT_ALIGN" }
    --buildoptions {"-std=c++11"}
    cppdialect "c++14"
    -- disable some warnings
    buildoptions {"-Wno-deprecated-declarations"}
    buildoptions {"-Wno-deprecated-register"}
    buildoptions {"-Wno-logical-op-parentheses"}
    -- use clang on macOS -> there is actual clang support via the toolset function
    -- NOTE: configuration doesn't affect these settings, they NEED to be in a if
    if (os.ishost("macosx") and _OPTIONS["platform"] ~= "Nao") then
      toolset ("clang")
      --premake.gcc.cc = 'clang'
      --premake.gcc.cxx = 'clang++'
    end
    
  -- for linux systems and cygwin 
  filter {"platforms:Native", "action:gmake or gmake2", "system:linux"} 
  -- configuration {"Native", "linux", "gmake"}
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
    --buildoptions {"-std=c++11"}
    cppdialect "c++14"
    
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
   
-----------------------------------------------------------------------   

  group "Core"
    -- commons
    dofile (FRAMEWORK_PATH .. "/Commons/Make/Commons.lua")
      vpaths { ["*"] = FRAMEWORK_PATH .. "/Commons/Source" }

    -- core
    dofile "NaoTHSoccer.lua"
      vpaths { ["*"] = "../Source" } 
      
    -- NOTE: this only makes sense for VS projects
    if os.ishost("windows") and _ACTION ~= nil and string.match(_ACTION, "^vs.*") then
      project "Generate"
        kind "Utility"
        prebuildcommands { "cd ../../Make/ && premake5 --Test " .. _ACTION }
    end
  
  -- set up platforms
  if _OPTIONS["platform"] == "Nao" then
    group "Platform"
      dofile (FRAMEWORK_PATH .. "/Platforms/Make/NaoSMAL.lua")
        if AL_DIR ~= nil then
          sysincludedirs {AL_DIR .. "/include"}
          syslibdirs {AL_DIR .. "/lib"}
        end
        vpaths { ["*"] = FRAMEWORK_PATH .. "/Platforms/Source/NaoSMAL" }
        defines { "BOOST_SIGNALS_NO_DEPRECATION_WARNING" }
        -- ACHTUNG: NaoSMAL doesn't build with the flag -std=c++11 (because of Boost)
        cppdialect "gnu++11"
        
        dofile (FRAMEWORK_PATH .. "/Platforms/Make/NaoRobot.lua")
        kind "ConsoleApp"
        links { "NaoTHSoccer", "Commons", naoth_links}
        vpaths { ["*"] = FRAMEWORK_PATH .. "/Platforms/Source/NaoRobot" }
       
        dofile (FRAMEWORK_PATH .. "/Platforms/Make/LolaAdaptor.lua")
        kind "ConsoleApp"
        links { "NaoTHSoccer", "Commons", naoth_links}
        vpaths { ["*"] = FRAMEWORK_PATH .. "/Platforms/Source/LolaAdaptor" }
      
    -- generate tests if required
    if _OPTIONS["Test"] ~= nil then
      group "Test"
        dofile ("../Test/Make/EigenPerformance.lua")
          kind "ConsoleApp"
          vpaths { ["*"] = "../Test/Source/EigenPerformance" }
          
        dofile ("../Test/Make/GeneralAlignment.lua")
            kind "ConsoleApp"
            vpaths { ["*"] = "../Test/Source/GeneralAlignment" }

        dofile ("../Test/Make/Optimizers.lua")
            kind "ConsoleApp"
            vpaths { ["*"] = "../Test/Source/Optimizers" }

	    dofile ("../Test/Make/Polygon.lua")
            kind "ConsoleApp"
            vpaths { ["*"] = "../Test/Source/Polygon" }
        
        dofile ("../Test/Make/LoLa.lua")
            kind "ConsoleApp"
            vpaths { ["*"] = "../Test/Source/LoLa" }
            
        dofile ("../Test/Make/AudioRecorder.lua")
          kind "ConsoleApp"
          vpaths { ["*"] = "../Test/Source/AudioRecorder" }
    end

    
  else
    group "Platform"
      dofile (FRAMEWORK_PATH .. "/Platforms/Make/SimSpark.lua")
        kind "ConsoleApp"
        links { "NaoTHSoccer", "Commons", naoth_links}
        vpaths { ["*"] = FRAMEWORK_PATH .. "/Platforms/Source/SimSpark" }
        debugargs { "--sync" }
        
      dofile (FRAMEWORK_PATH .. "/Platforms/Make/LogSimulator.lua")
        kind "ConsoleApp"
        links { "NaoTHSoccer", "Commons", naoth_links}
        vpaths { ["*"] = FRAMEWORK_PATH .. "/Platforms/Source/LogSimulator" }
        
      dofile (FRAMEWORK_PATH .. "/Platforms/Make/DummySimulator.lua")
        links { "NaoTHSoccer", "Commons", naoth_links}
        vpaths { ["*"] = FRAMEWORK_PATH .. "/Platforms/Source/DummySimulator" }

      dofile (FRAMEWORK_PATH .. "/Platforms/Make/ScriptableSimulator.lua")
        links { "NaoTHSoccer", "Commons", naoth_links}
        vpaths { ["*"] = FRAMEWORK_PATH .. "/Platforms/Source/DummySimulator" }
        
      dofile (FRAMEWORK_PATH .. "/Platforms/Make/WhistleSimulator.lua")
        kind "ConsoleApp"
        links { "NaoTHSoccer", "Commons", naoth_links}
        vpaths { ["*"] = FRAMEWORK_PATH .. "/Platforms/Source/WhistleSimulator" }

      dofile (FRAMEWORK_PATH .. "/Platforms/Make/Webots.lua")
        kind "ConsoleApp"
        links { "NaoTHSoccer", "Commons", naoth_links}
        vpaths { ["*"] = FRAMEWORK_PATH .. "/Platforms/Source/Webots" }
        debugargs { "--sync" }
      
    -- generate tests if required
    if _OPTIONS["Test"] ~= nil then
      group "Test"
        dofile ("../Test/Make/EigenPerformance.lua")
          kind "ConsoleApp"
          vpaths { ["*"] = "../Test/Source/EigenPerformance" }

        dofile ("../Test/Make/GeneralAlignment.lua")
          kind "ConsoleApp"
          vpaths { ["*"] = "../Test/Source/GeneralAlignment" }

        dofile ("../Test/Make/Optimizers.lua")
          kind "ConsoleApp"
          vpaths { ["*"] = "../Test/Source/Optimizers" }

	    dofile ("../Test/Make/Polygon.lua")
          kind "ConsoleApp"
          vpaths { ["*"] = "../Test/Source/Polygon" }
      
      dofile ("../Test/Make/LoLa.lua")
          kind "ConsoleApp"
          vpaths { ["*"] = "../Test/Source/LoLa" }
    end

    -- generate LogSimulatorJNI if required
    if _OPTIONS["JNI"] ~= nil then
      group "Platform"
        dofile (FRAMEWORK_PATH .. "/Platforms/Make/LogSimulatorJNI.lua")
          kind "SharedLib"
          links { "NaoTHSoccer", "Commons", naoth_links}
          vpaths { ["*"] = FRAMEWORK_PATH .. "/Platforms/Source/LogSimulatorJNI" }
    end
  end
