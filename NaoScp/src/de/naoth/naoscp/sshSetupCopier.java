package de.naoth.naoscp;

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
import com.jcraft.jsch.*;
import com.jcraft.jsch.ChannelSftp.*;
import java.io.*;
import javax.swing.JProgressBar;

/**
 *
 * @author claas
 */
abstract class SshSetupCopier extends SshCopier
{
  protected String mode;

  SshSetupCopier(NaoScpConfig config, JProgressBar progressBar, String sNaoNo, String mode)
  {
    super(config, progressBar, "0", sNaoNo);
    this.mode = mode;
  }

  SshSetupCopier(NaoScpConfig config, JProgressBar progressBar, String Ip, String sNaoNo, String mode)
  {
    super(config, progressBar, Ip, "0", sNaoNo);
    this.mode = mode;
  }

  @Override
  protected Boolean exec() throws JSchException, InterruptedException
  {
    boolean result = true;
    result = result && writeNaoSetupFiles(session, config.sNaoNo);
    if(result && mode.equals("full"))
    {
      result = result && super.exec();
    }
    return result;
  }

  @Override
  protected Boolean doInBackground()
  {
    errors = "";
    hasError = false;
    try
    {
      hasError = !testAndConnect();
      if(!hasError)
      {
        hasError = !exec();
      }
      else
      {
        errors = "No connection with Nao " + config.sNaoNo;
      }      
      disconnect();
    }
    catch(Exception e)
    {
      if(!errors.equals(""))
      {
        errors += "\n";
      }
      errors += "Exception in doInBackground: " + e.toString();
    }    
    return !hasError;
  }

  /**
   * copy new files to nao
   *
   * @param session jsch session
   * @param sNaoNo nao number
   */
  protected boolean writeNaoSetupFiles(Session session, String sNaoNo)
  {

    try
    {
      if(openChannel("sftp"))
      {
        channel.connect();
        setInfo("initialization writing setup files part");
        
        File localSetupScriptFiles = new File(config.localDeployOutPath(sNaoNo) + config.setupScriptPath());

        rmDirSftp(config.remoteSetupScriptPath(sNaoNo));

        recursiveSftpPut(localSetupScriptFiles, config.remoteSetupScriptPath());

        if(config.copySysLibs && config.stagingLibDir != null)
        {
          File localSysLibsFiles = new File(config.stagingLibDir);

          String remoteSysLibsDst = config.homePath() + "/lib";

          recursiveSftpPut(localSysLibsFiles, remoteSysLibsDst);
        }
      }
      disconnectChannel();
    }
    catch(Exception e)
    {
      haveError("Exception in writeNaoSetupFiles (" + sNaoNo + "): " + e.toString());
      return false;
    }

    return true;
  }

}