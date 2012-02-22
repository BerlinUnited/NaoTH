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
  public sshCopier(naoScpConfig config, String Ip, String sNaoNo, String sNaoByte)
  {
    super(config, Ip, sNaoNo, sNaoByte);
  }

  protected Boolean exec() throws JSchException, InterruptedException
  {
    boolean onlyMinimalBackup;
    
    onlyMinimalBackup = config.debugVersion && config.noBackup;
      
    if(onlyMinimalBackup)
    {
      // NO BACKUP!!!
      minimalBackupNao();
    }
    else
    {
      backupNao();
    }
    return writeNao();
  }

  protected Boolean doInBackground()
  {
    hasError = false;
    errors = "";
    
    try
    {
      if(connect())
      {
        hasError = !exec();
      }
      else
      {
        errors = "Couldn't connect with Nao " + config.Ip + " (" + config.sNaoNo + ")";
      }      
      disconnect();
    }
    catch(Exception e)
    {
      errors += e.toString();
    }
    
    return !hasError;
  }

  protected void backupNao()
  {
    String exceptionHelper = "during backup init";
    
//    boolean copyConfig;
//    boolean copyLib;
//    boolean copyExe;
//    boolean copyLogs;
//    boolean backupIsSelected;
    
//    String localDeployRootPath;
//    String localDeployInPath;
//    String remoteLibPath;
//    String remoteBinPath;
//    String remoteConfigPath;
//    String remoteRootPath;
//
//    String selectedBackup = "";
//
//    copyConfig = config.copyConfig;
//    copyLib = config.copyLib;
//    copyExe = config.copyExe;
//    copyLogs = config.copyLogs;
//
//    backupIsSelected = parent.jBackupBox.getSelectedIndex() != 0;

//    localDeployRootPath = parent.localDeployRootPath();
//    localDeployInPath = parent.localDeployInPath(sNaoNo, sNaoByte);
//    remoteLibPath = parent.remoteRootPath(sNaoNo) + parent.libnaoPath() + "/";
//    remoteBinPath = parent.remoteRootPath(sNaoNo) + parent.binPath() + "/";
//    remoteConfigPath = parent.remoteRootPath(sNaoNo) + parent.configPath();
//    remoteRootPath = parent.remoteRootPath(sNaoNo);

    // copy a backup
    
    setInfo("Starting Backup Session for Nao " + config.sNaoNo);
    if(config.backupIsSelected)
    {
      if(config.copyLib)
      {
        File libFile = new File(config.localDeployRootPath() + "/in/" + config.selectedBackup + "/libnaoth.so");
        if(!libFile.exists() || !libFile.isFile())
        {
          setInfo("selected backup contains no libnaoth.so file");
          config.copyLib = false;
        }
      }

      if(config.copyExe)
      {
        File exeDir = new File(config.localDeployRootPath() + "/in/" + config.selectedBackup + "/naoth");
        if(!exeDir.exists() || !exeDir.isFile())
        {
          setInfo("selected backup contains no libnaoth.so file");
          config.copyExe = false;
        }
      }

      if(config.copyConfig)
      {
        File configDir = new File(config.localDeployRootPath() + "/in/" + config.selectedBackup + "/Config");
        if(!configDir.exists() || !configDir.isDirectory())
        {
          setInfo("selected backup contains no Config directory");
          config.copyConfig = false;
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
          if(config.copyLib || config.copyExe || config.copyConfig)
          {
            exceptionHelper = "mkdir local " + config.localDeployInPath();
            File backup = new File(config.localDeployInPath());
            backup.mkdirs();
          }

          if(config.copyLib)
          {
            setInfo("get libnaoth.so");

            exceptionHelper = "get from robot " + config.remoteLibPath() + "libnaoth.so to "  + config.localDeployInPath() + "/libnaoth.so";
            c.get
            (
              config.remoteLibPath() + "libnaoth.so",
              new FileOutputStream(config.localDeployInPath() + "/libnaoth.so")
  //                  ,
  //            new progressMonitor(parent.progressBar)
            );
          }
        }
        catch(Exception e)
        {
          setInfo("Exception in backupNao (" + config.sNaoNo + "): " + e.toString() + "(" + exceptionHelper + ")");
        }

        try
        {
          if(config.copyLib)
          {
            setInfo("get comment.cfg");
            c.get
            (
              config.remoteLibPath() + "comment.cfg",
              new FileOutputStream(config.localDeployInPath() + "/comment.cfg")
  //                  ,
  //            new progressMonitor(parent.progressBar)
            );
          }
        }
        catch(Exception e)
        {
          setInfo("Exception in backupNao (" + config.sNaoNo + "): " + e.toString() + "(" + exceptionHelper + ")");
        }

        try
        {
          if(config.copyExe)
          {
            setInfo("get naoth (exe)");
            exceptionHelper = "get from robot " + config.remoteLibPath() + "naoth "  + config.localDeployInPath() + "/naoth";
            c.get
            (
              config.remoteBinPath() + "naoth",
              new FileOutputStream(config.localDeployInPath() + "/naoth")
  //                  ,
  //            new progressMonitor(parent.progressBar)
            );

          }
        }
        catch(Exception e)
        {
          setInfo("Exception in backupNao (" + config.sNaoNo + "): " + e.toString() + "(" + exceptionHelper + ")");
        }

        try
        {
          if(config.copyConfig)
          {
            exceptionHelper = "get recursive from robot " + config.remoteConfigPath() + " to " + config.localDeployInPath() + "/Config";
            recursiveSftpGet(config.localDeployInPath() + "/Config", config.remoteConfigPath());
          }
        }
        catch(Exception e)
        {
          setInfo("Exception in backupNao (" + String.valueOf(config.sNaoNo) + "): " + e.toString() + "(" + exceptionHelper + ")");
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
          setInfo("Exception in backupNao (" + String.valueOf(config.sNaoNo) + "): " + e.toString() + "(" + exceptionHelper + ")");
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
//    String localDeployInPath;
//    String remoteConfigPath;
//
//    localDeployInPath = parent.localDeployInPath(sNaoNo, sNaoByte);
//    remoteConfigPath = parent.remoteRootPath(sNaoNo) + parent.configPath();
      
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
//    String localLibPath;
//    String remoteLibPath;
//    String localBinPath;
//    String remoteBinPath;
//    String localConfigPath;
//    String remoteConfigPath;
//    String localDeployRootPath;
//
//    boolean copyConfig;
//    boolean copyLib;
//    boolean copyExe;
//
//    boolean backupIsSelected;
//    String selectedBackup = "";
//    Object boxSelected = "";

//    localLibPath = parent.localDeployOutPath(sNaoNo) + parent.libnaoPath() + "/";
//    remoteLibPath = parent.remoteRootPath(sNaoNo) + parent.libnaoPath() + "/";
//    localBinPath = parent.localDeployOutPath(sNaoNo) + parent.binPath() + "/";
//    remoteBinPath = parent.remoteRootPath(sNaoNo) + parent.binPath() + "/";
//    localConfigPath = parent.localDeployOutPath(sNaoNo) + parent.configPath();
//    remoteConfigPath = parent.remoteRootPath(sNaoNo) + parent.configPath();
//    localDeployRootPath = parent.localDeployRootPath();
//
//    copyConfig = parent.copyConfig;
//    copyLib = parent.copyLib;
//    copyExe = parent.copyExe;

//    backupIsSelected = !parent.jBackupBox.getSelectedItem().equals(parent.jBackupBox.getItemAt(0));

    setInfo("initialization writing part");
    try
    {        
      if(openChannel("sftp"))
      {
        channel.connect();
        ChannelSftp c = (ChannelSftp) channel;

        String localLibPath = config.localLibPath();
        String localBinPath = config.localBinPath();
        String localConfigPath = config.localConfigPath();

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
              setInfo("selected backup contains no libnaoth.so file " + config.localLibPath());
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
              setInfo("selected backup contains no libnaoth.so file " + config.localLibPath());
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
              setInfo("selected backup contains no Config directory " + config.localConfigPath());
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
          recursiveSftpPut(new File(localLibPath + "libnaoth.so"), config.remoteLibPath() + "libnaoth.so");
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
          c.chmod(504, config.remoteBinPath() + "naoth");
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
