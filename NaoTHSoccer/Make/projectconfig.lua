----------------------------------------
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

function table.append(t,s)
	if s ~= nil then for _,v in pairs(s) do if v ~= nil then
			table.insert(t, 1, v)
	end end end
end

--------------------------------------------------------------
PATH = {}
PATH.libs = {}
PATH.includes = {}
	
function PATH:includedirs(s)
	table.append(self.includes,s)
end

function PATH:libdirs(s)
	table.append(self.libs,s)
end

-- for debug
function PATH:print()
	print("INFO: list includedirs")
	for _,v in pairs(self.includes) do
		printPath("> ",v)
	end
	print("INFO: list libdirs")
	for _,v in pairs(self.libs) do
		printPath("> ",v)
	end
end
--------------------------------------------------------------



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
	EXTERN_PATH_NATIVE = os.getenv("EXTERN_PATH_NATIVE")
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
assert(FRAMEWORK_PATH ~= nil and os.isdir(FRAMEWORK_PATH), 
	"a valid FRAMEWORK_PATH is needed for compilation.")
dofile (FRAMEWORK_PATH .. "/BuildTools/ansicolors.lua")

print("INFO: list raw path configuration")
printPath("  FRAMEWORK_PATH = ", tostring(FRAMEWORK_PATH))
printPath("  EXTERN_PATH_NATIVE = ", tostring(EXTERN_PATH_NATIVE))
printPath("  NAO_CTC = ", tostring(NAO_CTC))
printPath("  EXTERN_PATH_NAO = ", tostring(EXTERN_PATH_NAO))
printPath("  COMPILER_PATH_NAO = ", tostring(COMPILER_PATH_NAO))
printPath("  AL_DIR = ", tostring(AL_DIR))
printPath("  WEBOTS_HOME = ", tostring(WEBOTS_HOME))
--------------------------------------------------------------


-- define pathes depending on the platform
if PLATFORM == "Nao" then
  assert(EXTERN_PATH_NAO ~= nil, "EXTERN_PATH_NAO is needed to be able to compile for nao.")
	EXTERN_PATH = path.getabsolute(EXTERN_PATH_NAO)
	if AL_DIR ~= nil then
		PATH:includedirs {AL_DIR .. "/include"}
		PATH:libdirs {AL_DIR .. "/lib"}
	end
else
	assert(EXTERN_PATH_NATIVE ~= nil, "EXTERN_PATH_NATIVE is need to be able to compile.")
	EXTERN_PATH = path.getabsolute(EXTERN_PATH_NATIVE)
	if WEBOTS_HOME ~= nil then
		PATH:includedirs {WEBOTS_HOME .. "/include/controller/c"}
		PATH:libdirs {WEBOTS_HOME .. "/lib"}
	end
end

-- add general pathes
-- this mainly reflects the internal structure of the extern directory
PATH:includedirs {
	FRAMEWORK_PATH .. "/Commons/Source",
	FRAMEWORK_PATH .. "/Commons/Source/Messages",
	EXTERN_PATH .. "/include",
	EXTERN_PATH .. "/include/glib-2.0",
	EXTERN_PATH .. "/lib/glib-2.0/include"
	}
PATH:libdirs { EXTERN_PATH .. "/lib"}


--------------------------------------------------------------
PATH:print()
print()

