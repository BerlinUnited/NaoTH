project "ExampleSoccerAgent"
	kind "StaticLib"
	language "C++"

	files {"../Source/**.h","../Source/**.cpp"}

	includedirs {
	  "../Source/"
	}

	links {"NaoTH-Commons"}

	targetname "example-soccer-agent"
