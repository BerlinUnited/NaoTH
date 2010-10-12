-- get the directory of the netbeans.lua script
local scriptDir = path.getdirectory(_SCRIPT)

function createNetbeansProjectFiles()

 
 os.mkdir("nbproject")
 local cwd = os.getcwd()
  
 os.copyfile(scriptDir .. "/nbproject/project.xml", cwd .. "/nbproject/project.xml")
 
end



newaction {
   trigger     = "netbeans",
   description = "Create Netbeans 6.9 project files",
   execute     = createNetbeansProjectFiles
}
