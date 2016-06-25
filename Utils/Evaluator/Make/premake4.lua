

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
  targetdir "../"

  -- global lib path for all configurations
  -- additional includes
  libdirs (PATH["libs"])
  libdirs ("../../../NaoTHSoccer/dist/Native")
  
  -- global include path for all projects and configurations
  includedirs (PATH["includes"])

  if(_ACTION == "gmake") then
      -- "position-independent code" needed to compile shared libraries.
      -- In our case it's only the NaoSMAL. So, we probably don't need this one.
      -- Premake4 automatically includes -fPIC if a project is declared as a SharedLib.
      -- http://www.akkadia.org/drepper/dsohowto.pdf
      --buildoptions {"-fPIC"}
      -- may be needed for newer glib2 versions, remove if not needed
      buildoptions {"-Wno-deprecated-declarations"}
      buildoptions {"-Wno-deprecated"}
      buildoptions {"-std=c++11"}
      flags { "ExtraWarnings" }
     -- links {"pthread"}
    
      -- Why? OpenCV is always dynamically linked and we can only garantuee that there is one version in Extern (Thomas)
     linkoptions {"-Wl,-rpath \"" .. path.getabsolute(EXTERN_PATH .. "/lib/") .. "\""}
    end
  dofile "Evaluator.lua"
