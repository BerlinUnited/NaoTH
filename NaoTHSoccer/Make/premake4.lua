dofile "helper/naocrosscompile.lua"
dofile "helper/extract_todos.lua"

newaction {
  trigger = "todo",
  description = "extracts all todos",
  execute = function()
    
    local result = {};
    
    extract_todos_files(os.matchfiles(".../Source/**.cpp"), result);
    extract_todos_files(os.matchfiles("../Source/**.h"), result);
    
    io.output("../TODO")
    io.write("= TODO list =\n\n")
    io.write("(automatically generated from source, please do not edit)\n\n")
    io.write("|| '''Nr.''' || '''Status''' || '''Message''' || '''File''' ||\n")
    
    local i = 1
    
    for k,v in pairs(result) do
      local message = string.gsub(v.message, "\n", "")
      message = string.gsub(message, "\r", "")
      
      io.write("|| " .. i .. " || " .. v.kind .. " || " .. message .. " || [source:trunk/Projects/NaoController/Make/" ..  
        v.file .. "#L" .. v.line .. " " .. path.getname(v.file) .. ":" .. v.line .. "] ||\n")
      i = i +1
    end
    print("found " .. # result .. " issues")
    
  end
}

-- definition of the solution
solution "NaoTHSoccer"
  platforms {"Native", "Nao"}
  configurations {"Debug", "OptDebug", "Release"}
  
  CORE_PATH = {
    path.getabsolute("../Source/Core/Cognition/"), 
    path.getabsolute("../Source/Core/Motion/"),
    path.getabsolute("../Source/")  
  }
  
  CORE = {"NaoTHSoccer", "DebugCommunication"}

  
  -- debug configuration
  configuration { "Debug" }
    defines { "DEBUG" }
    flags { "Symbols" }
  
  configuration { "OptDebug" }
    defines { "DEBUG" }
    flags { "Optimize" }
      
  configuration{"Native"}
    includedirs {
      "../../Extern/include/",
      "../../Extern/include/glib-2.0/",
      "../../Extern/lib/glib-2.0/include/"}
    libdirs {
      "../../Extern/lib"
    }
    targetdir "../dist/Native"
    
  configuration {"Nao"}
    targetdir "../dist/Nao"
  
  -- additional defines for windows
  configuration {"windows"}
    defines {"WIN32", "NOMINMAX"}
    
  configuration {"linux"}
    buildoptions {"-fPIC"}
      
  -- base
  dofile "../../Framework/NaoTH-Tools/Make/NaoTHTools.lua"
  dofile "../../Framework/DebugCommunication/Make/DebugCommunication.lua"
  
  -- platforms
  if(_OPTIONS["platform"] == "Nao") then
    dofile "../../Framework/Platforms/Make/DCM.lua"
  else
    dofile "../../Framework/Platforms/Make/SimSpark.lua"
    dofile "../../Framework/Platforms/Make/Webots.lua"
    dofile "../../Framework/Platforms/Make/LogSimulator.lua"    
    dofile "Tests.lua"
  end
  -- the core
  dofile "NaoTHSoccer.lua"
