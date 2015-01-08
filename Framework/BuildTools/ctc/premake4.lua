-- this file has to be located within the cross compile directory

-- use the current directory as root for the ctc
local crossDir = path.getabsolute(".")
local linux = "i686-naoth-linux-gnu"
local crossSystemDir = crossDir .. "/" .. linux

-- intel atom
local cpu_flags = "-m32 -march=i686 -msse -msse2 -mssse3"

-- system pathes needed for cross compilation
local cross_flags = 
   " --sysroot=" .. crossSystemDir .. "/sysroot/" ..
   " -isystem"   .. crossSystemDir .. "/sysroot/usr/include/" .. 
   " -L"         .. crossSystemDir .. "/sysroot/usr/lib/"

   
-- DEFINE A NEW PLATFORM --
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
  flags = cpu_flags .. cross_flags
}

if(_OPTIONS["platform"] == "Nao") then
  -- reset compiler path to the cross compiler
  premake.gcc.cc     = crossDir .. "/bin/" .. linux .. "-gcc"
  premake.gcc.cxx    = crossDir .. "/bin/" .. linux .. "-g++"
  premake.gcc.ar     = crossDir .. "/bin/" .. linux .. "-ar"
  print("INFO: GCC path was changed for cross compiling to")
  print("> " .. crossDir)
end

