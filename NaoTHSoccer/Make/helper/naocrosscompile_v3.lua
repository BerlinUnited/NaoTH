-- check if NAO_CROSSCOMPILE is set
-- local NAO_CROSSCOMPILE = os.getenv("NAO_CROSSCOMPILE")
if(NAO_CROSSCOMPILE ~= nil) then
  print("INFO: Using \"" ..  NAO_CROSSCOMPILE .. "\" as cross compile root location.")   
else
  print("WARN: Enviroment variale NAO_CROSSCOMPILE was not set. It may be that no cross compilation possible.")
  print("WARN: I will default to /opt/aldebaran/info/crosscompile")
  NAO_CROSSCOMPILE = "/opt/aldebaran/info/crosscompile"
end -- NAO_CROSSCOMPILE ~= nil

local crossDir = NAO_CROSSCOMPILE .. "/cross/"
local stageDir = NAO_CROSSCOMPILE .. "/staging/"

-- the following steps are needed to add the nao cross compiler to the platforms
-- extend the command line option list
table.insert(premake.option.list["platform"].allowed, { "Naov3", "Nao v3 (from NaoTH)" })
-- extend the global variable
premake.platforms.Naov3 = 
{ 
  cfgsuffix       = "Naov3",
  iscrosscompiler = true,
}
-- and one more (we are called after the automatic binding)
table.insert(premake.fields.platforms.allowed, "Naov3")

-- GCC/G++ settings
premake.gcc.platforms.Naov3 =
{
  cppflags = "-MMD",
  -- Winline was here
  flags = "--param inline-unit-growth=400 -m32 -march=geode -m3dnow --sysroot=" .. crossDir .. "/i586-unknown-linux-gnu/sys-root/" ..
   " -I" .. stageDir .. "/usr/include/" .. " -I" .. crossDir .. "/i586-unknown-linux-gnu/sys-root/usr/include/" .. 
   " -I" .. crossDir .. "/i586-unknown-linux-gnu/include/c++/4.3.3/" .. 
   " -I" .. stageDir .. "/usr/include/glib-2.0/" ..
   " -I" .. stageDir .. "/usr/include/gio-unix-2.0/" ..
   " -I" .. stageDir .. "/usr/lib/glib-2.0/include/" ..
   " -L" .. stageDir .. "/usr/lib/" ..
   " -L" .. crossDir .. "/i586-unknown-linux-gnu/sys-root/usr/lib/" 
}

if(_OPTIONS["platform"] == "Naov3") then
  -- reset compiler path to the cross compiler
  premake.gcc.cc     = crossDir .. "/bin/i586-unknown-linux-gnu-gcc"
  premake.gcc.cxx    = crossDir .. "/bin/i586-unknown-linux-gnu-g++"
  premake.gcc.ar     = crossDir .. "/bin/i586-unknown-linux-gnu-ar"
  print("INFO: GCC path was changed for cross compiling")
end

