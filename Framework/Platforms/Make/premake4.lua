dofile "../../../Utils/tools.lua"

newaction {
  trigger = "webots_copy",
  description = "copy webots files",
  execute = function()
    
    local playerRedDir = 
      "../nao_robocup/controllers/nao_team_0/"
  
    local playerBlueDir =
      "../nao_robocup/controllers/nao_team_1/"
    local playerRedExe = playerRedDir .. path.toExecutable("nao_team_0")
    local playerBlueExe = playerBlueDir .. path.toExecutable("nao_team_1")
    
    os.mkdir(path.getdirectory(playerBlueExe))
    
    -- copy blue controller (from the red one)
    os.copyfile(playerRedExe, playerBlueExe)
    
    -- copy Config (blue and red)
    os.copydir("../../../NaoTHSoccer/Config/", playerRedDir .. "Config")
    os.copydir("../../../NaoTHSoccer/Config/", playerBlueDir .. "Config")
    
  end
}

