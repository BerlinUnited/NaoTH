package de.naoth.naoscp;

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
import java.util.*;
import javax.swing.*;

/**
 *
 * @author claas
 */
public class NaoScpConfig
{
  boolean noBackup;
  boolean forceBackup;
  boolean debugVersion;
  boolean restartNaoth;
  boolean restartNaoqi;
  boolean reboot;

  boolean copyConfig;
  boolean copyLib;
  boolean copyExe;
  boolean copyLogs;
  boolean copySysLibs;

  boolean backupIsSelected;

  String comment; // comment for the current deploy
  String jDirPathLabel;

  String stagingLibDir;

  String selectedBackup;
  String boxSelected;

  ArrayList<String> addresses;
  String actIp;
  String sNaoNo;
  String sNaoByte;
  
  String sTeamCommPort;
  String scheme;
  String teamNumber;
  String teamColor;

  String sshUser;
  String sshPassword;
  String sshRootPassword;

  HashMap backups = new HashMap();

  //Timestamp-Prefix used for the nameing scheme in Deploy/in/
  String backupTimestamp;
  //Only for debugging purposes
  String remotePrefix;
  boolean fhIsTesting;

  JProgressBar progressBar;

  NaoScpConfig()
  {
    addresses = new ArrayList<String>();
    remotePrefix = "";
    fhIsTesting = false;
  }

  NaoScpConfig(NaoScpConfig config)
  {
    noBackup = config.noBackup;
    forceBackup = config.forceBackup;
    debugVersion = config.debugVersion;
    restartNaoth = config.restartNaoth;
    restartNaoqi = config.restartNaoqi;
    reboot = config.reboot;

    copyConfig = config.copyConfig;
    copyLib = config.copyLib;
    copyExe = config.copyExe;
    copyLogs = config.copyLogs;
    copySysLibs = config.copySysLibs;

    backupIsSelected = config.backupIsSelected;

    jDirPathLabel = config.jDirPathLabel;

    stagingLibDir = config.stagingLibDir;

    selectedBackup = config.selectedBackup;
    boxSelected = config.boxSelected;

    addresses = new ArrayList<String>(config.addresses);
    actIp = config.actIp;
    sNaoNo = config.sNaoNo;
    sNaoByte = config.sNaoByte;

    sshUser = config.sshUser;
    sshPassword = config.sshPassword;
    sshRootPassword = config.sshRootPassword;

    backups = new HashMap(config.backups);

    //Timestamp-Prefix used for the nameing scheme in Deploy/in/
    backupTimestamp = config.backupTimestamp;
    //Only for debugging purposes
    remotePrefix = config.remotePrefix;
    fhIsTesting = config.fhIsTesting;

    progressBar = config.progressBar;
    
    comment = config.comment;
  }

  public String setupScriptPath()
  {
    return homePath() + "/naoqi/naothSetup";
  }

  public String configPath()
  {
    return homePath() + "/naoqi/Config";
  }

  public String setupPath()
  {
    return homePath() + "/naoqi/Config";
  }


  public String naoqiPrefPath()
  {
    return homePath() + "/naoqi/preferences";
  }

  public String binPath()
  {
    return homePath() + "/bin";
  }

  public String localLibnaosmalPath()
  {
    return jDirPathLabel + "/dist/Nao";
  }

  public String libnaoPath(){return homePath() + "/bin";}

  public String localLibPath(){return localLibPath(sNaoNo);}
  public String localLibPath(String sNaoNo){return localDeployOutPath(sNaoNo) + libnaoPath() + "/";}
  public String localBinPath(){return localBinPath(sNaoNo);}
  public String localBinPath(String sNaoNo){return localDeployOutPath(sNaoNo) + binPath() + "/";}
  public String localConfigPath(){return jDirPathLabel + "/Config";}
  public String localConfigDeployOutPath(){return localConfigDeployOutPath(sNaoNo);}
  public String localConfigDeployOutPath(String sNaoNo){return localDeployOutPath(sNaoNo) + configPath() + "/";}
  public String localDeployRootPath(){return jDirPathLabel + "/Deploy";}
  public String localDeployInPath(){return localDeployInPath(sNaoNo, sNaoByte);}
  public String localDeployInPath(String sNaoNo, String sNaoByte){return localDeployRootPath() + "/in/" + backupTimestamp + "-" + sNaoNo + "-" + sNaoByte;}
  public String localSetupScriptPath(){return jDirPathLabel + "/../Utils/NaoConfigFiles";}
  public String localSetupStickPath(){return jDirPathLabel + "/../Utils/SetupStick";}
  public String localMediaPath(){return jDirPathLabel + "/Media";}
  public String localDeployOutPath(){return localDeployOutPath(sNaoNo);}
  public String localDeployOutPath(String sNaoNo){return localDeployRootPath() + "/out/" + sNaoNo;}

  public String remoteSetupScriptPath(){return remoteSetupScriptPath(sNaoNo);}
  public String remoteSetupScriptPath(String sNaoNo){return remoteRootPath(sNaoNo) + setupScriptPath();}
  public String remoteLibPath(){return remoteLibPath(sNaoNo);}
  public String remoteLibPath(String sNaoNo){return remoteRootPath(sNaoNo) + libnaoPath() + "/";}
  public String remoteBinPath(){return remoteBinPath(sNaoNo);}
  public String remoteBinPath(String sNaoNo){return remoteRootPath(sNaoNo) + binPath() + "/";}
  public String remoteConfigPath(){return remoteConfigPath(sNaoNo);}
  public String remoteConfigPath(String sNaoNo){return remoteRootPath(sNaoNo) + configPath();}
  public String remoteRootPath(){return remoteRootPath(sNaoNo);}
  public String remoteRootPath(String sNaoNo)
  {
    if(fhIsTesting)
    {
      return remotePrefix + "/" + sNaoNo;
    }
    return remotePrefix;
  }
  public String homePath(){return "/home/nao";}



}
