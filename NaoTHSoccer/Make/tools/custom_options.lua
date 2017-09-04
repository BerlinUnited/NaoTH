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

newoption {
   trigger     = "platform",
   description = "Set the target platform",
}

if _OPTIONS["Test"] ~= nil then
      print("DEBUG: Generate test projects")
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