-- install stuff

function install()

  local prefix = "../../Extern"
  if(_OPTIONS["prefix"]) then
    prefix = _OPTIONS["prefix"]
  end   
  
  print("Installing NaoTH-Tools to \"" .. prefix .. "\"")

  -- general target directory structure
  print("Creating \"include\" and \"lib\" directories")
  os.mkdir(prefix .. "/include")
  os.mkdir(prefix .. "/lib")
  
  -- libs
  print("Copying the static library files")
  if os.is("windows") then
    os.copyfile("../dist/naothtools.lib", prefix .. "/lib/naothtools.lib");
  else
	os.copyfile("../dist/libnaothtools.a", prefix .. "/lib/libnaothtools.a");
  end
  
  -- header files - directories
  print("Creating directory structure for header files")
  local headerdirs = os.matchdirs("../Source/**")
  for i,d in ipairs(headerdirs) do
    local noPrefix = string.sub(d,string.len("../Source/")+1)
    local newPath = prefix .. "/include/" .. noPrefix
    
    os.mkdir(newPath)
  end
  
  -- header files - files
  print("Copying the header files")
  local headerfiles = os.matchfiles("../Source/**.h");
  for i, f in ipairs(headerfiles) do
    local noPrefix = string.sub(f,string.len("../Source/")+1)
    os.copyfile(f, prefix .. "/include/" .. noPrefix)  
  end
    
end --install

newoption {
  trigger = "prefix",
  value = "DIR",
  description = "installation prefix (e.g. \"/usr/local/\"), if empty defaults to \"../Extern/\""
}

newaction {
 trigger = "install",
 description = "Install NaoTH-Tools",
 execute = install
}

