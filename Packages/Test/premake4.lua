solution "TestPackage"
  
  platforms {"Native"}
  configurations {"Debug"}

  targetdir "dist"
  includedirs {"../../Framework/NaoTH-Tools/Source/", 
    "../../NaoTHSoccer/Source/", "../../Framework/DebugCommunication/Source/",
    "../../Extern/include/glib-2.0",
    "../../Extern/lib/glib-2.0/include"}
  libdirs {"../../Extern/lib"}

  project "TestPackage"
    kind "SharedLib"
    language "C++"
    
    includedirs {"src/"}
    files {"src/**.cpp"}
