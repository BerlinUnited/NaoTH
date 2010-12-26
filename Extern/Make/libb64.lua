project "b64"
  kind "StaticLib"
  language "C"
  if os.is("windows") then
    defines {"WIN32"}
  end -- if windows
  files { "../source/libb64/*.c" }
