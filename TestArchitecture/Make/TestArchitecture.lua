project "TestArchitecture"
  kind "ConsoleApp"
  language "C++"
  targetdir "../dist/"
  files{ "../Source/**.cpp", "../Source/**.h"}

  includedirs {
	"../Source/",
	"../../Framework/NaoTH-Tools/Source/",  
  }
  libdirs {"../../Extern/lib/"}
  
  links {"NaoTH-Tools"}
