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
  
 
  files {"../Tests/**.cpp"}

  includedirs {
        "../Tests/",
	"../Source/",
        "../../Framework/NaoTH-Tools/Source/",
        "../../Framework/DebugCommunication/Source/"}

  links { "NaoTH-Tools", "DebugCommunication", "pthread" }
  
  links {"gtest", "gmock", "pthread"}
  
  targetdir "../Tests"
  targetname "executeTests"
