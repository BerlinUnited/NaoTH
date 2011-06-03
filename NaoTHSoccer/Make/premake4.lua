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
    path.getabsolute("../Source/Core/")
  }
  
  CORE = {"NaoTHSoccer"}

  -- for all configurations
  includedirs {
	  "../../Framework/NaoTH-Commons/Source/"
  }
  -- debug configuration
  configuration { "Debug" }
    defines { "DEBUG" }
    flags { "Symbols", "ExtraWarnings", "FatalWarnings" }
  
  configuration { "OptDebug" }
    defines { "DEBUG" }
    flags { "Optimize", "ExtraWarnings", "FatalWarnings" }
      
  configuration{"Native"}
    libdirs {
       "../../Extern/lib"
    }
    targetdir "../dist/Native"
    
  configuration {"Nao"}
    libdirs {
       "../../Extern/lib/Nao"
    }
    targetdir "../dist/Nao"
  
  -- additional defines for windows
  configuration {"windows"}
    defines {"WIN32", "NOMINMAX"}
    
  configuration {"linux"}
    buildoptions {"-fPIC"}
  
  -- includes
  includedirs {
       "../../Extern/include/",
       "../../Extern/include/glib-2.0/",
       "../../Extern/lib/glib-2.0/include/"}
  -- libs
  

  -- base
  dofile "../../Framework/NaoTH-Commons/Make/NaoTH-Commons.lua"
  
  -- platforms
  if(_OPTIONS["platform"] == "Nao") then
    dofile "../../Framework/Platforms/Make/DCM.lua"
  else
    dofile "../../Framework/Platforms/Make/SimSpark.lua"
    dofile "../../Framework/Platforms/Make/Webots.lua"
    dofile "../../Framework/Platforms/Make/LogSimulator.lua"    
    dofile "Tests.lua"
    dofile "../../Framework/NaoTH-Commons/Make/Tests.lua"
  end
  -- the core
  dofile "NaoTHSoccer.lua"
