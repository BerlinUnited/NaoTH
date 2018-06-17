function extract_modules(file)  
  
  local result = {}
  local lineno = 0
  local found_begin = false
  
  local moduleBeginPattern = "^%s*BEGIN_DECLARE_MODULE%s*[(]%s*(.*)%s*[)]";
  local moduleEndPattern = "^%s*END_DECLARE_MODULE%s*[(]%s*(.*)%s*[)]";
  local requirePattern = "^%s*REQUIRE%s*[(]%s*(.*)%s*[)]";
  local providePattern = "^%s*PROVIDE%s*[(]%s*(.*)%s*[)]";
  
  
  io.input(file)
  while true do
    local line = io.read("*line")
    lineno = lineno + 1
    if(line == nil) then
      break
    else
	
	  local i = #result+1
	  
	  if not found_begin then
		  -- find the beginning of the module
		  local s,e,m = string.find(line,moduleBeginPattern)
		  if m ~= nil then
			result[i] = {}
			result[i].name = m
			result[i].line = lineno
			result[i].file = file  
			result[i].kind = "MODULE" 
			found_begin = true
		  end
      else
		  -- find the beginning of the module
		  local s,e,m = string.find(line,moduleEndPattern)
		  if m ~= nil then
			break
		  end
		  
		  local s,e,m = string.find(line,providePattern)
		  if m ~= nil then
			result[i] = {}
			result[i].name = m
			result[i].line = lineno
			result[i].file = file  
			result[i].kind = "PROVIDE"   
			found = true
		  end
		  
		  local s,e,m = string.find(line,requirePattern)
		  if m ~= nil then
			result[i] = {}
			result[i].name = m
			result[i].line = lineno
			result[i].file = file  
			result[i].kind = "REQUIRE"   
			found = true
		  end
		  
	  end
            
    end
 end
 return result;
  
end --extract_todos

function extract_todos_files(files, result)

  for i,v in ipairs(files) do
   local newresults = extract_modules(v)
   local l = # result
   for k,v in pairs(newresults) do result[l+k] = v end
  end

end -- extract_todos_files


function get_color(name)
	local general_modules = " HistogramProvider GridProvider HistogramFieldDetector FieldDetector LineClusterProvider NeoLineDetector LineDetector "
	local colorclass_modules = " BallDetector GoalDetector RobotDetector "
	local neovision_modules = " SimpleFieldColorClassifier ScanLineEdgelDetector MaximumRedBallDetector GradientGoalDetector "
	local color_modules = " BaseColorClassifier ColorProvider SimpleGoalColorClassifier SimpleBallColorClassifier "
	
	local modules_list = " LineClusterProvider NeoLineDetector LineDetector "
	-- OpenCVDebug OpenCVImageProvider ImageCorrector BodyContourProvider 
	-- to remove: FieldColorClassifier
	
	local experimental_modules = " FieldColorClassifierFull "
	
	modules_list = neovision_modules .. color_modules
	
  
  local vision14 = " HistogramProvider HistogramOverTimeProvider SimpleFieldColorClassifier ScanLineEdgelDetector FieldDetector LineGraphProvider GoalFeatureDetector NewGoalDetector StableBallDetector "
  
	local show_general_modules = false
	local show_colorclass_modules = false
	local show_neovision_modules = false
	local show_color_modules = false
  local show_vision14 = true
  
	if show_general_modules and string.find(general_modules, (" " .. name .. " ")) ~= nil then
		return "gray"
	elseif show_colorclass_modules and string.find(colorclass_modules, (" " .. name .. " ")) ~= nil then
		return "orange"
	elseif show_neovision_modules and string.find(neovision_modules, (" " .. name .. " ")) ~= nil then
		return "green"
	elseif show_color_modules and string.find(color_modules, (" " .. name .. " ")) ~= nil then
		return "red"
  elseif show_vision14 and string.find(vision14, (" " .. name .. " ")) ~= nil then
		return "lightgray"
	else
		return nil
	end
	
end


-- create a premake4 action
newaction {
  trigger = "modules",
  description = "extracts all modules",
  execute = function()
    
    local result = {};
    extract_todos_files(os.matchfiles("../Source/**.h"), result);
    
    -- todo: detect the build directory automatically
    io.output("../build/modules.txt")
    io.write("digraph G {\n")
	
    local i = 1
    
	
	local provided = {}
	local required = {}
	local module_name = nil
	
	local relations = {}
	local modules = {}
	
    for k,v in pairs(result) do
	
	  if v.kind == "MODULE" then
		local c = get_color(v.name)
		if c == nil then
			module_name = nil
		else
			module_name = v.name
			modules[module_name] = c;
		end
		--io.write("[" .. i .. "] " .. v.kind .. " || " .. v.name .. "\n")
	  else
		--io.write("  " .. v.kind .. " || " .. v.name .. "\n")
		if module_name ~= nil and string.sub (v.name, -3) ~= "Top" then
			if v.kind == "REQUIRE" then
				--io.write(v.name .. " -> " .. module_name .. ";\n")
				required[v.name] = module_name
				relations[i] = {}
				relations[i].r = v.name
				relations[i].t = "R"
				relations[i].m = module_name
			else 
				if v.kind == "PROVIDE" then
					--io.write(module_name .. " -> " .. v.name .. ";\n")
					provided[v.name] = module_name
					relations[i] = {}
					relations[i].r = v.name
					relations[i].t = "P"
					relations[i].m = module_name
				end
			end
		end
	  end
	  
	  i = i + 1
    end
	
	
	for k,v in pairs(modules) do
		io.write(k .. "[shape=box,style=filled,color=black,fillcolor=" .. v .. "];\n")
	end
	
	
	for k,v in pairs(relations) do
		-- if required[v.r] ~= nil and provided[v.r] ~= nil then
    if provided[v.r] ~= nil then
			if v.t == "P" then
				--io.write(v.m .. " -> " .. v.r .. "[color=" .. modules[v.m] .. "];\n")
        io.write(v.m .. " -> " .. v.r .. ";\n")
			else
				io.write(v.r .. " -> " .. v.m .. ";\n")
			end
		end
	end
	
	
	
	io.write("}\n")
    print("found " .. # result .. " issues")
    
  end
}
