package de.naoth.naoscp;

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
import com.jcraft.jsch.*;
import com.jcraft.jsch.ChannelSftp.*;
import java.io.*;
import javax.swing.SwingUtilities;

/**
 *
 * @author claas
 */
abstract class sshCopier extends sshWorker
{   
  public sshCopier(NaoScpConfig config, String sNaoNo, String sNaoByte)
  {
    super(config, sNaoNo, sNaoByte);
  }

  public sshCopier(NaoScpConfig config, String Ip, String sNaoNo, String sNaoByte)
  {
    super(config, Ip, sNaoNo, sNaoByte);
  }

  protected Boolean exec() throws JSchException, InterruptedException
  {
    boolean onlyMinimalBackup = config.debugVersion && config.noBackup && !config.forceBackup;
    boolean copyFiles = config.copyConfig || config.copyExe || config.copyLib || config.copyLogs;
    boolean backupNeeded = config.forceBackup || copyFiles;

    if(onlyMinimalBackup)
    {
      // NO BACKUP!!!
      minimalBackupNao();
    }
    else
    {
      if(backupNeeded)
      {
        backupNao();
      }
    }
    if(copyFiles)
    {
      return writeNao();
    }
    return true;
  }

  protected Boolean doInBackground()
  {
    hasError = false;
    errors = "";
    
    try
    {
      hasError = !testAndConnect();
      if(!hasError)
      {
        hasError = !exec();
      }
      else
      {
        errors = "Couldn't connect with Nao " + config.sNaoNo;
      }      
      disconnect();
    }
    catch(Exception e)
    {
      if(!errors.equals(""))
      {
        errors += "\n";
      }
      errors += "Exception in doInBackground :" + e.toString();
    }
    
    return !hasError;
  }

  protected void backupNao()
  {
    String exceptionHelper = "during backup init";

    boolean copyLib = config.copyLib;
    boolean copyExe = config.copyExe;
    boolean copyConfig = config.copyConfig;

    if(config.forceBackup)
    {
      copyLib = true;
      copyExe = true;
      copyConfig = true;
    }


    setInfo("Starting Backup Session for Nao " + config.sNaoNo);
    if(config.backupIsSelected)
    {
      if(copyLib)
      {
        File libFile = new File(config.localDeployRootPath() + "/in/" + config.selectedBackup + "/libnaoth.so");
        if(!libFile.exists() || !libFile.isFile())
        {
          setInfo("[43mselected backup contains no libnaoth.so file\n[0m");
          config.copyLib = false;
          copyLib = false;
        }
      }

      if(copyExe)
      {
        File exeDir = new File(config.localDeployRootPath() + "/in/" + config.selectedBackup + "/naoth");
        if(!exeDir.exists() || !exeDir.isFile())
        {
          setInfo("[43mselected backup contains no libnaoth.so file\n[0m");
          config.copyExe = false;
          copyExe = false;
        }
      }

      if(copyConfig)
      {
        File configDir = new File(config.localDeployRootPath() + "/in/" + config.selectedBackup + "/Config");
        if(!configDir.exists() || !configDir.isDirectory())
        {
          setInfo("[43mselected backup contains no Config directory\n[0m");
          config.copyConfig = false;
          copyConfig = false;
        }
      }
    }
    
    try
    {
      if(openChannel("sftp"))
      {
        channel.connect();
        ChannelSftp c = (ChannelSftp) channel;
        try
        {
          if(copyLib || copyExe || copyConfig)
          {
            exceptionHelper = "mkdir local " + config.localDeployInPath();
            File backup = new File(config.localDeployInPath());
            if(!backup.isDirectory())
            {
              backup.mkdirs();
            }
          }

          if(copyLib)
          {
            setInfo("get libnaoth.so");

            exceptionHelper = "get from robot " + config.remoteLibPath() + "libnaoth.so to "  + config.localDeployInPath() + "/libnaoth.so";
            c.get
            (
              config.remoteLibPath() + "libnaoth.so",
              new FileOutputStream(config.localDeployInPath() + "/libnaoth.so")
                    ,
              new progressMonitor(config.progressBar)
            );
          }
        }
        catch(Exception e)
        {
          setInfo("[0;31mException in backupNao (" + config.sNaoNo + "): " + e.toString() + "(" + exceptionHelper + ")\n[0m");
        }

        try
        {
          if(copyExe)
          {
            setInfo("get comment.cfg");
            c.get
            (
              config.remoteLibPath() + "comment.cfg",
              new FileOutputStream(config.localDeployInPath() + "/comment.cfg"),
              new progressMonitor(config.progressBar)
            );
          }
        }
        catch(Exception e)
        {
          setInfo("[0;31mException in backupNao (" + config.sNaoNo + "): " + e.toString() + "(" + exceptionHelper + ")\n[0m");
        }

        try
        {
          if(copyExe)
          {
            setInfo("get naoth (exe)");
            exceptionHelper = "get from robot " + config.remoteLibPath() + "naoth "  + config.localDeployInPath() + "/naoth";
            c.get
            (
              config.remoteBinPath() + "naoth",
              new FileOutputStream(config.localDeployInPath() + "/naoth")
                    ,
              new progressMonitor(config.progressBar)
            );

          }
        }
        catch(Exception e)
        {
          setInfo("[0;31mException in backupNao (" + config.sNaoNo + "): " + e.toString() + "(" + exceptionHelper + ")\n[0m");
        }

        try
        {
          if(copyConfig)
          {
            exceptionHelper = "get recursive from robot " + config.remoteConfigPath() + " to " + config.localDeployInPath() + "/Config";
            recursiveSftpGet(config.localDeployInPath() + "/Config", config.remoteConfigPath());
          }
        }
        catch(Exception e)
        {
          setInfo("[0;31mException in backupNao (" + String.valueOf(config.sNaoNo) + "): " + e.toString() + "(" + exceptionHelper + ")\n[0m");
        }

        try
        {
          if(config.copyLogs)
          {
            exceptionHelper = "get recursive from robot " + config.remoteRootPath() + "/tmp/nao.log" + " to " + config.localDeployInPath() + "/nao.log";
            recursiveSftpGet(config.localDeployInPath() + "/nao.log", config.remoteRootPath() + "/tmp/nao.log");
          }
        }
        catch(Exception e)
        {
          setInfo("[0;31mException in backupNao (" + String.valueOf(config.sNaoNo) + "): " + e.toString() + "(" + exceptionHelper + ")\n[0m");
        }
      }
      disconnectChannel();
    }
    catch(Exception e)
    {
      haveError("Exception in backupNao (" + String.valueOf(config.sNaoNo) + "): " + e.toString() + "(" + exceptionHelper + ")");
    }
  }

  protected void minimalBackupNao()
  {
    String exceptionHelper = "during backup init";
    setInfo("Starting minimal Backup Session for Nao " + String.valueOf(config.sNaoNo));
    try
    {
      String myConfigPathIn = config.localDeployInPath() + "/MinimalConfig/";
      File myConfigDirIn = new File(myConfigPathIn);
      myConfigDirIn.mkdirs();

      if(openChannel("sftp"))
      {
        channel.connect();
        ChannelSftp c = (ChannelSftp) channel;
        exceptionHelper = "get recursive minimal backup from robot " + config.remoteConfigPath() + " to "
                            + config.localDeployInPath() + "/Config";
        String regex = "[0-9A-Za-z]{2}_[0-9A-Za-z]{2}_[0-9A-Za-z]{2}_[0-9A-Za-z]{2}_[0-9A-Za-z]{2}_[0-9A-Za-z]{2}";
        regexRecursiveSftpGet(myConfigPathIn, config.remoteConfigPath(), regex);
      }
      disconnectChannel();
    }
    catch(Exception e)
    {
      haveError("Exception in backupNao (" + String.valueOf(config.sNaoNo) + "): " + e.toString() + "(" + exceptionHelper + ")");
    }
  }

  /**
   * copy new files to nao
   *
   * @param session jsch session
   * @param sNaoNo nao number
   */
  protected boolean writeNao()
  {
    setInfo("initialization writing part");
    try
    {        
      if(openChannel("sftp"))
      {
        channel.connect();
        ChannelSftp c = (ChannelSftp) channel;

        String localLibPath = config.localLibPath();
        String localBinPath = config.localBinPath();
        String localConfigPath = config.localConfigDeployOutPath();

        if(config.backupIsSelected)
        {
          if(config.copyLib)
          {
            File libFile = new File(config.localDeployRootPath() + "/in/" + config.selectedBackup + "/libnaoth.so");
            if(libFile.exists() && libFile.isFile())
            {
              localLibPath = config.localDeployRootPath() + "/in/" + config.selectedBackup + "/";
              setInfo("writing libnaoth.so file from Backup " + config.boxSelected.toString() + " to Nao " + config.sNaoNo);
            }
            else
            {
              setInfo("[43mselected backup contains no libnaoth.so file " + config.localLibPath() + "\n[0m");
              config.copyLib = false;
            }
          }

          if(config.copyExe)
          {
            File exeFile = new File(config.localDeployRootPath() + "/in/" + config.selectedBackup + "/naoth");
            if(exeFile.exists() && exeFile.isFile())
            {
              localBinPath = config.localDeployRootPath() + "/in/" + config.selectedBackup + "/";
              setInfo("writing naoth file from Backup " + config.boxSelected.toString() + " to Nao " + config.sNaoNo);
            }
            else
            {
              setInfo("[43mselected backup contains no libnaoth.so file " + config.localLibPath()+ "\n[0m");
              config.copyExe = false;
            }
          }

          if(config.copyConfig)
          {
            File configDir = new File(config.localDeployRootPath() + "/in/" + config.selectedBackup + "/Config/");
            if(configDir.exists() && configDir.isDirectory())
            {
              localConfigPath = configDir.getAbsolutePath();
              setInfo("writing Config directory from Backup " + config.boxSelected.toString() + " to Nao " + config.sNaoNo);
            }
            else
            {
              setInfo("[43mselected backup contains no Config directory " + config.localConfigPath() + "\n[0m");
              config.copyConfig = false;
            }
          }
        }        

        if(config.copyLib)
        {
          try
          {
            c.rm(config.remoteLibPath() + "libnaoth.so");
          }
          catch(SftpException ex)
          {
            // if the file is not there its ok
            if(ex.id != ChannelSftp.SSH_FX_NO_SUCH_FILE)
            {
              throw ex;
            }
          }//end try
          recursiveSftpPut(new File(localLibPath + "libnaoth.so"), config.remoteLibPath() + "libnaoth.so");
        }

        if(config.copyExe)
        {
          try
          {
            c.rm(config.remoteLibPath() + "comment.cfg");
          }
          catch(SftpException ex)
          {
            // if the file is not there its ok
            if(ex.id != ChannelSftp.SSH_FX_NO_SUCH_FILE)
            {
              throw ex;
            }
          }//end try
          recursiveSftpPut(new File(localLibPath + "comment.cfg"), config.remoteLibPath() + "comment.cfg");
        }

        if(config.copyExe)
        {
          try
          {
            c.rm(config.remoteBinPath() + "naoth");
          }
          catch(SftpException ex)
          {
            // if the file is not there its ok
            if(ex.id != ChannelSftp.SSH_FX_NO_SUCH_FILE)
            {
              throw ex;
            }
          }//end try

          recursiveSftpPut(new File(localBinPath + "naoth"), config.remoteBinPath() + "naoth");
          try
          {
            c.chmod(504, config.remoteBinPath() + "naoth");
          }
          catch(SftpException ex)
          {
            // if the file is not there its ok
            if(ex.id != ChannelSftp.SSH_FX_NO_SUCH_FILE)
            {
              throw ex;
            }
          }//end try
        }
        
        if(config.copyConfig)
        {
          File localConfigFiles = new File(localConfigPath);

          rmDirSftp(config.remoteConfigPath() + "/general");
          rmDirSftp(config.remoteConfigPath() + "/robots");
          rmDirSftp(config.remoteConfigPath() + "/scheme");
          rmDirSftp(config.remoteConfigPath() + "/private");

          recursiveSftpPut(localConfigFiles, config.remoteConfigPath());
        }
        else
        {
          try
          {
            c.rm(config.remoteConfigPath() + "/general/player.cfg");
          }
          catch(SftpException ex)
          {
            // if the file is not there its ok
            if(ex.id != ChannelSftp.SSH_FX_NO_SUCH_FILE)
            {
              throw ex;
            }
          }//end try  
          try
          {
            c.rm(config.remoteConfigPath() + "/private/player.cfg");
          }
          catch(SftpException ex)
          {
            // if the file is not there its ok
            if(ex.id != ChannelSftp.SSH_FX_NO_SUCH_FILE)
            {
              throw ex;
            }
          }//end try  
          recursiveSftpPut(new File(localConfigPath + "/general/player.cfg"), config.remoteConfigPath() + "/general/player.cfg");
        }
      }
      disconnectChannel();
    }
    catch(Exception e)
    {
      haveError("Exception in writeNao (" + config.sNaoNo + "): " + e.toString());
      return false;
    }
    return true;   
  }
 

}
