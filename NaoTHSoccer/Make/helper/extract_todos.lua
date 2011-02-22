function extract_todos(file)  
  
  local result = {}
  local lineno = 0
  
  local todoPattern = "/[*/]%s*[tT][oO][dD][oO]%s*[:,]?%s*([^*/]*)";
  local fixmePattern = "/[*/]%s*[fF][iI][xX][mM][eE]%s*[:,]?%s*([^*/]*)";
  local hackPattern = "/[*/]%s*[hH][aA][cC][kK]%s*[:,]?%s*([^*/]*)";
  
  io.input(file)
  while true do
    local line = io.read("*line")
    lineno = lineno +1
    if(line == nil) then
      break
    else
      
      local found = false
      local i = #result+1
      -- find lines with TODO comments
      local s,e,m = string.find(line,todoPattern)
      if m ~= nil then        
        result[i] = {}
        result[i].message = m
        result[i].line = lineno
        result[i].file = file  
        result[i].kind = "TODO"   
        found = true
      end
      -- fixme
      local s,e,m = string.find(line,fixmePattern)
      if m ~= nil then
        result[i] = {}
        result[i].message = m
        result[i].line = lineno
        result[i].file = file
        result[i].kind = "FIXME"  
        found = true
      end
      -- hack
      local s,e,m = string.find(line,hackPattern)
      if m ~= nil then
        result[i] = {}
        result[i].message = m
        result[i].line = lineno
        result[i].file = file
        result[i].kind = "HACK"  
        found = true
      end
      
      if found then
        -- read next line with the source
        line = io.read("*line")
        lineno = lineno +1
        if(line ~= nil) then
          result[i].source = line
        end
      end
      
    end
 end
 return result;
  
end --extract_todos

function extract_todos_files(files, result)

  for i,v in ipairs(files) do
   local newresults = extract_todos(v)
   local l = # result
   for k,v in pairs(newresults) do result[l+k] = v end
  end

end -- extract_todos_files


