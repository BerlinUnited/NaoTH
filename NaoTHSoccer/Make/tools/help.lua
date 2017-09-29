--[[
  TODO: Structure the options 
  TODO: add additional hints for our usage
  TODO: maybe remove some options and actions that we dont want to use
]]--

local original_help = premake.showhelp

premake.showhelp = function(prj)
  -- display the basic usage
    print("Blablablabla")
    printf("Premake %s, a build script generator", _PREMAKE_VERSION)
    printf(_PREMAKE_COPYRIGHT)
    printf("%s %s", _VERSION, _COPYRIGHT)
    printf("")
    printf("Usage: premake5 [options] action [arguments]")
    printf("")

    
    -- filter all options by category.
    local categories = {}
    for _, option in pairs(premake.option.list) do
      local cat = "OPTIONS - General"
      if option.category then
        cat = "OPTIONS - " .. option.category;
      end

      if categories[cat] then
        table.insert(categories[cat], option)
      else
        categories[cat] = {option}
      end
    end

    -- display all options
    for k, options in spairs(categories) do
      printf(k)
      printf("")

      local length = 0
      for _, option in ipairs(options) do
        local trigger = option.trigger
        if (option.value) then trigger = trigger .. "=" .. option.value end
        if (#trigger > length) then length = #trigger end
      end

      for _, option in ipairs(options) do
        local trigger = option.trigger
        local description = option.description
        if (option.value) then trigger = trigger .. "=" .. option.value end
        if (option.allowed) then description = description .. "; one of:" end

        printf(" --%-" .. length .. "s %s", trigger, description)
        if (option.allowed) then
          for _, value in ipairs(option.allowed) do
            printf("     %-" .. length-1 .. "s %s", value[1], value[2])
          end
          printf("")
        end
      end
      printf("")
    end
    
    -- display all actions
    printf("ACTIONS")
    printf("")
    for action in premake.action.each() do
      printf(" %-17s %s", action.trigger, action.description)
    end
    printf("")

    -- see more
    printf("For additional information, see https://github.com/premake/premake-core/wiki")
end
