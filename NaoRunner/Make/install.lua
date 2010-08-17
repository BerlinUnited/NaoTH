-- install stuff

function verbosecopy(from, to)
 print("Copy \"" .. from .. "\" to \"" .. to .. "\"")
 os.copyfile(from, to); 
end -- verbosecopy

function install()

  local prefix = "../../Extern"
  if(_OPTIONS["prefix"]) then
    prefix = _OPTIONS["prefix"]
  end   
  
  print("Installing NaoRunner to \"" .. prefix .. "\"")
  
  -- libs
  verbosecopy("../dist/libnaointerface.a", prefix .. "/lib/libnaointerface.a");  
  verbosecopy("../dist/libnaoth-simspark.a", prefix .. "/lib/libnaoth-simspark.a");
  verbosecopy("../dist/libnaowebots.a", prefix .. "/lib/libnaowebots.a");
  
  -- header files - directories
  local headerdirs = os.matchdirs("../Source/**")
  for i,d in ipairs(headerdirs) do
    local noPrefix = string.sub(d,string.len("../Source/")+1)
    local newPath = prefix .. "/include/" .. noPrefix
    print("Creating directory \"" .. newPath .. "\"")
    os.mkdir(newPath)
  end
  
  -- header files - files
  local headerfiles = os.matchfiles("../Source/**.h");
  for i, f in ipairs(headerfiles) do
    local noPrefix = string.sub(f,string.len("../Source/")+1)
    verbosecopy(f, prefix .. "/include/" .. noPrefix)  
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

