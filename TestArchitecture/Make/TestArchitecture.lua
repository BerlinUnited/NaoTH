project "TestArchitecture"
  kind "ConsoleApp"
  language "C++"
  targetdir "../dist/"
  files{ "../Source/**.cpp", "../Source/**.h"}

  includedirs {
	"../Source/",
	"../../NaoTH-Tools/Source/",  
	"../../Extern/include/",
	"../../Extern/include/glib-2.0/",
	"../../Extern/lib/glib-2.0/include/"}


  libdirs {"../../Extern/lib/"}
  
  links {"NaoTH-Tools"}
