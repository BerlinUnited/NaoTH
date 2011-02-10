-- install stuff

function install()

  local prefix = "../../../Extern"
  if(_OPTIONS["prefix"]) then
    prefix = _OPTIONS["prefix"]
  end   
  
  print("Installing Platforms to \"" .. prefix .. "\"")

  -- general target directory structure
  print("Creating \"include\" and \"lib\" directories")
  os.mkdir(prefix .. "/include")
  os.mkdir(prefix .. "/lib")
  
  -- libs
  print("Copying the static library files")
  os.copyfile("../dist/libnaoth-simspark.a", prefix .. "/lib/libnaoth-simspark.a");
  os.copyfile("../dist/libnaowebots.a", prefix .. "/lib/libnaowebots.a");
  
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
 description = "Install NaoRunner",
 execute = install
}

