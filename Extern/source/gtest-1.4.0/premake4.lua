project "gtest"
  kind "StaticLib"
  language "C"

  includedirs {"include/", ""}
  
  files {"src/gtest-death-test.cc","src/gtest-filepath.cc","src/gtest-test-part.cc",
        "src/gtest-port.cc","src/gtest-typed-test.cc","src/gtest.cc",
        "src/gtest_main.cc"}
  
-- END gtest
