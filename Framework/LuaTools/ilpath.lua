--
function create_path()
	local PathTable = { ["includedirs"]={}, ["libdirs"]={} }

	function PathTable:add_includedir(s)
		table.insert(self.includedirs, s)
	end

	function PathTable:add_libdir(s)
		table.insert(self.libdirs, s)
	end

	-- for debug
	function PathTable:print()
		print("INFO: list includedirs")
		for i,v in pairs(self.includedirs) do
		  print ( "> " .. v )
		end
		print("INFO: list libdirs")
		for i,v in pairs(self.libdirs) do
		  print ( "> " .. v )
		end
	end

	return PathTable
end