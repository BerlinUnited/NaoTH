

-- load the global default settings
dofile "../../../NaoTHSoccer/Make/projectconfig.lua"

-- load some helpers
dofile (FRAMEWORK_PATH .. "/BuildTools/info.lua")
dofile (FRAMEWORK_PATH .. "/BuildTools/protoc.lua")
--dofile (FRAMEWORK_PATH .. "/BuildTools/ilpath.lua")
dofile (FRAMEWORK_PATH .. "/BuildTools/qtcreator.lua")
dofile (FRAMEWORK_PATH .. "/BuildTools/qtcreator_2.7+.lua")
--dofile (FRAMEWORK_PATH .. "/BuildTools/extract_todos.lua")


-- definition of the solution
solution "EvaluatorSolution"
  platforms {"Native"}
  configurations {"OptDebug", "Debug", "Release"}
  location "../build"
  targetdir "../dist"

  -- global lib path for all configurations
  -- additional includes
  libdirs (PATH["libs"])
  libdirs ("../../../NaoTHSoccer/dist/Native")
  
  -- global include path for all projects and configurations
  includedirs (PATH["includes"])

  
  configuration { "Debug" }
    defines { "DEBUG" }
    flags { "Symbols", "FatalWarnings" }
  
  configuration { "OptDebug" }
    defines { "DEBUG" }
    flags { "Optimize", "FatalWarnings" }
	
  -- additional defines for visual studio
  configuration {"windows", "vs*"}
    defines {"WIN32", "NOMINMAX", "EIGEN_DONT_ALIGN"}
    buildoptions {"/wd4351"} -- disable warning: "...new behavior: elements of array..."
    buildoptions {"/wd4996"} -- disable warning: "...deprecated..."
    buildoptions {"/wd4290"} -- exception specification ignored (typed stecifications are ignored)
    links {"ws2_32"}
    debugdir "$(SolutionDir).."
  
  
  configuration {"linux or windows", "Native", "gmake"}
    -- "position-independent code" needed to compile shared libraries.
    -- In our case it's only the NaoSMAL. So, we probably don't need this one.
    -- Premake4 automatically includes -fPIC if a project is declared as a SharedLib.
    -- http://www.akkadia.org/drepper/dsohowto.pdf
    buildoptions {"-fPIC"}
    
    -- may be needed for newer glib2 versions, remove if not needed
    buildoptions {"-Wno-deprecated-declarations"}
    buildoptions {"-Wno-deprecated"}
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
    defines { "BOOST_SIGNALS_NO_DEPRECATION_WARNING" }
    buildoptions {"-std=c++11"}
    -- disable some warnings
    buildoptions {"-Wno-deprecated-declarations"}
    buildoptions {"-Wno-deprecated-register"}
    buildoptions {"-Wno-logical-op-parentheses"}
    -- use clang on macOS
    -- NOTE: configuration doesn't affect these settings, they NEED to be in a if
    if os.is("macosx") then
      premake.gcc.cc = 'clang'
      premake.gcc.cxx = 'clang++'
    end
  
  
  dofile "Evaluator.lua"
