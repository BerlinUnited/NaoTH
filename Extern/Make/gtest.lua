project "gtest"
  kind "StaticLib"
  language "C"
  if os.is("windows") then
    defines {"WIN32"}
  end -- if windows
  includedirs { "../source/gtest-1.4.0/include/","../source/gtest-1.4.0/" }
  files { "../source/gtest-1.4.0/src/*.cc" }
