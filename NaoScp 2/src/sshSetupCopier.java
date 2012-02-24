/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
import com.jcraft.jsch.*;
import com.jcraft.jsch.ChannelSftp.*;
import java.io.*;

/**
 *
 * @author claas
 */
abstract class sshSetupCopier extends sshCopier
{
  protected String mode;

  sshSetupCopier(naoScpConfig config, String Ip, String sNaoNo, String mode)
  {
    super(config, Ip, "0", sNaoNo);
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