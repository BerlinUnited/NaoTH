

solution "extern-source"
  
  configurations {"Release"}

  configuration "Release"
    flags {"Optimize"}
    targetdir "dist"

  -- include projects  
  dofile "libb64.lua"
  dofile "gmock.lua"
  dofile "gtest.lua"
  dofile "install.lua"
