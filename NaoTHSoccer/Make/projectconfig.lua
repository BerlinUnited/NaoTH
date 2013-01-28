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
  PlatformPath.Native:add_includedir(WEBOTS_HOME .. "/include/controller/c/")
  PlatformPath.Native:add_libdir(WEBOTS_HOME .. "/lib")
end

if AL_DIR ~= nil then
  PlatformPath.Nao:add_includedir(AL_DIR .. "/include")
  PlatformPath.Nao:add_libdir(AL_DIR .. "/lib")
end
--PlatformPath.Native:print()
--PlatformPath.Nao:print()



-- define pathes depending on the platform
if _OPTIONS["platform"] == "Nao" then
  EXTERN_PATH = EXTERN_PATH_NAO
  PATH = PlatformPath.Nao
else
  EXTERN_PATH = EXTERN_PATH_NATIVE
  PATH = PlatformPath.Native
end

-- add general pathes
-- this mainly reflects the internal structure of the extern directory
PATH:add_includedir(FRAMEWORK_PATH .. "/NaoTH-Commons/Source/")
PATH:add_includedir(EXTERN_PATH .. "/include/")
PATH:add_includedir(EXTERN_PATH .. "/include/glib-2.0/")
PATH:add_includedir(EXTERN_PATH .. "/lib/glib-2.0/include/")
PATH:add_libdir(EXTERN_PATH .. "/lib")



print("INFO: list path configuration")
print("  FRAMEWORK_PATH = " .. tostring(FRAMEWORK_PATH))
print("  EXTERN_PATH = " .. tostring(EXTERN_PATH))
print("  NAO_CTC = " .. tostring(NAO_CTC))
print("  COMPILER_PATH_NAO = " .. tostring(COMPILER_PATH_NAO))
print("  AL_DIR = " .. tostring(AL_DIR))
print("  WEBOTS_HOME = " .. tostring(WEBOTS_HOME))
PATH:print()
print()


