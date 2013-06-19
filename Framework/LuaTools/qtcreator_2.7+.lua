dofile "qtcreator/_qtcreator_2.7+.lua"
dofile "qtcreator/qtcreator_misc.lua"

 
  -- quick fix von maggie
  premake.qtcreator27.files = premake.qtcreator.files
  premake.qtcreator27.includes = premake.qtcreator.includes
  premake.qtcreator27.defines = premake.qtcreator.defines
