/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
import com.jcraft.jsch.*;
import com.jcraft.jsch.ChannelSftp.*;
import javax.swing.*;
import java.io.*;
import java.awt.event.*;

/**
 *
 * @author claas
 */
abstract class sshScriptRunner extends sshWorker
{
  private String scriptName;
//  private String hostname;
  
  JTextField cIn = new JTextField();
  final TextFieldStreamer ts = new TextFieldStreamer(cIn);

  private OutputStream out;
  private String lastOut;

  public sshScriptRunner(NaoScpConfig config, String sNaoNo, String sNaoByte, String scriptName, boolean reboot)
  {
    super(config, sNaoNo, sNaoByte);
    init(scriptName);
    config.reboot = reboot;
  }

  public sshScriptRunner(NaoScpConfig config, String Ip, String sNaoNo, String sNaoByte, String scriptName, boolean reboot)
  {
    super(config, Ip, sNaoNo, sNaoByte);
    init(scriptName);
    config.reboot = reboot;
  }

  private void init(String script)
  {
    scriptName = script;
    cIn.addActionListener(ts);
    out = new OutputStream() 
          {  
            @Override  
            public void write(int b) throws IOException 
            {  
              updateLog(String.valueOf((char) b));  
            }  

            @Override  
            public void write(byte[] b, int off, int len) throws IOException 
            {  
              updateLog(new String(b, off, len));  
            }  

            @Override  
            public void write(byte[] b) throws IOException 
            {  
              write(b, 0, b.length);  
            }  
          };
  }

  private void updateLog(final String text) 
  {  
//    System.out.append(text);
    lastOut += text;
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
        if(scriptName.equalsIgnoreCase("restartNaoTH"))
        {
          hasError = !restartNaoTH();
        }
        else if(scriptName.equalsIgnoreCase("setRobotNetworkConfig"))
        {
          hasError = !setRobotNetworkConfig();
        }
        else if(scriptName.equalsIgnoreCase("initializeRobot"))
        {
          hasError = !initializeRobot();
        }
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
  protected boolean restartNaoTH()
  {
    try
    {
      if(openChannel("exec"))
      {
        setInfo("restarting naoqi");
        ((ChannelExec) channel).setCommand("/etc/init.d/naoqi restart");

        setInfo("waiting");

//        Thread.sleep(5000);

        setInfo("restarting naoth cognition process");
        ((ChannelExec) channel).setCommand("/etc/init.d/naoth stop");
        ((ChannelExec) channel).setCommand("/etc/init.d/naoth start");
        InputStream consoleOut = channel.getInputStream();
        channel.connect();
        byte[] buffer = new byte[4096];
        while(true) 
        {
          if(consoleOut.available()>0) 
          {
            int ret = consoleOut.read(buffer, 0, 4096);
            if(ret<0) 
            {
               break;
            }
          }
          if(((ChannelExec) channel).isClosed()) 
          {
            break;
          }
          try
          {
            Thread.sleep(100);
          }
          catch(Exception threadEx)
          {}
        }
      }
      disconnectChannel();
      channel.getExitStatus();
    }
    catch(Exception e)
    {
      haveError("Exception in restartNaoQi - Nao " + config.sNaoNo + ": " + e.toString());
      return false;
    }
    return true;
  }

  private String sshExecAndWaitForResponse(String cmd, String waitFor) throws InterruptedException
  {
     cIn.setText(cmd);
     ActionEvent evt = new ActionEvent(cIn, 0, "\n");
     ts.actionPerformed(evt);
     synchronized (ts) 
     {
        while(!ts.wasTriggered())
        {            
          Thread.sleep(50);           
        }
        ts.resetTriggered();
     }
     return sshWaitForResponse(waitFor);

  }

  private String sshWaitForResponse(String waitFor) throws InterruptedException
  {
    String[] waitForStrings = waitFor.split("\\|"); 

    String o = lastOut;
    String lastFound = null;
    int count = 20000;      

    boolean breakUp = false;
    while(!breakUp)
    {
      Thread.sleep(10);
      for(int i = 0; i < waitForStrings.length; i++)
      {
        if(o.contains(waitForStrings[i]))
        {
          breakUp = true;
          lastFound = waitForStrings[i];
        }
      }
      o = lastOut;
      if(count < 0)
      {
        breakUp = true;
        lastFound = null;
      }
      count--;
    }      
    lastOut = lastOut.replace(o, "");
    if(lastFound != null)
    {
      setInfo(o);
    }
//    infos += o.replaceAll("[\\W\\S\\D]+", o);
    return lastFound;
  }

  private boolean runShellScriptAsRoot(String directory, String shellScript) throws JSchException, InterruptedException 
  {
    String rootPW;
    rootPW = config.sshRootPassword;
    
    setInfo("trying to execute shellscript '" + shellScript + "' as root");
    
    if(openChannel("shell"))
    {
      cIn.setText("");
      lastOut = "";

      channel.setInputStream(ts, false);
      channel.setExtOutputStream(out, false);
      channel.setOutputStream(out, false);
      channel.connect();
      
      String response;

      response = sshWaitForResponse("localhost|nao|$");
      if(response == null)
      {
        showTimeOutMsg();
        return false;
      }
      response = sshExecAndWaitForResponse("su", "Password:");
      if(response == null)
      {
        showTimeOutMsg();
        return false;
      }
      response = sshExecAndWaitForResponse(rootPW, "localhost|nao|Authentication|failure|$");
      if(response == null || response.equals("Authentication") || response.equals("failure"))
      {
        haveError( "superuser password is wrong");
        ts.stop();
        disconnectChannel();
        return false;
      }
      response = sshExecAndWaitForResponse("cd " + directory, "localhost|nao|$");
      if(response == null)
      {
        showTimeOutMsg();
        return false;
      }
      response = sshExecAndWaitForResponse("chown root:root ./" + shellScript + " && chmod 744 ./" + shellScript + " && nohup ./" + shellScript + " 2>&1", "localhost|nao|$");
      if(response == null)
      {
        showTimeOutMsg();
        return false;
      }
      if(config.reboot)
      {
        response = sshExecAndWaitForResponse("cd .. && rm -rf ./naothSetup && reboot && exit", "localhost|nao|$");
      }
      else
      {
        response = sshExecAndWaitForResponse("cd .. && rm -rf ./naothSetup && exit", "localhost|nao|$");
      }
      if(response == null)
      {
        showTimeOutMsg();
        return false;
      }
      response = sshExecAndWaitForResponse("exit", "logout|localhost|nao|$");
    }
    setInfo("\n");
    ts.stop();
    disconnectChannel();
    return true;
  }
    
  private void showTimeOutMsg() throws JSchException
  {
    haveError("ssh shell response timed out");
    ts.stop();
    disconnectChannel();
  }
  
  /**
   * run shell script for network initialisation as root
   *
   * @param session jsch session
   * @param sNaoNo nao number
   */
  protected boolean setRobotNetworkConfig()
  {
    setInfo("initialization setup network part");
    try
    {
      return runShellScriptAsRoot(config.setupScriptPath(), "init_net.sh");
    }
    catch(Exception e)
    {
      haveError("Exception in setRobotNetworkConfig - Nao " + config.sNaoNo + ": " + e.toString());
      return false;
    }
  }

  /**
   * run shell script for complete robot initialization as root
   *
   * @param session jsch session
   * @param sNaoNo nao number
   */
  protected boolean initializeRobot()
  {     
    setInfo("initialization setup network part");
    try
    {
      return runShellScriptAsRoot(config.setupScriptPath(), "init_env.sh");
    }
    catch(Exception e)
    {
      haveError("Exception in setRobotNetworkConfig - Nao " + config.sNaoNo + ": " + e.toString());
      return false;
    }
  }

  @Override
  protected void done()
  {
    if(hasError)
    {
      haveError(errors);
    }
    try
    {
      cIn.removeActionListener(ts);
      out.close();
    }
    catch(IOException e){}
    actionDone("script");
  }

}
