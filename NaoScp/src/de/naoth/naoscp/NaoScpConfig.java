package de.naoth.naoscp;

import java.util.ArrayList;
import java.util.HashMap;

/**
 *
 * @author claas
 */
public class NaoScpConfig
{
  // what to do
  boolean noBackup;
  boolean forceBackup;
  boolean debugVersion;
  boolean restartNaoth;
  boolean restartNaoqi;
  boolean reboot;

  // what to copy
  boolean copyConfig;
  boolean copyLib;
  boolean copyExe;
  boolean copyLogs;
  boolean copySysLibs;

  // infos needed when a backup is played back
  HashMap backups = new HashMap();
  boolean backupIsSelected;
  String selectedBackup;
  String boxSelected;
  
  String naoControllerDirectoryPath;
  String stagingLibDir;

  ArrayList<String> addresses;
  String actIp;
  String sNaoNo;
  String sNaoByte;
  
  // nao parameters
  String sTeamCommPort;
  String scheme;
  String teamNumber;
  String teamColor;
  String comment; // comment for the current deploy

  // ssh info
  String sshUser;
  String sshPassword;
  String sshRootPassword;

  //Timestamp-Prefix used for the nameing scheme in Deploy/in/
  String backupTimestamp;
  
  //Only for debugging purposes
  String remotePrefix;
  boolean fhIsTesting;

  NaoScpConfig()
  {
    addresses = new ArrayList<String>();
    remotePrefix = "";
    fhIsTesting = false;
  }

  NaoScpConfig(NaoScpConfig other)
  {
    this.noBackup = other.noBackup;
    this.forceBackup = other.forceBackup;
    this.debugVersion = other.debugVersion;
    this.restartNaoth = other.restartNaoth;
    this.restartNaoqi = other.restartNaoqi;
    this.reboot = other.reboot;

    this.copyConfig = other.copyConfig;
    this.copyLib = other.copyLib;
    this.copyExe = other.copyExe;
    this.copyLogs = other.copyLogs;
    this.copySysLibs = other.copySysLibs;

    this.backups = new HashMap(other.backups);
    this.backupIsSelected = other.backupIsSelected;
    this.selectedBackup = other.selectedBackup;
    this.boxSelected = other.boxSelected;
    
    this.naoControllerDirectoryPath = other.naoControllerDirectoryPath;
    this.stagingLibDir = other.stagingLibDir;

    this.addresses = new ArrayList<String>(other.addresses);
    this.actIp = other.actIp;
    this.sNaoNo = other.sNaoNo;
    this.sNaoByte = other.sNaoByte;

    this.sTeamCommPort = other.sTeamCommPort;
    this.scheme = other.scheme;
    this.teamNumber = other.teamNumber;
    this.teamColor = other.teamColor;
    this.comment = other.comment;

    this.sshUser = other.sshUser;
    this.sshPassword = other.sshPassword;
    this.sshRootPassword = other.sshRootPassword;
    
    //Timestamp-Prefix used for the nameing scheme in Deploy/in/
    this.backupTimestamp = other.backupTimestamp;
    
    //Only for debugging purposes
    this.remotePrefix = other.remotePrefix;
    this.fhIsTesting = other.fhIsTesting;
  }

  public String setupScriptPath()   { return homePath() + "/naoqi/naothSetup"; }
  public String configPath()        { return homePath() + "/naoqi/Config"; }
  public String setupPath()         { return homePath() + "/naoqi/Config"; }
  //public String naoqiPrefPath()     { return homePath() + "/naoqi/preferences"; }
  public String libNaoSMALPath()    { return homePath() + "/bin"; }
  public String binPath()           { return homePath() + "/bin"; }
  public String homePath()          { return "/home/nao"; }

  
  public String localBinPath() { return naoControllerDirectoryPath + "/dist/Nao"; }
  public String localConfigPath(){return naoControllerDirectoryPath + "/Config";}
  
  public String localDeployRootPath(){return naoControllerDirectoryPath + "/Deploy";}
  public String localDeployInPath(){return localDeployInPath(sNaoNo, sNaoByte);}
  public String localDeployInPath(String sNaoNo, String sNaoByte){return localDeployRootPath() + "/in/" + backupTimestamp + "-" + sNaoNo + "-" + sNaoByte;}
  public String localSetupScriptPath(){return naoControllerDirectoryPath + "/../Utils/NaoConfigFiles";}
  public String localSetupStickPath(){return naoControllerDirectoryPath + "/../Utils/SetupStick";}
  public String localMediaPath(){return naoControllerDirectoryPath + "/Media";}
  
  
  public String localLibPath(){return localLibPath(sNaoNo);}
  public String localLibPath(String sNaoNo){return localDeployOutPath(sNaoNo) + libNaoSMALPath() + "/";}
  public String localDeployBinPath(){return localDeployBinPath(sNaoNo);}
  public String localDeployBinPath(String sNaoNo){return localDeployOutPath(sNaoNo) + binPath() + "/";}
  public String localConfigDeployOutPath(){return localConfigDeployOutPath(sNaoNo);}
  public String localConfigDeployOutPath(String sNaoNo){return localDeployOutPath(sNaoNo) + configPath() + "/";}
  public String localDeployOutPath(){return localDeployOutPath(sNaoNo);}
  public String localDeployOutPath(String sNaoNo){return localDeployRootPath() + "/out/" + sNaoNo;}

  
  public String remoteSetupScriptPath(){return remoteSetupScriptPath(sNaoNo);}
  public String remoteSetupScriptPath(String sNaoNo){return remoteRootPath(sNaoNo) + setupScriptPath();}
  public String remoteLibPath(){return remoteLibPath(sNaoNo);}
  public String remoteLibPath(String sNaoNo){return remoteRootPath(sNaoNo) + libNaoSMALPath() + "/";}
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
  
  // NOTE: experimental - not uset yet
  public static class DeployPathConfig
  {
      // The only root set by default, it can be set to something else
      // for debugging purposes.
      private String remoteRoot = "";
      private String localRoot = null;
      private String deployRoot = null;
      
      public String localRoot() { return localRoot; }
      public String localConfig() { return localRoot() + "/Config"; }
      public String localBin() { return localRoot() + "/dist/Nao"; }
      
      public String remoteRoot() { return remoteRoot; }
      public String remoteHome() { return remoteRoot() + "/home/nao"; }
      public String remoteConfig() { return remoteHome() + "/naoqi/Config"; }
      public String remoteBin() { return remoteHome() + "/bin"; }
      
      public String deployRoot() { return deployRoot; }
      public String deployHome() { return deployRoot() + "/home/nao"; }
      public String deployConfig() { return deployHome() + "/naoqi/Config"; }
      public String deployBin() { return deployHome() + "/bin"; }

      public void setRemoteRoot(String remote_root) { this.remoteRoot = remote_root;}
      public void setLocalRoot(String local_root) { this.localRoot = local_root; }
      public void setDeployRoot(String deploy_root) { this.deployRoot = deploy_root; }
  }
  
}
