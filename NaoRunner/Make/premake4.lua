solution "NaoRunner"
  kind "StaticLib"
  language "C++"
  targetdir "dist/"
  
  platforms {"Native"}
  configurations {"Release","Debug"}
  
  -- store the makefiles the Make/ directory
  location "."
  
  include "../Interface/"
  include "../LogSimulator/"
