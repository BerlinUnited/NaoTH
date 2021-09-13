filter "configurations:Debug"
  -- FatalWarnings treats compiler/linker warnings as errors
  -- in premake4 linker warnings are not enabled
  flags { "FatalCompileWarnings"}

filter "configurations:OptDebug"
  -- FatalWarnings treats compiler/linker warnings as errors
  -- in premake4 linker warnings are not enabled
  --flags { "FatalCompileWarnings" } --"LinkTimeOptimization"

-- special defines for the Nao robot
  filter { "platforms:Nao" }   
    warnings "Extra"
    -- Wconversion is not included in Wall and Wextra
    buildoptions {"-Wconversion"}
    -- These are a lot of warnings that should be fixed, but currently this is not the highest priority
    buildoptions {"-Wno-sign-conversion"}
    -- clang - allow unused functions in cpp files
    buildoptions {"-Wno-unused-function"}

    -- TODO do we really want this only for nao? maybe its enough because only clang complains
    filter "files:../Source/Tools/DataStructures/Spline.cpp"
      warnings "Off"

    filter "files:../Source/Cognition/Modules/VisualCortex/BallDetector/Classifier/*"
      warnings "Off"

-- additional defines for visual studio   
filter {"system:windows", "action:vs*"}
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

-- TODO test this on a mac  
filter "system:macosx"
  defines { "BOOST_SIGNALS_NO_DEPRECATION_WARNING" }
  -- disable some warnings
  buildoptions {"-Wno-deprecated-declarations"}
  buildoptions {"-Wno-deprecated-register"}
  buildoptions {"-Wno-logical-op-parentheses"}

-- for linux systems and cygwin 
filter {"platforms:Native", "action:gmake or gmake2", "system:linux"} 
  -- may be needed for newer glib2 versions, remove if not needed
  buildoptions {"-Wno-deprecated-declarations"}
  buildoptions {"-Wno-deprecated"}
  -- Prohibit GCC to be clever and use undefined behavior for some optimizations
  -- (see http://www.airs.com/blog/archives/120 for some nice explanation)
  buildoptions {"-fno-strict-overflow"}
  
  if _OPTIONS["Wno-conversion"] == nil then
    buildoptions {"-Wconversion"}
  end

  if _OPTIONS["Wno-misleading-indentation"] ~= nil then
    buildoptions {"-Wno-misleading-indentation"}
  end

  if _OPTIONS["Wno-ignored-attributes"] ~= nil then
    buildoptions {"-Wno-ignored-attributes"}
  end
