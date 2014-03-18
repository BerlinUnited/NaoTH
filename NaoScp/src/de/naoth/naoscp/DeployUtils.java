/*
 * 
 */
package de.naoth.naoscp;

import static de.naoth.naoscp.FileUtils.copyFiles;
import static de.naoth.naoscp.FileUtils.deleteDir;
import static de.naoth.naoscp.FileUtils.writeToFile;
import java.io.File;
import java.io.IOException;
import java.util.Map;

/**
 *
 * @author Thomas Krause <thomas.krause@alumni.hu-berlin.de>
 */
public class DeployUtils
{
  
  public static boolean assembleDeployDir(NaoScp parent, NaoScpConfig cfg, String deployPath)
  {
    // STEP 1: create the deploy directory for the playerNumber
    File currentDeployDir = new File(deployPath);
    
    // delete the target directory if it's existing, 
    // so we have a fresh new directory
    if(currentDeployDir.isDirectory())
    {
      deleteDir(currentDeployDir);
    }

    if(!currentDeployDir.mkdirs())
    {
      parent.actionError("Error: Could not create deploy out directory");
      return false;
    }

    File deployBinPath = new File(deployPath + cfg.binPath());
    deployBinPath.mkdirs();

    //STEP 2: copy the binaries
    if(cfg.copyLib)
    {
      File dst = new File(deployBinPath, "libnaosmal.so");
      File src = new File(cfg.localBinPath(), "libnaosmal.so");
      copyFiles(parent, src, dst);
    }

    if(cfg.copyExe)
    {
      File dst = new File(deployBinPath, "naoth");
      File src = new File(cfg.localBinPath(), "naoth");
      copyFiles(parent, src, dst);
      
      try
      {
        writeToFile(cfg.comment, new File(deployBinPath, "comment.cfg"));
      }
      catch(IOException ioe)
      {
        parent.actionError("I/O Error in prepareDeploy- " + ioe.toString());
      }
    }
    
    if(cfg.copyConfig)
    {
      File dst = new File(deployPath, cfg.configPath());
      File src = new File(cfg.localConfigPath());
      copyFiles(parent, src, dst);
    }
    
    return true;
  }
  
  public static void configureUSBDeployDir(NaoScp parent, NaoScpConfig cfg, Map<String,Integer> bodyIdToPlayerNumber, String deployPath)
  {
    // create special configs
    String myConfigPath = deployPath + cfg.configPath();
    File myGeneralConfigDir = new File(myConfigPath + "/general");
    myGeneralConfigDir.mkdirs();

    writeTeamcommCfg(parent, cfg.sTeamCommPort, new File(myGeneralConfigDir, "teamcomm.cfg"));
    writeScheme(parent, cfg.scheme, new File(myConfigPath, "scheme.cfg"));
    
    for(Map.Entry<String,Integer> e: bodyIdToPlayerNumber.entrySet())
    {
      String bodyIdConfigPath = myConfigPath + "/robots/" + e.getKey();
      File bodyIdConfigFile = new File(bodyIdConfigPath);
      if(!bodyIdConfigFile.isDirectory()) 
      {
        parent.actionError("Directory for the body number " 
          + e.getKey() + " doesn't exist! You might need to enable \"copyConfig\"");
        continue;
      }
      
      writePlayerCfg(parent, new File(bodyIdConfigFile, "player.cfg"), 
                              String.valueOf(e.getValue()), cfg.teamNumber, cfg.teamColor);
    }
  }
  
  public static void configureDeployDir(NaoScp parent, NaoScpConfig cfg, int playerNumber, String deployPath)
  {
    // create special configs
    String myConfigPath = deployPath + cfg.configPath();
    File myGeneralConfigDir = new File(myConfigPath + "/general");
    myGeneralConfigDir.mkdirs();

    writeTeamcommCfg(parent, cfg.sTeamCommPort, new File(myGeneralConfigDir, "teamcomm.cfg"));
    writeScheme(parent, cfg.scheme, new File(myConfigPath, "scheme.cfg"));
    writePlayerCfg(parent, new File(myGeneralConfigDir, "player.cfg"), 
                            String.valueOf(playerNumber), cfg.teamNumber, cfg.teamColor);
  }
  
  /**
   * Staging - prepares "deploy out dir" with the files to be copied to the robots
   * copies files, creates scheme.cfg, modifies other cfgs according to UI settings
   * @return
   */
  public static boolean prepareDeploy(NaoScp parent, NaoScpConfig cfg, int playerNumber, String deployPath)
  {
    boolean result = assembleDeployDir(parent, cfg, deployPath);
    configureDeployDir(parent, cfg, playerNumber, deployPath);
    return result;
  }//end prepareDeploy

  
  public static void writePlayerCfg(
    NaoScp parent, 
    File configFile, 
    String playerNumber,
    String teamNumber,
    String teamColor
    )
  {
    StringBuilder c = new StringBuilder();
    
    c.append("[player]\n");    
    c.append("PlayerNumber=");
    c.append(playerNumber);
    c.append("\n");
    
    c.append("TeamNumber=");
    c.append(teamNumber);
    c.append("\n");
    
    c.append("TeamColor=");
    c.append(teamColor);
    c.append("\n");
    
    try
    {
      writeToFile(c.toString(), configFile);
    }
    catch(IOException ex)
    {
      parent.actionInfo("I/O Error in writePlayerCfg- " + ex.getMessage());
    }
  }
  
  /**
   * Creates scheme.cfg with the scheme
   * @param schemeCfg File
   */
  public static void writeScheme(NaoScp parent, String scheme, File schemeCfg)
  {
    if(scheme != null && !scheme.isEmpty() && !"n/a".equals(scheme))
    {
      try
      {
        writeToFile(scheme, schemeCfg);
      }
      catch(IOException ioe)
      {
        parent.actionInfo("I/O Error in writeScheme- " + ioe.toString());
      }
    }
  }
  
  public static void writeTeamcommCfg(NaoScp parent, String teamCommPort, File configFile)
  {
    StringBuilder c = new StringBuilder();
    
    c.append("[teamcomm]\n");
   
    c.append("port=");
    c.append(teamCommPort);
    c.append("\n");
    
    c.append("interface=wlan0\n");
    
    /*
    c.append("wlan=");
    c.append(lblTeamCommWLAN.getText());
    c.append("\n");
    
    c.append("lan=");
    c.append(lblTeamCommLAN.getText());
    c.append("\n");
    */
    
    try
    {
      writeToFile(c.toString(), configFile);
    }
    catch(IOException ex)
    {
      parent.actionInfo("I/O Error in writeTeamcommCfg- " + ex.getMessage());
    }
  }
}

