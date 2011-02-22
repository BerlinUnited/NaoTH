project "Tests"
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
        "../../Framework/NaoTH-Tools/Source/",
        "../../Framework/DebugCommunication/Source/"}
  
  links {"NaoTHSoccer", "NaoTH-Tools", "DebugCommunication", 
    "glib-2.0","gio-2.0","gobject-2.0", "protobuf",
    "gtest", "gmock", "pthread"
  }
  
  targetdir "../dist/Native"
  targetname "executeTests"
  
  configuration {"linux"}
    linkoptions {"-Wl,-rpath \"" .. path.getabsolute("../../Extern/lib/") .. "\""}
    --postbuildcommands {"../dist/Native/executeTests"}
