project "NaoTHSoccer"
  kind "StaticLib"
  language "C++"
  
  includedirs { 
    "../Source/" 
  }
  externalincludedirs { 
    "../Source/Messages/" 
  }
  
  files {
    "../Source/**.h",
    "../Source/**.cpp",
    "../Source/**.cc"
  }
  
  links { "Commons"	}
  
  targetname "naoth-soccer"