--------------------
function printPath(prefix, path)
	local msg = tostring(prefix) .. tostring(path)
	if path == nil then
		print ( "WARNING: path not set:", msg )
	elseif os.isdir(path) then
		print ( msg )
	else
		print ( "ERROR: path doesn't exist:", msg )
	end
end

function create_path()
	local PathTable = {}
	PathTable.libs = {}
	PathTable.includes = {}
	
	function PathTable:includedirs(s)
		for _,v in pairs(s) do
			table.insert(self.includes, v)
		end
	end

	function PathTable:libdirs(s)
		for _,v in pairs(s) do
			table.insert(self.libs, v)
		end
	end

	-- for debug
	function PathTable:print()
		print("INFO: list includedirs")
		for _,v in pairs(self.includes) do
			printPath("> ",v)
		end
		print("INFO: list libdirs")
		for _,v in pairs(self.libs) do
			printPath("> ",v)
		end
	end

	return PathTable
end

-- those contain additional pathes for includes and libs for both platforms
PlatformPath = {}
PlatformPath.Nao = create_path()
PlatformPath.Native = create_path()


-- load local user settings if available
if os.isfile("projectconfig.user.lua") then
	print("INFO: loading local user path settings")
	dofile "projectconfig.user.lua"
end

-------------- Set default paths if not set by the user ------------
-- path to the framework
if FRAMEWORK_PATH == nil then
  FRAMEWORK_PATH = path.getabsolute("../../Framework")
end

-- external directory for the local system
if EXTERN_PATH_NATIVE == nil then
  EXTERN_PATH_NATIVE = "../../Extern"
end

-- path to the directory containing the nao cross compile tool chain
-- in a default configuration the directory NAO_CTC has the followinf layout
-- NAO_CTC
--  |-- compiler
--  `-- extern
-- needed to cross compile for the Nao
if NAO_CTC == nil then
  NAO_CTC = os.getenv("NAO_CTC")
end
-- external libraries for the Nao robot
if EXTERN_PATH_NAO == nil and NAO_CTC ~= nil then
  EXTERN_PATH_NAO = NAO_CTC .. "/extern"
end
if COMPILER_PATH_NAO == nil and NAO_CTC ~= nil then
  COMPILER_PATH_NAO = NAO_CTC .. "/compiler"
end

-- (optional) path to the aldebarans naoqi directory for Nao
-- used to compile the NaoSMAL
if AL_DIR == nil then
  AL_DIR = os.getenv("AL_DIR")
end
-- (optional) path to the installation of webots, e.g., C:\Program files\Webots
-- needed by webots platform
if WEBOTS_HOME == nil then
  WEBOTS_HOME = os.getenv("WEBOTS_HOME")
end
--------------------------------------------------------------

-- add optional paths
if WEBOTS_HOME ~= nil then
  PlatformPath.Native:includedirs {WEBOTS_HOME .. "/include/controller/c/"}
  PlatformPath.Native:libdirs {WEBOTS_HOME .. "/lib"}
end

if AL_DIR ~= nil then
  PlatformPath.Nao:includedirs {AL_DIR .. "/include"}
  PlatformPath.Nao:libdirs {AL_DIR .. "/lib"}
end
--PlatformPath.Native:print()
--PlatformPath.Nao:print()


-- define pathes depending on the platform
if PLATFORM == "Nao" then
  EXTERN_PATH = EXTERN_PATH_NAO
  PATH = PlatformPath.Nao
else
  EXTERN_PATH = EXTERN_PATH_NATIVE
  PATH = PlatformPath.Native
end

-- add general pathes
-- this mainly reflects the internal structure of the extern directory
PATH:includedirs {
	FRAMEWORK_PATH .. "/NaoTH-Commons/Source",
	EXTERN_PATH .. "/include",
	EXTERN_PATH .. "/include/glib-2.0",
	EXTERN_PATH .. "/lib/glib-2.0/include"
	}
PATH:libdirs { EXTERN_PATH .. "/lib"}


-----------------------
dofile (FRAMEWORK_PATH .. "/LuaTools/ansicolors.lua")

print("INFO: list path configuration")
printPath("  FRAMEWORK_PATH = ", tostring(FRAMEWORK_PATH))
printPath("  EXTERN_PATH = ", tostring(EXTERN_PATH))
printPath("  NAO_CTC = ", tostring(NAO_CTC))
printPath("  COMPILER_PATH_NAO = ", tostring(COMPILER_PATH_NAO))
printPath("  AL_DIR = ", tostring(AL_DIR))
printPath("  WEBOTS_HOME = ", tostring(WEBOTS_HOME))
PATH:print()
print()

