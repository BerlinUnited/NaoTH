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
			bla = os.rmdir("../build")
		end
		if os.isdir("../dist") then
			blub = os.rmdir("../dist")
		end
		
		-- force protobuf to recompile
		if os.isfile("../Messages/.Representations.proto.lastCompile~") then
			asdf = os.remove("../Messages/.Representations.proto.lastCompile~")
		end
		--[[
		if bla == nil then
			print("Error: could not delete build folder")
		end
		]]--
		-- TODO throw error if removal failed
		-- TODO delete dist folder as well
	end,
}

return m