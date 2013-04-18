-- An example project generator; see _example.lua for action description

-- 
-- The project generation function, attached to the action in _example.lua.
-- By now, premake.generate() has created the project file using the name
-- provided in _example.lua, and redirected input to this new file.
--
	local qc = premake.qtcreator
	
  function premake.qtcreator.header()
    _p("<?xml version=\"1.0\" encoding=\"UTF-8\"?>")
    _p("<!DOCTYPE QtCreatorProject>")
    _p("<!-- Written by NaoTH qtc generator script -->")
	  _p("<qtcreator>")
	  
		_p(1, "<data>")
    _p(2, "<variable>ProjectExplorer.Project.ActiveTarget</variable>")
    _p(2, "<value type=\"int\">0</value>")
		_p(1, "</data>")
		_p(1, "<data>")
    _p(1, "<variable>ProjectExplorer.Project.EditorSettings</variable>")
    _p(1, "<valuemap type=\"QVariantMap\">")
    _p(2, "<value type=\"bool\" key=\"EditorConfiguration.AutoIndent\">true</value>")
    _p(2, "<value type=\"bool\" key=\"EditorConfiguration.AutoSpacesForTabs\">false</value>")
    _p(2, "<value type=\"bool\" key=\"EditorConfiguration.CamelCaseNavigation\">true</value>")
    _p(2, "<valuemap type=\"QVariantMap\" key=\"EditorConfiguration.CodeStyle.0\">")
    _p(3, "<value type=\"QByteArray\" key=\"language\">Cpp</value>")
    _p(3, "<valuemap type=\"QVariantMap\" key=\"value\">")
    _p(4, "<value type=\"QString\" key=\"CurrentPreferences\">CppGlobal</value>")
    _p(3, "</valuemap>")
    _p(2, "</valuemap>")
    _p(2, "<valuemap type=\"QVariantMap\" key=\"EditorConfiguration.CodeStyle.1\">")
    _p(3, "<value type=\"QByteArray\" key=\"language\">QmlJS</value>")
    _p(4, "<valuemap type=\"QVariantMap\" key=\"value\">")
    _p(5, "<value type=\"QString\" key=\"CurrentPreferences\">QmlJSGlobal</value>")
    _p(4, "</valuemap>")
    _p(3, "</valuemap>")
    _p(3, "<value type=\"int\" key=\"EditorConfiguration.CodeStyle.Count\">2</value>")
    _p(3, "<value type=\"QByteArray\" key=\"EditorConfiguration.Codec\">System</value>")
    _p(3, "<value type=\"bool\" key=\"EditorConfiguration.ConstrainTooltips\">false</value>")
    _p(3, "<value type=\"int\" key=\"EditorConfiguration.IndentSize\">2</value>")
    _p(3, "<value type=\"bool\" key=\"EditorConfiguration.KeyboardTooltips\">false</value>")
    _p(3, "<value type=\"bool\" key=\"EditorConfiguration.MouseNavigation\">true</value>")
    _p(3, "<value type=\"int\" key=\"EditorConfiguration.PaddingMode\">1</value>")
    _p(3, "<value type=\"bool\" key=\"EditorConfiguration.ScrollWheelZooming\">true</value>")
    _p(3, "<value type=\"int\" key=\"EditorConfiguration.SmartBackspaceBehavior\">0</value>")
    _p(3, "<value type=\"bool\" key=\"EditorConfiguration.SpacesForTabs\">true</value>")
    _p(3, "<value type=\"int\" key=\"EditorConfiguration.TabKeyBehavior\">0</value>")
    _p(3, "<value type=\"int\" key=\"EditorConfiguration.TabSize\">2</value>")
    _p(3, "<value type=\"bool\" key=\"EditorConfiguration.UseGlobal\">true</value>")
    _p(3, "<value type=\"int\" key=\"EditorConfiguration.Utf8BomBehavior\">1</value>")
    _p(3, "<value type=\"bool\" key=\"EditorConfiguration.addFinalNewLine\">true</value>")
    _p(3, "<value type=\"bool\" key=\"EditorConfiguration.cleanIndentation\">true</value>")
    _p(3, "<value type=\"bool\" key=\"EditorConfiguration.cleanWhitespace\">true</value>")
    _p(3, "<value type=\"bool\" key=\"EditorConfiguration.inEntireDocument\">true</value>")
    _p(2, "</valuemap>")
		_p(1, "</data>")
		_p(1, "<data>")
    _p(2, "<variable>ProjectExplorer.Project.PluginSettings</variable>")
    _p(2, "<valuemap type=\"QVariantMap\"/>")
		_p(1, "</data>")

	
  end
  
  function premake.qtcreator.footer()
   
   _p("<data>")
  _p(1,"<variable>ProjectExplorer.Project.Updater.EnvironmentId</variable>")
  _p(1, "<value type=\"QString\">{fe4e41f2-fff5-415d-b64b-7cc87a11753f}</value>")
  _p("</data>")
		
  _p("<data>")
  _p(1, "<variable>ProjectExplorer.Project.Updater.FileVersion</variable>")
  _p(1, "<value type=\"int\">12</value>")
  _p("</data>")
  
  _p("</qtcreator>")
  end
  
  function premake.qtcreator.creator(prj)
    _p("[General]")
  end
  
  function premake.qtcreator.run_configuration(prj, cfg, cfgCounter)
    _p(2, "<valuemap type=\"QVariantMap\" key=\"ProjectExplorer.Target.RunConfiguration.%d\">", cfgCounter)
    _p(3, "<value type=\"bool\" key=\"Analyzer.Project.UseGlobal\">true</value>")
    _p(3, "<valuelist type=\"QVariantList\" key=\"Analyzer.Valgrind.AddedSuppressionFiles\"/>")
    _p(3, "<value type=\"bool\" key=\"Analyzer.Valgrind.Callgrind.CollectBusEvents\">false</value>")
    _p(3, "<value type=\"bool\" key=\"Analyzer.Valgrind.Callgrind.CollectSystime\">false</value>")
    _p(3, "<value type=\"bool\" key=\"Analyzer.Valgrind.Callgrind.EnableBranchSim\">false</value>")
    _p(3, "<value type=\"bool\" key=\"Analyzer.Valgrind.Callgrind.EnableCacheSim\">false</value>")
    _p(3, "<value type=\"bool\" key=\"Analyzer.Valgrind.Callgrind.EnableEventToolTips\">true</value>")
    _p(3, "<value type=\"double\" key=\"Analyzer.Valgrind.Callgrind.MinimumCostRatio\">0.01</value>")
    _p(3, "<value type=\"double\" key=\"Analyzer.Valgrind.Callgrind.VisualisationMinimumCostRatio\">10</value>")
    _p(3, "<value type=\"bool\" key=\"Analyzer.Valgrind.FilterExternalIssues\">true</value>")
    _p(3, "<value type=\"int\" key=\"Analyzer.Valgrind.NumCallers\">25</value>")
    _p(3, "<valuelist type=\"QVariantList\" key=\"Analyzer.Valgrind.RemovedSuppressionFiles\"/>")
    _p(3, "<value type=\"bool\" key=\"Analyzer.Valgrind.TrackOrigins\">true</value>")
    _p(3, "<value type=\"QString\" key=\"Analyzer.Valgrind.ValgrindExecutable\">valgrind</value>")
    _p(3, "<valuelist type=\"QVariantList\" key=\"Analyzer.Valgrind.VisibleErrorKinds\">")
    _p(4, "<value type=\"int\">0</value>")
    _p(4, "<value type=\"int\">1</value>")
    _p(4, "<value type=\"int\">2</value>")
    _p(4, "<value type=\"int\">3</value>")
    _p(4, "<value type=\"int\">4</value>")
    _p(4, "<value type=\"int\">5</value>")
    _p(4, "<value type=\"int\">6</value>")
    _p(4, "<value type=\"int\">7</value>")
    _p(4, "<value type=\"int\">8</value>")
    _p(4, "<value type=\"int\">9</value>")
    _p(4, "<value type=\"int\">10</value>")
    _p(4, "<value type=\"int\">11</value>")
    _p(4, "<value type=\"int\">12</value>")
    _p(4, "<value type=\"int\">13</value>")
    _p(4, "<value type=\"int\">14</value>")
    _p(3, "</valuelist>")
    _p(3, "<value type=\"QString\" key=\"ProjectExplorer.CustomExecutableRunConfiguration.Arguments\"></value>")
    _p(3, "<value type=\"int\" key=\"ProjectExplorer.CustomExecutableRunConfiguration.BaseEnvironmentBase\">2</value>")
    _p(3, "<value type=\"QString\" key=\"ProjectExplorer.CustomExecutableRunConfiguration.Executable\">%s</value>", cfg.buildtarget.fullpath)
    _p(3, "<value type=\"bool\" key=\"ProjectExplorer.CustomExecutableRunConfiguration.UseTerminal\">false</value>")
    _p(3, "<valuelist type=\"QVariantList\" key=\"ProjectExplorer.CustomExecutableRunConfiguration.UserEnvironmentChanges\"/>")
    _p(3, "<value type=\"QString\" key=\"ProjectExplorer.CustomExecutableRunConfiguration.WorkingDirectory\">%%{buildDir}</value>")
    _p(3, "<value type=\"QString\" key=\"ProjectExplorer.ProjectConfiguration.DefaultDisplayName\">Execute %s</value>", cfg.buildtarget.fullpath)
    _p(3, "<value type=\"QString\" key=\"ProjectExplorer.ProjectConfiguration.DisplayName\">%s</value>", cfg.longname)
    _p(3, "<value type=\"QByteArray\" key=\"ProjectExplorer.ProjectConfiguration.Id\">ProjectExplorer.CustomExecutableRunConfiguration</value>")
    _p(3, "<value type=\"uint\" key=\"RunConfiguration.QmlDebugServerPort\">3766</value>")
    _p(3, "<value type=\"bool\" key=\"RunConfiguration.UseCppDebugger\">true</value>")
    _p(3, "<value type=\"bool\" key=\"RunConfiguration.UseMultiProcess\">false</value>")
    _p(3, "<value type=\"bool\" key=\"RunConfiguration.UseQmlDebugger\">false</value>")
    _p(3, "<value type=\"bool\" key=\"RunConfiguration.UseQmlDebuggerAuto\">true</value>")
    _p(2, "</valuemap>")
  end
  
  function premake.qtcreator.build_configuration(prj, cfg, cfgCounter, platform)
    _p(2, "<valuemap type=\"QVariantMap\" key=\"ProjectExplorer.Target.BuildConfiguration.%d\">", cfgCounter)
    _p(3, "<value type=\"QString\" key=\"GenericProjectManager.GenericBuildConfiguration.BuildDirectory\"></value>")    
    -- the build steps for "Make"
    _p(3, "<valuemap type=\"QVariantMap\" key=\"ProjectExplorer.BuildConfiguration.BuildStepList.0\">")
    _p(4, "<valuemap type=\"QVariantMap\" key=\"ProjectExplorer.BuildStepList.Step.0\">")
    _p(5, "<value type=\"bool\" key=\"ProjectExplorer.BuildStep.Enabled\">true</value>")
    if string.endswith(cfg.longname, platform) and platform ~= "" then
      _p(5, "<value type=\"QString\" key=\"ProjectExplorer.ProcessStep.Arguments\">--platform=&quot;".. platform .."&quot; --file=../Make/premake4.lua gmake</value>")
    else
      _p(5, "<value type=\"QString\" key=\"ProjectExplorer.ProcessStep.Arguments\">--file=../Make/premake4.lua gmake</value>")
    end
    _p(5, "<value type=\"QString\" key=\"ProjectExplorer.ProcessStep.Command\">premake4</value>")
    _p(5, "<value type=\"QString\" key=\"ProjectExplorer.ProcessStep.WorkingDirectory\">%%{buildDir}</value>")
    _p(5, "<value type=\"QString\" key=\"ProjectExplorer.ProjectConfiguration.DefaultDisplayName\">Generate Makefiles with premake4</value>")
    _p(5, "<value type=\"QString\" key=\"ProjectExplorer.ProjectConfiguration.DisplayName\">premake4</value>")
    _p(5, "<value type=\"QByteArray\" key=\"ProjectExplorer.ProjectConfiguration.Id\">ProjectExplorer.ProcessStep</value>")
    _p(4, "</valuemap>")
    
    _p(4, "<valuemap type=\"QVariantMap\" key=\"ProjectExplorer.BuildStepList.Step.1\">")
    _p(5, "<valuelist type=\"QVariantList\" key=\"GenericProjectManager.GenericMakeStep.BuildTargets\"/>")
    
    _p(5, "<value type=\"bool\" key=\"GenericProjectManager.GenericMakeStep.Clean\">false</value>")
    _p(5, "<value type=\"QString\" key=\"GenericProjectManager.GenericMakeStep.MakeArguments\">-R config=%s %s</value>",  cfg.shortname, prj.name)
    _p(5, "<value type=\"QString\" key=\"GenericProjectManager.GenericMakeStep.MakeCommand\"></value>")
    _p(5, "<value type=\"bool\" key=\"ProjectExplorer.BuildStep.Enabled\">true</value>")
    _p(5, "<value type=\"QString\" key=\"ProjectExplorer.ProjectConfiguration.DefaultDisplayName\">Make %s</value>", prj.name)
    _p(5, "<value type=\"QString\" key=\"ProjectExplorer.ProjectConfiguration.DisplayName\">%s</value>",  cfg.shortname)
    
    _p(5, "<value type=\"QByteArray\" key=\"ProjectExplorer.ProjectConfiguration.Id\">GenericProjectManager.GenericMakeStep</value>")
    _p(4, "</valuemap>")
    
    _p(4, "<value type=\"int\" key=\"ProjectExplorer.BuildStepList.StepsCount\">2</value>")
    _p(4, "<value type=\"QString\" key=\"ProjectExplorer.ProjectConfiguration.DefaultDisplayName\">Build %s</value>", prj.name)
    _p(4, "<value type=\"QString\" key=\"ProjectExplorer.ProjectConfiguration.DisplayName\"></value>")
    _p(4, "<value type=\"QByteArray\" key=\"ProjectExplorer.ProjectConfiguration.Id\">ProjectExplorer.BuildSteps.Build</value>")  
    _p(3, "</valuemap>")
    
    -- the build steps for "Clean"
    _p(3, "<valuemap type=\"QVariantMap\" key=\"ProjectExplorer.BuildConfiguration.BuildStepList.1\">")
    _p(4, "<valuemap type=\"QVariantMap\" key=\"ProjectExplorer.BuildStepList.Step.0\">")
    _p(5, "<valuelist type=\"QVariantList\" key=\"GenericProjectManager.GenericMakeStep.BuildTargets\">")
    _p(6, "<value type=\"QString\">clean</value>")
    _p(5, "</valuelist>")
    _p(5, "<value type=\"bool\" key=\"GenericProjectManager.GenericMakeStep.Clean\">true</value>")
    _p(5, "<value type=\"QString\" key=\"GenericProjectManager.GenericMakeStep.MakeArguments\">config=%s</value>", cfg.shortname)
    _p(5, "<value type=\"QString\" key=\"GenericProjectManager.GenericMakeStep.MakeCommand\"></value>")
    _p(5, "<value type=\"bool\" key=\"ProjectExplorer.BuildStep.Enabled\">true</value>")
    _p(5, "<value type=\"QString\" key=\"ProjectExplorer.ProjectConfiguration.DefaultDisplayName\">Make</value>")
    _p(5, "<value type=\"QString\" key=\"ProjectExplorer.ProjectConfiguration.DisplayName\"></value>")
    _p(5, "<value type=\"QByteArray\" key=\"ProjectExplorer.ProjectConfiguration.Id\">GenericProjectManager.GenericMakeStep</value>")
    _p(4, "</valuemap>")
    _p(4, "<value type=\"int\" key=\"ProjectExplorer.BuildStepList.StepsCount\">1</value>")
    _p(4, "<value type=\"QString\" key=\"ProjectExplorer.ProjectConfiguration.DefaultDisplayName\">Clean</value>")
    _p(4, "<value type=\"QString\" key=\"ProjectExplorer.ProjectConfiguration.DisplayName\"></value>")
    _p(4, "<value type=\"QByteArray\" key=\"ProjectExplorer.ProjectConfiguration.Id\">ProjectExplorer.BuildSteps.Clean</value>")
    _p(3, "</valuemap>")

     -- general stuff
    _p(3, "<value type=\"int\" key=\"ProjectExplorer.BuildConfiguration.BuildStepListCount\">2</value>")
    -- set PROMPT environment variable in order to tell the ansicolors.lua script not to use colors
    _p(3, "<value type=\"bool\" key=\"ProjectExplorer.BuildConfiguration.ClearSystemEnvironment\">false</value>")
    _p(3, "<valuelist type=\"QVariantList\" key=\"ProjectExplorer.BuildConfiguration.UserEnvironmentChanges\">")
    _p(4, "<value type=\"QString\">PROMPT=TRUE</value>")
    _p(3, "</valuelist>")   
    
    _p(3, "<value type=\"QString\" key=\"ProjectExplorer.ProjectConfiguration.DefaultDisplayName\"></value>")
    _p(3, "<value type=\"QString\" key=\"ProjectExplorer.ProjectConfiguration.DisplayName\">%s</value>", cfg.longname)
    _p(3, "<value type=\"QByteArray\" key=\"ProjectExplorer.ProjectConfiguration.Id\">GenericProjectManager.GenericBuildConfiguration</value>") 

    _p(2, "</valuemap>") -- end configuration valuemap
  end
  
	function premake.qtcreator.user(prj)
		-- If necessary, set an explicit line ending sequence
		-- io.eol = '\r\n'
		
		local cc = premake[_OPTIONS.cc]
		local userPlatform = ""
		if(_OPTIONS["platform"] ~= nil) then
		  userPlatform = _OPTIONS.platform
		end
		local platforms = premake.filterplatforms(prj.solution, cc.platforms, "Native")
		
		qc.header()
		
	  _p("<data>")
    _p(1, "<variable>ProjectExplorer.Project.Target.0</variable>")
    _p(1, "<valuemap type=\"QVariantMap\">")
    _p(2, "<value type=\"QString\" key=\"ProjectExplorer.ProjectConfiguration.DefaultDisplayName\">Desktop</value>")--, prj.name)
    _p(2, "<value type=\"QString\" key=\"ProjectExplorer.ProjectConfiguration.DisplayName\">Desktop</value>")
    _p(2, "<value type=\"QByteArray\" key=\"ProjectExplorer.ProjectConfiguration.Id\">{b699c497-eead-4e23-bb5c-bef17dc27c55}</value>")
    _p(2, "<value type=\"int\" key=\"ProjectExplorer.Target.ActiveBuildConfiguration\">0</value>")
    _p(2, "<value type=\"int\" key=\"ProjectExplorer.Target.ActiveDeployConfiguration\">0</value>")
    _p(2, "<value type=\"int\" key=\"ProjectExplorer.Target.ActiveRunConfiguration\">0</value>")
		local cfgCounter = 0
		for _, platform in ipairs(platforms) do
		  for cfg in premake.eachconfig(prj, platform) do
 		    qc.build_configuration(prj, cfg, cfgCounter, userPlatform)
    	  cfgCounter = cfgCounter + 1
		  end
		end
		_p(2, "<value type=\"int\" key=\"ProjectExplorer.Target.BuildConfigurationCount\">%d</value>", cfgCounter)

		_p(2, "<valuemap type=\"QVariantMap\" key=\"ProjectExplorer.Target.DeployConfiguration.0\">")
		_p(3, "<valuemap type=\"QVariantMap\" key=\"ProjectExplorer.BuildConfiguration.BuildStepList.0\">")
		_p(4, "<value type=\"int\" key=\"ProjectExplorer.BuildStepList.StepsCount\">0</value>")
		_p(4, "<value type=\"QString\" key=\"ProjectExplorer.ProjectConfiguration.DefaultDisplayName\">Deployment</value>")
		_p(4, "<value type=\"QString\" key=\"ProjectExplorer.ProjectConfiguration.DisplayName\"></value>")
		_p(4, "<value type=\"QByteArray\" key=\"ProjectExplorer.ProjectConfiguration.Id\">ProjectExplorer.BuildSteps.Deploy</value>")
		_p(3, "</valuemap>")
		_p(3, "<value type=\"int\" key=\"ProjectExplorer.BuildConfiguration.BuildStepListCount\">1</value>")
		_p(3, "<value type=\"QString\" key=\"ProjectExplorer.ProjectConfiguration.DefaultDisplayName\">Local Deploy</value>")
		_p(3, "<value type=\"QString\" key=\"ProjectExplorer.ProjectConfiguration.DisplayName\"></value>")
		_p(3, "<value type=\"QByteArray\" key=\"ProjectExplorer.ProjectConfiguration.Id\">ProjectExplorer.DefaultDeployConfiguration</value>")
		_p(2, "</valuemap>")

		_p(2, "<value type=\"int\" key=\"ProjectExplorer.Target.DeployConfigurationCount\">1</value>")
	
	  -- run configurations
	cfgCounter = 0
	  for _, platform in ipairs(platforms) do
	    for cfg in premake.eachconfig(prj, platform) do
        if(cfg.kind == "ConsoleApp" or cfg.kind == "WindowedApp") then
          qc.run_configuration(prj, cfg, cfgCounter)
          cfgCounter = cfgCounter + 1
        end -- if kind == *App
	    end
	  end
	  _p(2, "<value type=\"int\" key=\"ProjectExplorer.Target.RunConfigurationCount\">%d</value>", cfgCounter)
		
		_p(1, "</valuemap>")
		_p("</data>")
		
	  _p("<data>")
    _p(1, "<variable>ProjectExplorer.Project.TargetCount</variable>")
    _p(1, "<value type=\"int\">1</value>")
    _p("</data>")
		
		qc.footer()
	end
