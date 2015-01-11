-- as found on http://lua-users.org/wiki/SplitJoin
local function split(str, delim, maxNb)
    -- Eliminate bad cases...
    if string.find(str, delim) == nil then
        return { str }
    end
    if maxNb == nil or maxNb < 1 then
        maxNb = 0    -- No limit
    end
    local result = {}
    local pat = "(.-)" .. delim .. "()"
    local nb = 0
    local lastPos
    for part, pos in string.gfind(str, pat) do
        nb = nb + 1
        result[nb] = part
        lastPos = pos
        if nb == maxNb then break end
    end
    -- Handle the last field
    if nb ~= maxNb then
        result[nb + 1] = string.sub(str, lastPos)
    end
    return result
end

local function getShadowFile(file)
  name = path.getname(file)
  dir = path.getdirectory(file)
  return dir .. "/." .. name .. ".lastCompile~"
end

local function checkRecompileNeeded(file)

  file_info = os.stat(file)
  shadow_info = os.stat(getShadowFile(file))
  if(shadow_info == nil or (file_info ~= nil and file_info.mtime > shadow_info.mtime)) then
    print ("INFO: " .. file ..  " was changed")
    return true
  end
  return false
end

local function touchShadow(file, time)
  local shadowFile = getShadowFile(file)
  io.output(shadowFile)
  io.write(time)
  io.close()  
end

local function protocCompile(inputFiles, cppOut, javaOut, ipaths)
  -- get the protobuf compiler from the EXTERN_PATH
  compiler = "protoc"
  if(os.is("windows")) then
    compiler = "protoc.exe"
  end
  -- TODO: should we search on additional locations?
  compilerPath = os.pathsearch(compiler, EXTERN_PATH .. "/bin/")
  if compilerPath == nil then
	compilerPath = os.pathsearch(compiler, EXTERN_PATH_NATIVE .. "/bin/")
  end
  if compilerPath == nil then
    print ("ERROR: could not find protoc compiler executable in \"" .. EXTERN_PATH .. "/bin/" .. "\"")
    return false
  end
  
  local args = ""
  if(cppOut ~= nil) then
    args = args .. "--cpp_out=\"" ..  cppOut .. "\" " -- always mind the space at the end
  end
  if(javaOut ~= nil) then
    args = args .. "--java_out=\"" ..  javaOut .. "\" " -- always mind the space at the end
  end
  if(ipaths ~= nil) then
    for i=1,#ipaths do
      args = args .. "--proto_path=\"" ..  ipaths[i] .. "\" " -- always mind the space at the end
    end
  end 

  args = args .. table.concat(inputFiles, " ")
  local cmd = compilerPath .. "/" .. compiler .. " " .. args
  
  -- HACK: create the output directories if needed
  os.mkdir(cppOut)
  os.mkdir(javaOut)
  
  -- generate the message files
  print("INFO: executing " .. cmd)
  local returnCode = os.execute(cmd)
  
  if _OPTIONS["Wno-conversion"] == nil and returnCode == 0 then
	-- add few lines to suppress the conversion warnings to each of the generated *.cc files
	add_gcc_ignore_pragmas(os.matchfiles(cppOut .. "**.pb.cc"))
	add_gcc_ignore_pragmas(os.matchfiles(cppOut .. "**.pb.h"))
	--
  end
  
  return returnCode == 0
end


function add_gcc_ignore_pragmas(files)
	-- add gcc pragma to suppress the conversion warnings to each of the generated *.cc files
	-- hack for the GCC version < 4.6.x
	-- this is because "#pragma GCC diagnostic push/pop" was introduced in GCC 4.6
	local prefix = "// added by NaoTH \n" ..
				 "#if defined(__GNUC__) && defined(_NAOTH_CHECK_CONVERSION_)\n" ..
				 "#if __GNUC__ > 3 && __GNUC_MINOR__ > 5\n" ..
				 "#pragma GCC diagnostic push\n" ..
				 "#endif\n" ..
				 "#pragma GCC diagnostic ignored \"-Wconversion\"\n" ..
				 "#endif\n\n"
	
	-- enable the warnings at the end
	local suffix = "\n\n// added by NaoTH \n" ..
				 "#if defined(__GNUC__) && defined(_NAOTH_CHECK_CONVERSION_)\n" ..
				 "#if __GNUC__ > 3 && __GNUC_MINOR__ > 5\n" ..
				 "#pragma GCC diagnostic pop\n" ..
				 "#else\n" ..
				 "#pragma GCC diagnostic error \"-Wconversion\"\n" ..
				 "#endif\n" ..
				 "#endif\n\n"
	
	for i,v in ipairs(files) do
		local f = io.open(v, "r")
		if (f == nil) then
			print ("ERROR: could not open file \"" .. v)
		end
		local content = f:read("*all")
		f:close()
		
		local f = io.open(v, "w+")
		f:write(prefix);
		f:write(content);
		f:write(suffix);
		f:close()
	end
end

function invokeprotoc(inputFiles, cppOut, javaOut, ipaths)
    
	-- cack if protobuf compile is explicitely requested
    local compile = (_OPTIONS["protoc"] ~= nil)

	-- iterate over all files to check if any of them was changed
    for i = 1, #inputFiles do
      if(checkRecompileNeeded(inputFiles[i])) then
        compile = true
        break
      end
    end

	
    if(compile) then
      -- execute compile process for each file
      local time = os.time()
      
      -- do the recompilation
     if( protocCompile(inputFiles, cppOut, javaOut, ipaths)) then
       -- if successfull touch the shadow files
       for i = 1, #inputFiles do
          -- touch shadow file in order to remember this build date
          touchShadow(inputFiles[i], time)
      end -- end for each file to compile
     end
    end -- end if compile
end

newoption {
   trigger     = "protoc",
   description = "Force to recompile the protbuf messages"
}

newoption {
  trigger = "protoc-ipath",
  value = "INCLUDE-DIRS",
  description = "Include paths seperated by a \":\""
}

newoption {
  trigger = "protoc-cpp",
  value = "OUT-DIR",
  description = "Output directory for the C++ classes generated by protoc"
}

newoption {
  trigger = "protoc-java",
  value = "OUT-DIR",
  description = "Output directory for the Java classes generated by protoc"
}


-- the action protoc takes the files to compile as argument
-- see also the "protoc-cpp" and "protoc-java" arguments for specifying what kind of file
-- to generate and where to generate them
newaction {
  trigger = "protoc",
  description = "Compile premake4 messages given as argument",
  execute = function ()
    invokeprotoc(_ARGS, _OPTIONS["protoc-cpp"], _OPTIONS["protoc-java"], split(_OPTIONS["protoc-ipath"], ":") )    
  end
}
