solution "NaoRunner"
  kind "StaticLib"
  language "C++"
  targetdir "dist/"
  
  platforms {"Native"}
  configurations {"Release","Debug"}
  
  include "Interface/"
