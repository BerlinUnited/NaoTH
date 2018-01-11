--[[
  TODO: Structure the options 
  TODO: add additional hints for our usage
  TODO: maybe remove some options and actions that we dont want to use
]]--

local original_help = premake.showhelp

premake.showhelp = function(prj)
  -- display the basic usage
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

    -- define a table of triggers that should not show up in the help
    hidden_options = {}
    hidden_options["systemscript"] = true
    hidden_options["scripts"] = true 
    hidden_options["os"] = true
    hidden_options["dotnet"] = true
    hidden_options["dc"] = true
    hidden_options["cc"] = true

    hidden_options["fatal"] = true
    hidden_options["insecure"] = true
    hidden_options["interactive"] = true
    
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

        -- only show options if not hidden
        if not hidden_options[string.lower(trigger)] then
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

    -- NaoTH Notes:
    printf("")
    printf("after running premake, the makefile can be run with the following configs:")
    printf("optdebug_nao, optdebug_native, debug_nao, debug_native")
    printf("")
    printf("optdebug disables the gcc debug symbols, but our own debug symbols, e.g. PLOTS, DebugRequests and MODIFY are still working")

end
