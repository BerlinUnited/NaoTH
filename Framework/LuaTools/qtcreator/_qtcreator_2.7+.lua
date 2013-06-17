dofile "qtcreator_2.7+_project.lua"

-- namespace
local qtc = premake.qtcreator27

---- 
-- action for creating new projects from a solution
----
newaction 
{
	trigger = "qtc27",
	shortname = "Qt Creator 2.7+",		
	description = "Generates Qt Creator 2.7+ projects files (wraps GNU make)",

	valid_kinds = { "ConsoleApp", "WindowedApp", "SharedLib", "StaticLib" },
	valid_languages = { "C", "C++" },

	valid_tools     = {
		cc     = { "gcc"},
		dotnet = { },
	},

	onproject = function(prj)
		premake.generate(prj, "%%.creator", qtc.creator)
		premake.generate(prj, "%%.creator.user", qtc.user)
		premake.generate(prj, "%%.files", qtc.files)
		premake.generate(prj, "%%.includes", qtc.includes)
		premake.generate(prj, "%%.config", qtc.defines)
	end,

	oncleanproject  = function(prj)
		premake.clean.file(prj, "%%.creator.user")
		premake.clean.file(prj, "%%.creator")
		premake.clean.file(prj, "%%.files")
		premake.clean.file(prj, "%%.includes")
		premake.clean.file(prj, "%%.config")
	end,
	
	oncleantarget   = function(trg)
	  local files = os.matchfiles(trg .."*")
	  for _, fname in ipairs(files) do
		  os.remove(fname)
	  end
	end
}

----
-- action to only update the .include file
----
newaction 
{
	trigger = "qtc-includes",
	shortname = "Qt Creator \".includes\"",		
	description = "Updates the \".includes\" file of the Qt Creator projects",

	valid_kinds = { "ConsoleApp", "WindowedApp", "SharedLib", "StaticLib" },
	valid_languages = { "C", "C++" },

	valid_tools     = {
		cc     = { "gcc"},
		dotnet = { },
	},

	onproject = function(prj)
		premake.generate(prj, "%%.includes", qtc.includes)
	end,

	oncleanproject  = function(prj)
		premake.clean.file(prj, "%%.includes")
	end
}

----
-- action to only update the .files
----
newaction 
{
	trigger = "qtc-files",
	shortname = "Qt Creator \".files\"",		
	description = "Updates the \".files\" file of the Qt Creator projects",

	valid_kinds = { "ConsoleApp", "WindowedApp", "SharedLib", "StaticLib" },
	valid_languages = { "C", "C++" },

	valid_tools = {
		cc     = { "gcc"},
		dotnet = { },
	},

	onproject = function(prj)
		premake.generate(prj, "%%.files", qtc.files)
	end,

	oncleanproject  = function(prj)
		premake.clean.file(prj, "%%.files")
	end
}
	
