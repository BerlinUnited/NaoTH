project "gmock"
  kind "StaticLib"
  language "C"

  includedirs {"include/", "", "../gtest-1.4.0/include/"}
  
  files { "src/gmock-all.cc","src/gmock-internal-utils.cc","src/gmock-printers.cc",
	  "src/gmock.cc","src/gmock-cardinalities.cc","src/gmock-matchers.cc",
          "src/gmock-spec-builders.cc","src/gmock_main.cc" }
  
-- END gmock
