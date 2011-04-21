solution "TestPackage"
  
  platforms {"Native"}
  configurations {"Debug"}

  targetdir "dist"
  includedirs {"../../Framework/NaoTH-Tools/Source/", 
    "../../NaoTHSoccer/Source/", "../../Framework/DebugCommunication/Source/"}

  project "TestPackage"
    kind "SharedLib"
    language "C++"
    
    includedirs {"src/"}
    files {"src/**.cpp"}
