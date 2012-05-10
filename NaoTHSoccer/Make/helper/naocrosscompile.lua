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
table.insert(premake.option.list["platform"].allowed, { "Nao", "Nao v4 (from NaoTH)" })
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
  -- Winline was here
  flags = "-m32 -march=i686 -msse -msse2 -mssse3".. 
   " --sysroot=" .. crossDir .. "/i686-unknown-linux-gnu/sysroot/" ..
   " -isystem" .. crossDir .. "/i686-unknown-linux-gnu/sysroot/usr/include/" .. 
   " -isystem" .. crossDir .. "/i686-unknown-linux-gnu/include/c++/4.4.7/" .. 
   " -L" .. crossDir .. "/i686-unknown-linux-gnu/sysroot/usr/lib/" ..
   " -I" .. stageDir .. "/usr/include/" .. 
   " -L" .. stageDir .. "/usr/lib/" ..
   " -I" .. stageDir .. "/usr/include/glib-2.0/" ..
   " -I" .. stageDir .. "/usr/include/gio-unix-2.0/" ..
   " -I" .. stageDir .. "/usr/lib/glib-2.0/include/"
}

if(_OPTIONS["platform"] == "Nao") then
  -- reset compiler path to the cross compiler
  premake.gcc.cc     = crossDir .. "/bin/i686-unknown-linux-gnu-gcc"
  premake.gcc.cxx    = crossDir .. "/bin/i686-unknown-linux-gnu-g++"
  premake.gcc.ar     = crossDir .. "/bin/i686-unknown-linux-gnu-ar"
  print("INFO: GCC path was changed for cross compiling")
end

