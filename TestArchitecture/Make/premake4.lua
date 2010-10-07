-- description of the solution

solution "TestArchitecture"	  
  platforms {"Native"}
  configurations {"Debug","Release"}
  
  if os.is("windows") then
	defines {"WIN32", "NOMINMAX"}
  end
  
   -- debug configuration
  configuration { "Debug" }
	defines { "DEBUG" }
	flags { "Symbols" }
	
  -- store the makefiles the Make/ directory
  location "."
  
  project "TestArchitecture"
	  kind "ConsoleApp"
	  language "C++"
	  targetdir "../dist/"
	  files{ "../Source/**.cpp", "../Source/**.h"}

	  includedirs {
		"../Source/",  
		"../../Extern/include/",
		"../../Extern/include/glib-2.0/","../../Extern/lib/glib-2.0/include/"}


	  libdirs {"../../Extern/lib/"}
	  
	  links {"naothtools"}
