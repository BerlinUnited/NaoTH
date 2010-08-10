solution "TestBehavior"
  project "WebotsTest"

  kind "ConsoleApp"
  language "C++"
  
  platforms {"Native"}
  configurations {"Debug", "Release"}
  
  files {"src/**.cpp"}
  
  includedirs {"src/", "../NaoRunner/Interface/include", "../NaoRunner/Webots/WebotsSource"}


