/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
import java.util.*;

/**
 *
 * @author claas
 */
public class naoScpConfig
{
  boolean noBackup;
  boolean debugVersion;
  boolean restartNaoth;
  boolean reboot;

  boolean copyConfig;
  boolean copyLib;
  boolean copyExe;
  boolean copyLogs;
  boolean copySysLibs;

  boolean backupIsSelected;

  String jDirPathLabel;

  String stagingLibDir;

  String selectedBackup;
  String boxSelected;

  String Ip;
  String sNaoNo;
  String sNaoByte;

  String sshUser;
  String sshPassword;
  String sshRootPassword;

  HashMap backups = new HashMap();

  //Timestamp-Prefix used for the nameing scheme in Deploy/in/
  String backupTimestamp;
  //Only for debugging purposes
  String remotePrefix = "";
  boolean fhIsTesting = false;



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

  public String localLibnaothPath()
  {
    return jDirPathLabel + "/dist/Nao";
  }

  public String libnaoPath(){return homePath() + "/bin";}

  public String localLibPath(){return localLibPath(sNaoNo);}
  public String localLibPath(String sNaoNo){return localDeployOutPath(sNaoNo) + libnaoPath() + "/";}
  public String localBinPath(){return localBinPath(sNaoNo);}
  public String localBinPath(String sNaoNo){return localDeployOutPath(sNaoNo) + binPath() + "/";}
  public String localConfigPath(){return jDirPathLabel + "/Config";}
  public String localDeployRootPath(){return jDirPathLabel + "/Deploy";}
  public String localDeployInPath(){return localDeployInPath(sNaoNo, sNaoByte);}
  public String localDeployInPath(String sNaoNo, String sNaoByte){return localDeployRootPath() + "/in/" + backupTimestamp + "-" + sNaoNo + "-" + sNaoByte;}
  public String localSetupScriptPath(){return jDirPathLabel + "/../Misc_Gentoo/NaoConfigFiles";}
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
