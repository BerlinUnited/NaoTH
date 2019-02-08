-- NaoTH controller for the logfile based "simulator"
project "USBJoypad"
  kind "ConsoleApp"
  language "C++"
  
  includedirs {
    "../Source/USBJoypad/",
    "../../Source/"
	}
  
  files {
    "../Source/USBJoypad/**.cpp",
    "../Source/USBJoypad/**.h"
	}
  
  links {
    "Commons",
    "udev",
    "pthread"
	}
	
  targetname "usbjoypad"
