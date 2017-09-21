-- Test for checking the premake version

if not premake.checkVersion(premake._VERSION, ">=5.0.0-alpha12") then
	print("ERROR: Premake Version" .. premake._VERSION .. " is to old. Use at least 5.0.0-alpha12")
	os.exit()
end

require "tools/clean_action" -- get custom clean action
dofile "tools/custom_options.lua" -- define custom options

-- TODO implement projectconfig lua stuff
dofile "projectconfig.lua" -- load the global default settings

-- load some helpers
dofile (FRAMEWORK_PATH .. "/BuildTools/info.lua")
dofile (FRAMEWORK_PATH .. "/BuildTools/protoc.lua")
-- dofile (FRAMEWORK_PATH .. "/BuildTools/qtcreator.lua")
dofile (FRAMEWORK_PATH .. "/BuildTools/qtcreator_2.7+.lua")

-- include the Nao platform
if COMPILER_PATH_NAO ~= nil then
	include (COMPILER_PATH_NAO)
end

print("INFO: generating solution NaoTHSoccer")
print("  PLATFORM = " .. PLATFORM)
print("  OS = " .. os.target())
print("  ACTION = " .. (_ACTION or "NONE"))
print()


workspace "NaoTHSoccer"
	platforms {"Native", "Nao"}
	configurations {"OptDebug", "Debug"}
	location "../build"
	
	-- global lib path for all configurations
	libdirs (PATH["libs"])
  
	-- global include path for all projects and configurations
	includedirs (PATH["includes"])
  
  -- global dependencies ( needed by NaoTHSoccer )
  --[[
	links {
			"opencv_core",
			"opencv_ml",
			"opencv_imgproc",
			"opencv_objdetect"
	}
  ]]--
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
	local COMMONS_MESSAGES = FRAMEWORK_PATH .. "/Commons/Messages/"

	local COMMONS_MESSAGES = FRAMEWORK_PATH .. "/Commons/Messages/"
	invokeprotoc(
		{ 
		  COMMONS_MESSAGES .. "CommonTypes.proto", 
		  COMMONS_MESSAGES .. "Framework-Representations.proto", 
		  COMMONS_MESSAGES .. "Messages.proto"
		},
		FRAMEWORK_PATH .. "/Commons/Source/Messages/", 
		"../../RobotControl/RobotConnector/src/", 
		"../../Utils/py/naoth/naoth",
		{COMMONS_MESSAGES}
	)
	-- I dont want to assume the location of the NaoTH Make folder for debug purposes
	NAOTH_MAKE = path.join(FRAMEWORK_PATH, "../NaoTHSoccer/Make")

	-- relative to naoth Make folder
	invokeprotoc(
		{path.join(NAOTH_MAKE, "../Messages/Representations.proto")},
		path.join(NAOTH_MAKE, "../Source/Messages/"),
		path.join(NAOTH_MAKE, "../../RobotControl/RobotConnector/src/"),
		path.join(NAOTH_MAKE, "../../Utils/py/naoth/naoth"),		
		{COMMONS_MESSAGES, path.join(NAOTH_MAKE, "../Messages/")}
	)

	filter "configurations:Debug"
		defines { "DEBUG" }		
		-- FatalWarnings treats compiler/linker warnings as errors
		-- in premake4 linker warnings are not enabled
		flags { "FatalCompileWarnings" }
		symbols "On"

	filter "configurations:OptDebug"	
		defines { "DEBUG" }
		-- FatalWarnings treats compiler/linker warnings as errors
		-- in premake4 linker warnings are not enabled
		flags { "FatalCompileWarnings" }
		optimize "Speed"

	filter { "platforms:Native" }
		targetdir "../dist/Native"

  -- special defines for the Nao robot
	filter { "platforms:Nao" }
		defines { "NAO" }
		targetdir "../dist/Nao"
		--warnings "Extra"

		-- for debugging
		-- buildoptions {"-time"}

		-- disable warning "comparison always true due to limited range of data type"
		-- this warning is caused by protobuf 2.4.1
		buildoptions {"-Wno-type-limits"}
		-- some of the protobuf messages are marked as deprecated but are still in use for legacy reasons
		buildoptions {"-Wno-deprecated-declarations"}
		-- buildoptions {"-Wconversion"}
		buildoptions {"-std=c++11"}

	-- additional defines for visual studio 	
	filter {"system:windows", "action:vs*"}
		defines {"WIN32", "NOMINMAX", "NOGDI", "EIGEN_DONT_ALIGN"}
		buildoptions {"/wd4351"} -- disable warning: "...new behavior: elements of array..."
		buildoptions {"/wd4996"} -- disable warning: "...deprecated..."
		buildoptions {"/wd4290"} -- exception specification ignored (typed specifications are ignored)
		links {"ws2_32"}
    
    linkoptions{ "/NODEFAULTLIB:MSVCRT" }
    --[[
    -- this is needed to supress the linker warning in VS2013 if gloabal links are used 
    linkoptions {
			"/ignore:4221", -- LNK4221: This object file does not define any previously undefined public symbols, so it will not be used by any link operation that consumes this library
		}
    ]]--
    
		debugdir "$(SolutionDir).."
		
		-- remove the nao platform if action is vs*
		removeplatforms { "Nao" }
		
	-- TODO test this on a mac	
	filter "system:macosx"
		defines { "BOOST_SIGNALS_NO_DEPRECATION_WARNING", "EIGEN_DONT_ALIGN" }
		buildoptions {"-std=c++11"}
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
	filter {"platforms:Native", "action:gmake", "system:linux"} 
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
-----------------------------------------------------------------------	 -- commons
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
      targetextension ("")
      
    dofile (FRAMEWORK_PATH .. "/Platforms/Make/NaoRobot.lua")
      kind "ConsoleApp"
      links { "NaoTHSoccer", "Commons", naoth_links}
      vpaths { ["*"] = FRAMEWORK_PATH .. "/Platforms/Source/NaoRobot" }
      targetextension ("")
      
  else
    dofile (FRAMEWORK_PATH .. "/Platforms/Make/SimSpark.lua")
      kind "ConsoleApp"
      links { "NaoTHSoccer", "Commons", naoth_links}
      vpaths { ["*"] = FRAMEWORK_PATH .. "/Platforms/Source/SimSpark" }
      debugargs { "--sync" }
      
    dofile (FRAMEWORK_PATH .. "/Platforms/Make/LogSimulator.lua")
      kind "ConsoleApp"
      links { "NaoTHSoccer", "Commons", naoth_links}
      vpaths { ["*"] = FRAMEWORK_PATH .. "/Platforms/Source/LogSimulator" }
      
    dofile (FRAMEWORK_PATH .. "/Platforms/Make/LogSimulatorJNI.lua")
      kind "SharedLib"
      links { "NaoTHSoccer", "Commons", naoth_links}
      vpaths { ["*"] = FRAMEWORK_PATH .. "/Platforms/Source/LogSimulatorJNI" }
      
    -- generate tests if required
    if _OPTIONS["Test"] ~= nil then
      dofile ("../Test/Make/BallEvaluator.lua")
        kind "ConsoleApp"
        links { "NaoTHSoccer", "Commons", naoth_links}
        vpaths { ["*"] = "../Test/Source/BallEvaluator" }
    end
  end