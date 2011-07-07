-- check if NAO_CROSSCOMPILE is set
local envVal = os.getenv("NAO_CROSSCOMPILE")
if(envVal ~= nil) then
  
  print("INFO: Using \"" ..  envVal .. "\" as cross compile root location.")   
else
  print("WARN: Enviroment variale NAO_CROSSCOMPILE was not set. It may be that no cross compilation possible.")
  print("WARN: I will default to /opt/aldebaran/info/crosscompile")
  envVal = "/opt/aldebaran/info/crosscompile"
end -- NAO_CROSSCOMPILE ~= nil

local crossDir = envVal .. "/cross/"
local stageDir = envVal .. "/staging/"

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
  flags = "-m32 -march=geode -m3dnow -finline-functions --sysroot=" .. crossDir .. "/i586-unknown-linux-gnu/sys-root/" ..
   " -I" .. stageDir .. "/usr/include/" .. " -I" .. crossDir .. "/i586-unknown-linux-gnu/sys-root/usr/include/" .. 
   " -I" .. crossDir .. "/i586-unknown-linux-gnu/include/c++/4.3.3/" .. 
   " -I" .. stageDir .. "/usr/include/glib-2.0/" ..
   " -I" .. stageDir .. "/usr/include/gio-unix-2.0/" ..
   " -I" .. stageDir .. "/usr/lib/glib-2.0/include/" ..
   " -L" .. stageDir .. "/usr/lib/" ..
   " -L" .. crossDir .. "/i586-unknown-linux-gnu/sys-root/usr/lib/"
}

if(_OPTIONS["platform"] == "Nao") then
  -- reset compiler path to the cross compiler
  premake.gcc.cc     = crossDir .. "/bin/i586-unknown-linux-gnu-gcc"
  premake.gcc.cxx    = crossDir .. "/bin/i586-unknown-linux-gnu-g++"
  premake.gcc.ar     = crossDir .. "/bin/i586-unknown-linux-gnu-ar"
  print("INFO: GCC path was changed for cross compiling")
end

