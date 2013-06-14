/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.naoscp;

import java.io.BufferedWriter;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileWriter;
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
    
    if(currentDeployDir.isDirectory())
    {
      deleteDir(currentDeployDir);
    }

    if( !currentDeployDir.mkdirs())
    {
      parent.actionInfo("Error: Could not create deploy out directory");
      return false;
    }

    File localLibPath = new File(deployPath + cfg.libnaoPath());
    localLibPath.mkdirs();
    File localBinPath = new File(deployPath + cfg.binPath());
    localBinPath.mkdirs();

    //STEP 2: copy the binaries
    if(cfg.copyLib)
    {
      File localLib = new File( deployPath + 
                                cfg.libnaoPath() + 
                                "/libnaosmal.so");
      if(localLib.exists())
      {
        localLib.delete();
      }
      copyFiles(parent, new File(cfg.localLibnaosmalPath() + "/libnaosmal.so"), localLib);
    }

    if(cfg.copyExe)
    {
      File localExe = new File( deployPath + 
                                cfg.binPath() + "/naoth");
      if(localExe.exists())
      {
        localExe.delete();
      }
      try
      {
        FileOutputStream fos = new FileOutputStream(deployPath + cfg.libnaoPath() + "/comment.cfg");
        DataOutputStream out = new DataOutputStream(fos);
        out.writeBytes(cfg.comment);
        fos.close();
      }
      catch(IOException ioe)
      {
        parent.actionInfo("I/O Error in prepareDeploy- " + ioe.toString());
      }

      copyFiles(parent, new File(cfg.localLibnaosmalPath() + "/naoth"), localExe);
    }
    
    if(cfg.copyConfig)
    {
      String myConfigPath = deployPath + cfg.configPath();
      File myConfigDir = new File(myConfigPath);
      copyFiles(parent, new File(cfg.localConfigPath()), myConfigDir);
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
    writeScheme(parent, cfg.scheme, new File(myConfigPath + "/scheme.cfg"));
    
    for(Map.Entry<String,Integer> e: bodyIdToPlayerNumber.entrySet())
    {
      String bodyIdConfigPath = myConfigPath + "/robots/" + e.getKey();
      File bodyIdConfigFile = new File(bodyIdConfigPath);
      if(!bodyIdConfigFile.isDirectory()) 
      {
        parent.actionInfo("Directory for the body number " + e.getKey() + " doesn't exist!");
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
    writeScheme(parent, cfg.scheme, new File(myConfigPath + "/scheme.cfg"));
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
      BufferedWriter writer = new BufferedWriter(new FileWriter(configFile));
      writer.write(c.toString());
      writer.close();
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
    if(!"n/a".equals(scheme))
    {
      try
      {
        BufferedWriter writer = new BufferedWriter(new FileWriter(schemeCfg));
        writer.write(scheme);
        writer.close();
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
      BufferedWriter writer = new BufferedWriter(new FileWriter(configFile));
      writer.write(c.toString());
      writer.close();
    }
    catch(IOException ex)
    {
      parent.actionInfo("I/O Error in writeTeamcommCfg- " + ex.getMessage());
    }
  }
  
  /**
   * recursively delete local dir
   * @param dir File dir
   */
  public static void deleteDir(File dir)
  {
    if(dir.isDirectory())
    {
      File fileList[] = dir.listFiles();
      for(int index = 0; index < fileList.length; index ++)
      {
        File file = fileList[index];
        deleteDir(file);
      }
    }
    dir.delete();
  }//end deleteDir

  
  /**
   * recursively copy local files, skips .svn
   *
   * @param src File source
   * @param dest File destination
   */
  public static void copyFiles(NaoScp parent, File src, File dest)
  {
    if( ! src.exists() ||  ! src.canRead() || src.getName().equals(".svn") || src.getName().equals(".bzr") || src.getName().equals(".hg") || src.getName().equals(".git"))
    {
      return;
    }
    if(src.isDirectory())
    {
      if( ! dest.exists())
      {
        if( ! dest.mkdirs())
        {
          parent.actionInfo("copyFiles: Could not create direcotry: " + dest.getAbsolutePath() + ".");
          return;
        }
      }
      String list[] = src.list();
      for(int i = 0; i < list.length; i ++)
      {
        File dest1 = new File(dest, list[i]);
        File src1 = new File(src, list[i]);
        copyFiles(parent, src1, dest1);
      }
    }
    else
    {
      try
      {
        byte[] buffer = new byte[4096]; //Buffer 4K at a time (you can change this).
        int bytesRead;

        FileInputStream fin = new FileInputStream(src);
        FileOutputStream fout = new FileOutputStream(dest);

        while((bytesRead = fin.read(buffer)) >= 0)
        {
          fout.write(buffer, 0, bytesRead);
        }
        fin.close();
        fout.close();
      }
      catch(IOException e)
      {
        e.printStackTrace();
        parent.actionInfo("copyFiles: Unable to copy file: " + src.getAbsolutePath() + " to " + dest.
        getAbsolutePath() + ".");
      }
    }
  }//end copyFiles

}

