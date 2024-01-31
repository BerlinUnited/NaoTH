--------------------------------------------------------------
function printPath(prefix, path)
	local msg = tostring(prefix) .. tostring(path)
	if path == nil then
		print ( "WARNING: path not set:", msg )
	elseif os.isdir(path) then
		print ( msg )
	else
		print ( "WARNING: path doesn't exist:", msg )
	end
end


-- for debug
function checkPaths(paths)
	for _,v in pairs(paths) do
		printPath("> ",v)
	end
end

-- this function does nothing by default and can be redefined in projectconfig.user.lua
function set_user_defined_paths()
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
-- in a default configuration the directory NAO_CTC has the following layout
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
-- used to compile the NaoSMAL and the WhistleDetector
if AL_DIR == nil then
	AL_DIR = os.getenv("AL_DIR")
end

--------------------------------------------------------------
assert(FRAMEWORK_PATH ~= nil and os.isdir(FRAMEWORK_PATH), 
	"a valid FRAMEWORK_PATH is needed for compilation.")
  
-- make output colorful if possible
dofile (FRAMEWORK_PATH .. "/BuildTools/ansicolors.lua")

print("INFO: list raw path configuration")
printPath("  FRAMEWORK_PATH = ", tostring(FRAMEWORK_PATH))
printPath("  EXTERN_PATH_NATIVE = ", tostring(EXTERN_PATH_NATIVE))
printPath("  NAO_CTC = ", tostring(NAO_CTC))
printPath("  EXTERN_PATH_NAO = ", tostring(EXTERN_PATH_NAO))
printPath("  COMPILER_PATH_NAO = ", tostring(COMPILER_PATH_NAO))
printPath("  AL_DIR = ", tostring(AL_DIR))
--------------------------------------------------------------

-- define paths depending on the platform
if PLATFORM == "Nao" then
  assert(EXTERN_PATH_NAO ~= nil, "EXTERN_PATH_NAO is needed to be able to compile for nao.")
	EXTERN_PATH = path.getabsolute(EXTERN_PATH_NAO)
  --[[
	if AL_DIR ~= nil then
		sysincludedirs {AL_DIR .. "/include"}
		syslibdirs {AL_DIR .. "/lib"}
	end
  ]]--
else
	assert(EXTERN_PATH_NATIVE ~= nil, "EXTERN_PATH_NATIVE is need to be able to compile.")
	EXTERN_PATH = path.getabsolute(EXTERN_PATH_NATIVE)
end


