-- check if NAO_CROSSCOMPILE is set
local envVal = os.getenv("NAO_CROSSCOMPILE")
if(envVal ~= nil) then
  
  print("INFO: Using \"" ..  envVal .. "\" as cross compile root location.")   
else
  print("WARN: Enviroment variale NAO_CROSSCOMPILE was not set. It may be that no cross compilation possible.")
  print("WARN: I will default to /opt/aldebaran/info/crosscompile")
  envVal = "/opt/aldebaran/info/crosscompile"
end -- NAO_CROSSCOMPILE ~= nil

local crossDir = envVal .. "/cross/geode/"
local stageDir = envVal .. "/staging/geode-linux/"

local includeDir = stageDir .. "/usr/include/"
local cpIncludeDir = crossDir .. "/i586-linux/include/c++/"
local gcpIncludeDir = crossDir .. "/i586-linux/include/c++/i586-linux/"
local gIncludeDir = stageDir .. "/i586-linux/include/"

-- the following steps are needed to add the nao cross compiler to the platforms
-- extend the command line option list
table.insert(premake.option.list["platform"].allowed, { "Nao", "Nao v3 (from NaoTH)" })
-- extend the global variable
premake.platforms.Nao = 
{ 
  cfgsuffix       = "Nao",
  iscrosscompiler = true,
}
-- and one more (we are called after the automatic binding)
table.insert(premake.fields.platforms.allowed, "Nao")

-- GCC/G++ settings
premake.gcc.platforms.Nao =
{
  cppflags = "-MMD",
  flags = "-m32 -march=geode -m3dnow --sysroot=" .. stageDir .. " " ..
          "-I" .. includeDir .. " -I" .. gIncludeDir .. " -I" .. cpIncludeDir .. " -I" .. gcpIncludeDir
}

if(_OPTIONS["platform"] == "Nao") then
  -- reset compiler path to the cross compiler
  premake.gcc.cc     = crossDir .. "/bin/i586-linux-gcc"
  premake.gcc.cxx    = crossDir .. "/bin/i586-linux-g++"
  premake.gcc.ar     = crossDir .. "/bin/i586-linux-ar"
  print("INFO: GCC path was changed for cross compiling")
end

