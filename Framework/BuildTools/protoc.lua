
local function checkRecompileNeeded(file, cppOut, javaOut, pythonOut)

  local file_info = os.stat(file)
  local name = path.getbasename(file)
  
  if file_info == nil then
    print ("ERROR: conld not get the stats from file " .. file)
    return false
  end
  
  local cpp_info = os.stat(path.join(cppOut, name..".pb.cc"))
  local hpp_info = os.stat(path.join(cppOut, name..".pb.h"))
  --local java_info = os.stat(path.join(javaOut, name..".java")) -- this is not used because finding the correct java path is not that trivial
  local python_info = os.stat(path.join(pythonOut, name:gsub("-", "_").."_pb2.py"))
  
  if python_info == nil then
    print(path.join(pythonOut, name.."_pb2.py"))
  end

  return (cpp_info == nil    or file_info.mtime > cpp_info.mtime) or
         (hpp_info == nil    or file_info.mtime > cpp_info.mtime) or
         --(java_info == nil   or file_info.mtime > java_info.mtime) or
         (python_info == nil or file_info.mtime > python_info.mtime)
end


local function protocCompileAll(inputFiles, cppOut, javaOut, pythonOut, ipaths)
  -- get the protobuf compiler from the EXTERN_PATH
  compiler = os.ishost("windows") and "protoc.exe" or "protoc"
  
  -- TODO: should we search on additional locations?
  compilerPath = os.pathsearch(compiler, EXTERN_PATH .. "/bin/") or os.pathsearch(compiler, EXTERN_PATH_NATIVE .. "/bin/")
  
  if compilerPath == nil then
    print ("ERROR: could not find protoc compiler executable in \"" .. EXTERN_PATH .. "/bin/" .. "\"")
    return false
  end
  
  local args = ""

  if(pythonOut ~= nil) then
    args = args .. "--python_out=\"" ..  pythonOut .. "\" " -- always mind the space at the end
    os.mkdir(pythonOut) -- create the output directory if needed
  end 
  
  if(cppOut ~= nil) then
    args = args .. "--cpp_out=\"" ..  cppOut .. "\" " -- always mind the space at the end
    os.mkdir(cppOut) -- create the output directory if needed
  end
  
  if(javaOut ~= nil) then
    args = args .. "--java_out=\"" ..  javaOut .. "\" " -- always mind the space at the end
    os.mkdir(javaOut) -- create the output directory if needed
  end
  
  if(ipaths ~= nil) then
    for i=1,#ipaths do
      args = args .. "--proto_path=\"" ..  ipaths[i] .. "\" " -- always mind the space at the end
    end
  end 

  -- add files to compile
  args = args .. table.concat(inputFiles, " ")
  
  local cmd = "\"" .. compilerPath .. "/" .. compiler .. "\" " .. args
  
  if os.ishost("windows") then
    cmd = "\"" .. cmd .. "\""
  end
  
  -- TODO. why is this necessary?
  -- delete the generated cpp files
  print("Try to remove the pb.cc and .pb.h files ")
  for i,file in ipairs(inputFiles) do
    local name = path.getbasename(file)
    ok, err = os.remove(path.join(cppOut, name..".pb.cc"))
    ok, err = os.remove(path.join(cppOut, name..".pb.h"))
  end
  print("Removed all generated message files")
  
  -- generate the message files
  print("INFO: (Protbuf) executing " .. cmd)
  local succ, status, returnCode = os.execute(cmd)

  return returnCode == 0
end

-- a wrapper for an easier access with names parameters
-- e.g., makeprotoc{inputFiles = {""}, ...}
function makeprotoc(arg)
  invokeprotoc(arg.inputFiles, arg.cppOut, arg.javaOut, arg.pythonOut, arg.includeDirs)
end


function invokeprotoc(inputFiles, cppOut, javaOut, pythonOut, includeDirs)
	-- check if protobuf compile is explicitely requested
  local compileAll = (_OPTIONS["protoc"] ~= nil)

  local filesToCompile = {}

  -- collect the files that need to be compiled
  for i = 1, #inputFiles do
    if checkRecompileNeeded(inputFiles[i], cppOut, javaOut, pythonOut) then
      table.insert(filesToCompile, inputFiles[i])
    end
  end

  -- filesToCompile is not empty
  if next(filesToCompile) ~= nil then
    protocCompileAll(filesToCompile, cppOut, javaOut, pythonOut, includeDirs)
  end
  
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

newoption {
  trigger = "protoc-python",
  value = "OUT-DIR",
  description = "Output directory for the Python classes generated by protoc"
}
