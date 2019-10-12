function touchfile(file_name)
  local f = io.open(file_name, "r")
  if (f == nil) then
    print ("ERROR: (Protbuf) could not open file \"" .. file_name)
  end
  
  local content = f:read("*all")
  f:close()
  
  local f = io.open(file_name, "w+")
  f:write(content);
  f:close()
end