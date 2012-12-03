--

FRAMEWORK_PATH = path.getabsolute("../../Framework")

EXTERN_PATH = path.getabsolute("../../Extern")

CORE_PATH = {
	path.getabsolute("../Source/Core/Cognition/"), 
	path.getabsolute("../Source/Core/Motion/"),
	path.getabsolute("../Source/Core/")
}

CORE = {"NaoTHSoccer"}


-- needed to cross compile for the Nao
NAO_CROSSCOMPILE = os.getenv("NAO_CROSSCOMPILE")

-- needed to compile the naothlib
AL_DIR = os.getenv("AL_DIR")