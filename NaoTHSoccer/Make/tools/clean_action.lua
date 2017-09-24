-- tools/clean_action.lua

premake.modules.lua = {}
local m = premake.modules.lua

local p = premake

newaction {
	trigger = "clean",
	description = "Cleanup the build and dist folders",

	onStart = function()
		print("Cleanup build/dist folders + generated protobuf messages")
	end,

	execute = function()
		if os.isdir("../build") then
			os.rmdir("../build")
		end
		if os.isdir("../dist") then
			os.rmdir("../dist")
		end
		
		-- force protobuf to recompile
		if os.isfile("../Messages/.Representations.proto.lastCompile~") then
			os.remove("../Messages/.Representations.proto.lastCompile~")
		end
		
		if os.isfile("../../Framework/Commons/Messages/.CommonTypes.proto.lastCompile~") then
			os.remove("../../Framework/Commons/Messages/.CommonTypes.proto.lastCompile~")
		end
		
		if os.isfile("../../Framework/Commons/Messages/.Framework-Representations.proto.lastCompile~") then
			os.remove("../../Framework/Commons/Messages/.Framework-Representations.proto.lastCompile~")
		end
		
		if os.isfile("../../Framework/Commons/Messages/.Messages.proto.lastCompile~") then
			os.remove("../../Framework/Commons/Messages/.Messages.proto.lastCompile~")
		end
	end,
}

return m