solution "NaoRunner"
  kind "StaticLib"
  language "C++"
  targetdir "../Lib/"
  
  platforms {"Native"}
  configurations {"Release","Debug"}
  
  -- store the makefiles the Make/ directory
  location "."
  
  dofile "Interface.lua"
  dofile "Webots.lua"
