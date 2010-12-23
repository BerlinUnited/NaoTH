-- install stuff

function install()

  local prefix = "../"
  if(_OPTIONS["prefix"]) then
    prefix = _OPTIONS["prefix"]
  end   
  
  print("Installing Extern-Source to \"" .. prefix .. "\"")

  -- general target directory structure
  print("Creating \"include\" and \"lib\" directories")
  os.mkdir(prefix .. "/include")
  os.mkdir(prefix .. "/lib")
  
  -- libs
  print("Copying the library files")
	os.copyfile("dist/*", prefix .. "/lib/");
  
  -- header files - directories
  print("Creating directory structure for header files")
  local headerdirs = os.matchdirs("../source/**")
  for i,d in ipairs(headerdirs) do
    local noPrefix = string.sub(d,string.len("../source/")+1)
    local newPath = prefix .. "/include/" .. noPrefix
    
    os.mkdir(newPath)
  end
  
  -- header files - files
  print("Copying the header files")
  local headerfiles = os.matchfiles("../source/**.h");
  for i, f in ipairs(headerfiles) do
    local noPrefix = string.sub(f,string.len("../source/")+1)
    os.copyfile(f, prefix .. "/include/" .. noPrefix)  
  end
    
end --install

newoption {
  trigger = "prefix",
  value = "DIR",
  description = "installation prefix (e.g. \"/usr/local/\"), if empty defaults to \"../\""
}

newaction {
 trigger = "install",
 description = "Install Extern-Source",
 execute = install
}

