-- define the extern directory 
-- TODO Cant we use the var from premake4.lua here, maybe another keyword?
local extern_dir = "../../../Extern"

project "NaoTH-Tools-Tests"
  kind "ConsoleApp"
  language "C++"

    if _OPTIONS["gcov"] then
      -- If a project includes multiple calls to buildoptions the lists are 
      -- concatenated, in the order in which they appear in the script.
      print("Setting -ftest-coverage in test suite")
      buildoptions { "-fprofile-arcs", "-ftest-coverage"}
      linkoptions {"-fprofile-arcs", "-ftest-coverage"} 
  end -- option == gcov     
  
 
  files {"../Tests/**.cpp", "../Tests/**.h"}

  includedirs {
        "../Tests/",
        "../Source/",
    extern_dir .. "/include",
    extern_dir .. "/include/glib-2.0/",
    extern_dir .. "/lib/glib-2.0/include/"
  }
  
  links {"NaoTH-Tools", "glib-2.0","gio-2.0","gobject-2.0", "protobuf",
    "gtest", "gmock", "pthread"
  }
    
  targetdir "../dist/Native"
  targetname "executeTests"

  linkoptions {"-Wl"}

