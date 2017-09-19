-- tools/clean_action.lua

premake.modules.lua = {}
local m = premake.modules.lua

local p = premake

newaction {
	trigger = "clean",
	description = "Cleanup the build and dist folders",

	onStart = function()
		print("Cleanup build and dist folders")
	end,

	execute = function()
		bla = os.rmdir("../build")
		blub = os.rmdir("../dist")
		
		-- force protobuf to recompile
		if isfile("../Messages/.Representations.proto.lastCompile~") then
			print("found protobuf file")
			-- asdf = os.remove("../Messages/.Representations.proto.lastCompile~")
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