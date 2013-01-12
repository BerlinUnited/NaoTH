--
function make_string (var)
  if var == nil then
	return "";
  end
  return var;
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
if EXTERN_PATH_NAO == nil then
  EXTERN_PATH_NAO = NAO_CTC .. "/extern"
end
if COMPILER_PATH_NAO == nil then
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


-- define extern depending on the platform
if _OPTIONS["platform"] == "Nao" then
  EXTERN_PATH = EXTERN_PATH_NAO
else
  EXTERN_PATH = EXTERN_PATH_NATIVE
end

-- add optional paths
if WEBOTS_HOME ~= nil then
  table.insert(EXTERN_INCLUDES.Native, WEBOTS_HOME .. "/include/controller/c/")
  table.insert(EXTERN_LIBDIRS.Native, WEBOTS_HOME .. "/lib")
end

if AL_DIR ~= nil then
  table.insert(EXTERN_INCLUDES.Nao, AL_DIR .. "/include")
  table.insert(EXTERN_LIBDIRS.Nao, AL_DIR .. "/lib")
end


print("-------- path configuration --------")
print("FRAMEWORK_PATH = " .. make_string(FRAMEWORK_PATH))
print("EXTERN_PATH = " .. make_string(EXTERN_PATH))
print("NAO_CTC = " .. make_string(NAO_CTC))
print("COMPILER_PATH_NAO = " .. make_string(COMPILER_PATH_NAO))
print("AL_DIR = " .. make_string(AL_DIR))
print("WEBOTS_HOME = " .. make_string(WEBOTS_HOME))
print("--------")


