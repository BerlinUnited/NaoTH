project "rc-commons"
  kind "StaticLib"
  language "C++"
  
  -- include core
  if(PROTOBUF_INCLUDE == nil or PROTOBUF_LIB or PROTOC_COMMAND) then
    print("WARNING: no path for protobuf is set!!!")
	PROTOBUF_PATH = "."
  end
  
  targetname "rc-commons"
  
  includedirs { ".", PROTOBUF_INCLUDE }
  
  --links { PROTOBUF_LIB }
  
  files {
    "./**.cpp",
    "./**.h",
	"./**.cc"
  }
  
  prebuildcommands {
      PROTOC_COMMAND
  }
