--local EXTERN_PATH = path.getabsolute("../../../Extern")

project "NaoTH-Commons-Tests"
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
    "../Source/"
  }
  
  links {
	"NaoTH-Commons", 
	"glib-2.0",
	"gio-2.0",
	"gobject-2.0", 
	"protobuf",
    "gtest", 
	"gmock"
  }
    
  targetname "testCommons"
  
  configuration {"linux"}
    linkoptions {"-Wl,-rpath \"" .. path.getabsolute(EXTERN_PATH .. "/lib/") .. "\""}

