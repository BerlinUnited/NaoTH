solution "naoth-sources"

configurations {"Release"}

configuration "Release"
    flags {"Optimize"}

targetdir "../"	

project "sfsexp"
  kind "StaticLib"
  language "C"
  if os.is("windows") then
    defines {"WIN32"}
  end -- if windows
  files { "*.c" }
