-- install stuff

function install()

  local prefix = "../../Extern"
  if(_OPTIONS["prefix"]) then
    prefix = _OPTIONS["prefix"]
  end   
  
  print("Installing NaoRunner to \"" .. prefix .. "\"")
  
  os.copyfile("../dist/libnaointerface.a", prefix .. "/lib/libnaointerface.a");  
  os.copyfile("../dist/libnaoth-simspark.a", prefix .. "/lib/libnaoth-simspark.a");
  os.copyfile("../dist/libnaowebots.a", prefix .. "/lib/libnaowebots.a");
    
end --install

newoption {
  trigger = "prefix",
  value = "DIR",
  description = "installation prefix (e.g. \"/usr/local/\"), if empty defaults to \"../Extern/\""
}

newaction {
 trigger = "install",
 description = "Install NaoRunner",
 execute = install
}

-- description of the solution

solution "NaoRunner"
  kind "StaticLib"
  language "C++"
  targetdir "../dist/"
  
  platforms {"Native"}
  configurations {"Release","Debug"}
  
  if os.is("windows") then
	defines {"WIN32", "NOMINMAX"}
  end
  
      -- debug configuration
  configuration { "Debug" }
    defines { "DEBUG" }
    flags { "Symbols" }
	
  -- store the makefiles the Make/ directory
  location "."
  
  dofile "Interface.lua"
  dofile "Webots.lua"
  dofile "SimSpark.lua"
