-- check if NAO_CROSSCOMPILE is set
local envVal = os.getenv("NAO_CROSSCOMPILE")
if(envVal ~= nil) then
  
  print("INFO: Using \"" ..  envVal .. "\" as cross compile root location.")   
else
  print("WARN: Enviroment variale NAO_CROSSCOMPILE was not set. No cross compilation possible.")
  print("WARN: I will default to a nonsense-value")
  envVal = "<NAO_CROSSCOMPILE_not_set>"
end -- NAO_CROSSCOMPILE ~= nil

local crossDir = envVal .. "/cross/"
local stageDir = envVal .. "/staging/i486-linux/"

local includeDir = stageDir .. "/usr/include/"
local cpIncludeDir = stageDir .. "/usr/include/c++/"
local gcpIncludeDir = stageDir .. "/usr/include/c++/i486-linux/"
local gIncludeDir = stageDir .. "/usr/include/4.2.2/include/"

-- the following steps are needed to add the nao cross compiler to the platforms
-- extend the command line option list
-- table.insert(premake.option.list["platform"].allowed, { "Nao", "Nao v3 (from NaoTH)" })
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
  flags = "-m32 -march=pentium-mmx -m3dnow --sysroot=" .. stageDir .. " " ..
          "-I" .. includeDir .. " -I" .. gIncludeDir .. " -I" .. cpIncludeDir .. " -I" .. gcpIncludeDir
}

if(_OPTIONS["platform"] == "Nao") then
  -- reset compiler path to the cross compiler
  premake.gcc.cc     = crossDir .. "/bin/i486-linux-gcc"
  premake.gcc.cxx    = crossDir .. "/bin/i486-linux-g++"
  premake.gcc.ar     = crossDir .. "/bin/i486-linux-ar"
  print("INFO: GCC path was changed for cross compiling")
end

