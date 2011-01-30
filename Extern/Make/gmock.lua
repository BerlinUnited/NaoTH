project "gmock"
  kind "StaticLib"
  language "C"
  if os.is("windows") then
    defines {"WIN32"}
  end -- if windows
  includedirs { "../source/gmock-1.4.0/include/","../source/gmock-1.4.0/","../source/gtest-1.4.0/include/"}
  files { "../source/gmock-1.4.0/src/*.cc" }
