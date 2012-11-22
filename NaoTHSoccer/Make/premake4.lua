dofile "helper/naocrosscompile_v3.lua"
dofile "helper/naocrosscompile.lua"
dofile "helper/extract_todos.lua"
dofile "helper/qtcreator.lua"

newaction {
  trigger = "todo",
  description = "extracts all todos",
  execute = function()
    
    local result = {};
    
    extract_todos_files(os.matchfiles("../Source/**.cpp"), result);
    extract_todos_files(os.matchfiles("../Source/**.h"), result);
    extract_todos_files(os.matchfiles("../../Framework/NaoTH-Commmons/Source/**.cpp"), result);
    extract_todos_files(os.matchfiles("../../Framework/NaoTH-Commmons/Source/**.h"), result);
    extract_todos_files(os.matchfiles("../../Framework/Platforms/Source/**.cpp"), result);
    extract_todos_files(os.matchfiles("../../Framework/Platforms/Source/**.h"), result);
    
    io.output("../TODO")
    io.write("= TODO list =\n\n")
    io.write("(automatically generated from source, please do not edit)\n\n")
    io.write("|| '''Nr.''' || '''Status''' || '''Message''' || '''File''' ||\n")
    
    local i = 1
    
    for k,v in pairs(result) do
      local message = string.gsub(v.message, "\n", "")
      message = string.gsub(message, "\r", "")
      
      io.write("|| " .. i .. " || " .. v.kind .. " || " .. message .. " || [source:NaoTHSoccer/Make/" ..  
        v.file .. "#L" .. v.line .. " " .. path.getname(v.file) .. ":" .. v.line .. "] ||\n")
      i = i +1
    end
    print("found " .. # result .. " issues")
    
  end
}

-- definition of the solution
solution "NaoTHSoccer"
  platforms {"Native", "Nao"}
  configurations {"OptDebug", "Debug", "Release"}
  
  FRAMEWORK_PATH = path.getabsolute("../../Framework")
  EXTERN_PATH = path.getabsolute("../../Extern")
  
  CORE_PATH = {
    path.getabsolute("../Source/Core/Cognition/"), 
    path.getabsolute("../Source/Core/Motion/"),
    path.getabsolute("../Source/Core/")
  }
  
  CORE = {"NaoTHSoccer"}

  -- for all configurations
  includedirs {
	FRAMEWORK_PATH.."/NaoTH-Commons/Source/"
  }
  -- debug configuration
  configuration { "Debug" }
    defines { "DEBUG" }
    flags { "Symbols", "FatalWarnings" }
  
  configuration { "OptDebug" }
    defines { "DEBUG" }
    -- flags { "Optimize", "FatalWarnings" }
    flags { "Optimize" }
         
  configuration{"Native"}
    includedirs {
      EXTERN_PATH .. "/include/",
      EXTERN_PATH .. "/include/glib-2.0/",
      EXTERN_PATH .. "/lib/glib-2.0/include/",
      "/usr/include/"
	  }
    libdirs {
      EXTERN_PATH .. "/lib"
    }
    
    if os.get() == "linux" then      
      -- try to get the pkg-config include-instructions
      local f = io.popen("pkg-config --cflags glib-2.0")
      local l = f:read("*a")
      buildoptions { l }
    end

    targetdir "../dist/Native"
    
  configuration {"Nao"}
    defines { "NAO" }
    targetdir "../dist/Nao"

  -- additional defines for windows
  if(_OPTIONS["platform"] ~= "Nao" and _ACTION ~= "gmake") then
  configuration {"windows"}
    defines {"WIN32", "NOMINMAX"}
    buildoptions {"/wd4351", -- disable warning: "...new behavior: elements of array..."
				  "/wd4996", -- disable warning: "...deprecated..."
				  "/wd4290"} -- exception specification ignored (typed stecifications are ignored)
	links {"ws2_32"}
  end
  
  -- configuration {"linux"}
  if(_ACTION == "gmake") then
    buildoptions {"-fPIC"}
    flags { "ExtraWarnings" }
  end
      
  -- base
  dofile (FRAMEWORK_PATH .. "/NaoTH-Commons/Make/NaoTH-Commons.lua")
  
  -- platforms
  if(_OPTIONS["platform"] == "Nao") then
    dofile (FRAMEWORK_PATH .. "/Platforms/Make/DCM.lua")
  elseif(_OPTIONS["platform"] == "Naov3") then
    dofile (FRAMEWORK_PATH .. "/Platforms/Make/DCM_v3.lua")
  else
    dofile (FRAMEWORK_PATH .. "/Platforms/Make/SimSpark.lua")
	dofile (FRAMEWORK_PATH .. "/Platforms/Make/SPL_SimSpark.lua")
    dofile (FRAMEWORK_PATH .. "/Platforms/Make/Webots.lua")
    dofile (FRAMEWORK_PATH .. "/Platforms/Make/LogSimulator.lua")
	dofile (FRAMEWORK_PATH .. "/Platforms/Make/OpenCVImageLoader.lua")
	dofile (FRAMEWORK_PATH .. "/Platforms/Make/OpenCVWebCam.lua")
    dofile "Tests.lua"
    dofile (FRAMEWORK_PATH .. "/NaoTH-Commons/Make/Tests.lua")
  end
  -- the core
  dofile "NaoTHSoccer.lua"
