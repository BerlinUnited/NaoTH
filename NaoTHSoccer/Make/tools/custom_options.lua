newoption {
   trigger     = "Test",
   description = "Generate test projects"
}

newoption {
   trigger     = "JNI",
   description = "Generate LogSimulatorJNI project"
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

newoption {
   trigger     = "platform",
   description = "Set the target platform",
}

newoption {
   trigger     = "verbose",
   description = "Print additional information regarding the include and lib paths",
}


if _OPTIONS["Test"] ~= nil then
      print("DEBUG: Generate test projects")
end

if _OPTIONS["JNI"] ~= nil then
      print("Generate LogSimulatorJNI project")
end

if _OPTIONS["Wno-conversion"] ~= nil then
      print("DEBUG: Disable the -Wconversion warning for gcc")
end

if _OPTIONS["Wno-misleading-indentation"] ~= nil then
      print("DEBUG: Disable the -Wmisleading-indentation warning/error for gcc (6.0+)")
end

if _OPTIONS["Wno-ignored-attributes"] ~= nil then
      print("DEBUG: Disable the -Wignored-attributes warning/error for gcc (6.0+)")
end

PLATFORM = _OPTIONS["platform"]
if PLATFORM == nil then
  defaultplatform "Native"
  PLATFORM = "Native" -- just a variable for printing the platform name
end

-- 
if _OPTIONS["verbose"] ~= nil then
  local validate_project = premake.validation.elements.project
  
  premake.validation.elements.project = function(prj)
    print ("\n")
    print ("INFO: project " .. prj.name .. ":")
    
    -- print some debug output
    print("INFO: list includedirs")
    checkPaths(prj.includedirs)
    print("INFO: list libdirs")
    checkPaths(prj.libdirs)
    print("INFO: list sysincludedirs")
    checkPaths(prj.sysincludedirs)
    print("INFO: list syslibdirs")
    checkPaths(prj.syslibdirs)
    
    -- call the default premake function
    return validate_project(prj)
  end
end