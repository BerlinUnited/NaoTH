/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */


import com.jcraft.jsch.*;
import com.jcraft.jsch.ChannelSftp.*;
//import java.awt.Component;
import java.util.*;
import javax.swing.*;
import java.io.*;
import java.net.*;
import java.util.regex.*;

/**
 *
 * @author claas
 */
abstract class sshWorker extends SwingWorker<Boolean, File>
{
  naoScpConfig config;

  protected Session session;
  protected Channel channel;

  public boolean hasError;
  public String errors;
  public String infos;

  protected class sshLogger implements com.jcraft.jsch.Logger 
  {
    HashMap<Integer, String> name = new HashMap<Integer, String>();
    {
      name.put(new Integer(DEBUG), "DEBUG: ");
      name.put(new Integer(INFO), "INFO: ");
      name.put(new Integer(WARN), "WARN: ");
      name.put(new Integer(ERROR), "ERROR: ");
      name.put(new Integer(FATAL), "FATAL: ");
    }

    public boolean isEnabled(int level)
    {
      return true;
    }

    public void log(int level, String message)
    {
      System.err.print(name.get(new Integer(level)));
      System.err.println(message);
    }
  }      

  protected class sshUserInfo implements UserInfo, UIKeyboardInteractive
  {
    private String passwd;

    public sshUserInfo(String passwd) 
    {
      this.passwd = passwd;
    }


    public String getPassword()
    { 
      return passwd; 
    }

    public boolean promptYesNo(String str)
    {
      Object[] options={ "yes", "no" };
      int foo=JOptionPane.showOptionDialog(null, 
             str,
             "Warning", 
             JOptionPane.DEFAULT_OPTION, 
             JOptionPane.WARNING_MESSAGE,
             null, options, options[0]);
       return foo==0;
    }

    public String getPassphrase()
    { 
      return null; 
    }

    public boolean promptPassphrase(String message)
    { 
      return true; 
    }

    public boolean promptPassword(String message)
    {
      JTextField pwField=(JTextField)new JPasswordField(20);
      Object[] ob = {pwField}; 
      int result = JOptionPane.showConfirmDialog(null, ob, message, JOptionPane.OK_CANCEL_OPTION);
      if(result == JOptionPane.OK_OPTION)
      {
        passwd = pwField.getText();
        return true;
      }
      else
      { 
        return false; 
      }
    }

    public void showMessage(String message)
    {
      JOptionPane.showMessageDialog(null, message);
    }

    public String[] promptKeyboardInteractive
    (
      String destination,
      String name,
      String instruction,
      String[] prompt,
      boolean[] echo
    )
    {
      return new String[]{passwd};
    }
  }


  public sshWorker(naoScpConfig config, String Ip, String sNaoNo, String sNaoByte)
  {
    this.config = config;
    config.Ip = Ip;
    config.sNaoNo = sNaoNo;
    config.sNaoByte = sNaoByte;
    hasError = false;
    errors = "";
    infos = "";
    
    session = null;
    channel = null;    
  }

  protected boolean connect() throws JSchException
  {
    if(session == null || !session.isConnected())
    {
      setInfo("Trying to connect to " + config.Ip);
      naoSsh(config.Ip);
    }
    return session.isConnected();
  }

  protected boolean disconnect() throws JSchException
  {
    if(session != null && session.isConnected())
    {
      session.disconnect();
    }
    return !session.isConnected();
  }

  protected boolean openChannel(String type) throws JSchException
  {
    if(channel == null || !channel.isConnected())
    {
      channel = session.openChannel(type);
    }
    return !channel.isClosed();
  }

  protected boolean disconnectChannel() throws JSchException
  {
    if(channel != null && channel.isConnected())
    {
      channel.disconnect();
    }
    return !channel.isConnected();
  }

  /**
   * returns ssh-session object
   * @param Ip robot ip
   * @return session
   * @throws com.jcraft.jsch.JSchException
   */
  protected void naoSsh(String Ip) throws JSchException
  {
    String password = config.sshPassword;
    String user = config.sshUser;
    
    UserInfo ui = new sshUserInfo(password);
    java.util.Properties sshConfig = new java.util.Properties();
    sshConfig.put("StrictHostKeyChecking", "no");
    if(config.debugVersion)
    {
      JSch.setLogger(new sshLogger());
    }
    JSch jsch = new JSch();

    session = jsch.getSession(user, Ip, 22);
    session.setConfig(sshConfig);
    session.setUserInfo(ui);
    session.setPassword(password);
    session.connect();
  }

  public boolean testConnection()
  {
    try
    {
      InetAddress[] naoIps = InetAddress.getAllByName(config.Ip);
      int idx = 0;
      while(idx < naoIps.length)
      {
        setInfo("Try to reach Nao " + config.sNaoNo + " (" + naoIps[idx].getHostAddress() + ")");
        config.Ip = naoIps[idx].getHostAddress();
        if(naoIps[idx].isReachable(2500) && connect())
        {
          Channel channel = session.openChannel("sftp");
          channel.connect();
          Thread.sleep(100);
          if(channel.isConnected())
          {
            channel.disconnect();
            Thread.sleep(100);
            return true;
          }
          else
          {
            setInfo("Nao " + config.sNaoNo + " (" + config.Ip + ") could not open channel");
          }
          disconnect();
        }
        else
        {
          setInfo("Nao " + config.sNaoNo + " (" + config.Ip + ") unreachable");
        }
        idx++;
      }
    }
    catch(Exception e)
    {
      setInfo(e.toString());
      return false;
    }
    return false;
  }

  /**
   * rm -r via sftp
   * @param c ChannelSftp Object
   * @param dstDir String directory to delete
   */
  protected void rmDirSftp(String dstDir)
  {
    ChannelSftp c = (ChannelSftp) channel;
    try
    {
      Vector v = c.ls(dstDir);
      if(v != null)
      {
        for(int i = 0; i < v.size(); i ++)
        {
          Object obj = v.elementAt(i);
          if(obj instanceof LsEntry)
          {
            LsEntry lsEntry = (LsEntry) obj;
            if( ! lsEntry.getFilename().equals(".") &&  ! lsEntry.getFilename().
            equals(".."))
            {
              if(lsEntry.getAttrs().isDir())
              {
                rmDirSftp(dstDir + "/" + lsEntry.getFilename());
                c.rmdir(dstDir + "/" + lsEntry.getFilename());
              }
              else
              {
                setInfo("remote rm " + lsEntry.getFilename());
                c.rm(dstDir + "/" + lsEntry.getFilename());
              }
            }
          }
        }
      }
    }
    catch(Exception e)
    {
      setInfo("Exception in rmDirSftp: (" + dstDir + ")" + e.toString());
    }
  }

  /**
   * Recursively get Directory via Sftp
   * @param c ChannelSftp Object
   * @param local String Local Location (a.k.a destination)
   * @param remote String Remote Location
   */
  protected void recursiveSftpGet(String local, String remote)
  {
    ChannelSftp c = (ChannelSftp) channel;
    try
    {
      if(remote.endsWith("/"))
      {
        remote = remote.substring(0, remote.length() - 1);
      }
      Vector v = c.ls(remote);
      if(v != null)
      {
        for(int i = 0; i < v.size(); i ++)
        {
          Object obj = v.elementAt(i);
          if(obj instanceof LsEntry)
          {
            LsEntry lsEntry = (LsEntry) obj;
            if( ! lsEntry.getFilename().equals(".") &&  ! lsEntry.getFilename().
            equals(".."))
            {
              String outputFileName = local + "/" + lsEntry.getFilename();
              File f = new File(outputFileName);
              if(lsEntry.getAttrs().isDir())
              {
                f.mkdirs();
                recursiveSftpGet(outputFileName, remote + "/" + lsEntry.
                getFilename());
              }
              else
              {
                setInfo("get " + lsEntry.getFilename());
                FileOutputStream foo = new FileOutputStream(f);
                c.get(remote + "/" + lsEntry.getFilename(), foo);
              }
            }
          }
        }
      }
    }
    catch(Exception e)
    {
      setInfo("Exception in recursiveSftpGet: " + e.toString());
    }
    
  }
  /**
   * Get Directory via Sftp by Regex
   * @param c ChannelSftp Object
   * @param local String Local Location (a.k.a destination)
   * @param remote String Remote Location
   * @param pattern String Regex
   *
   */
  protected void regexRecursiveSftpGet(String local, String remote, String pattern)
  {
    ChannelSftp c = (ChannelSftp) channel;
    
    try
    {
      if(remote.endsWith("/"))
      {
        remote = remote.substring(0, remote.length() - 1);
      }
      Pattern p = Pattern.compile(pattern);
      Vector v = c.ls(remote);
      if(v != null)
      {
        for(int i = 0; i < v.size(); i ++)
        {
          Object obj = v.elementAt(i);
          if(obj instanceof LsEntry)
          {
            LsEntry lsEntry = (LsEntry) obj;
            boolean match = p.matcher(lsEntry.getFilename()).find();
            if( ! lsEntry.getFilename().equals(".") &&  ! lsEntry.getFilename().equals(".."))
            {
              String outputFileName = local + "/" + lsEntry.getFilename();
              File f = new File(outputFileName);
              if(lsEntry.getAttrs().isDir()) {
                regexRecursiveSftpGet(outputFileName, remote + "/" + lsEntry.getFilename(), pattern);
              } else if(match)
              {
                File f2 = new File(f.getParent());
                if(!f2.exists()) {
                    f2.mkdirs();
                }
                setInfo("get " + lsEntry.getFilename());
                FileOutputStream foo = new FileOutputStream(f);
                c.get(remote + "/" + lsEntry.getFilename(), foo);
              }
            }
          }
        }
      }
    }
    catch(Exception e)
    {
      setInfo("Exception in recursiveSftpGet: " + e.toString());
    }    
  }

  protected boolean recursiveSftpCheckPath(File src, String dst)
  {    
    ChannelSftp c = (ChannelSftp) channel;
    
    String[] splittedPath = dst.split("[/\\\\]");
    String path = "";
    int end = splittedPath.length;
    if(src.isFile())
    {
      end --;
    }

    for(int p = 1; p < end; p++)
    {
      path += "/" + splittedPath[p];
      try
      {
        SftpATTRS attrs = c.stat(path);
      }
      catch(SftpException e)
      {
        try
        {
          c.mkdir(path);
        }
        catch(SftpException ex)
        {
          setInfo("mkdir exception (" + e.toString() + ")");
          return false;
        }
      }
    }
    return true;
  }

  /**
   * recursive Put via sftp
   * @param c ChannelSftp Object
   * @param src File Local Sources
   * @param dst String Remote Destination
   */
  protected void recursiveSftpPut(File src, String dst)
  {
    if(src.exists())
    {
      ChannelSftp c = (ChannelSftp) channel;
      try
      {
        if(recursiveSftpCheckPath(src, dst) && src.isDirectory())
        {
          File files[] = src.listFiles();
          Arrays.sort(files);
          for(int i = 0, n = files.length; i < n; i ++)
          {
            String newdst = dst + "/" + files[i].getName();
            recursiveSftpPut(files[i], newdst);
          }
        }
        else if(src.isFile())
        {
          setInfo("put " + src.getName());
          if(recursiveSftpCheckPath(src, dst))
          {
            try
            {
              c.rm(dst);
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
              c.put(src.getAbsolutePath(), dst);
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
          else
          {
            setInfo("remote directory does not exist and could not be created.");
          }
        }
        else
        {
          setInfo("remote directory does not exist and could not be created.");
        }
      }
      catch(Exception e)
      {
        setInfo("Exception in recursiveSftpPut: " + e.toString());
      }
    }
    else
    {
      setInfo("possible Error: file or directory '" + src.getName() + "' does not exist or is not accessable!");
    }
  }
  
  abstract protected void setInfo(final String info);
  
  abstract protected void haveError(final String error);

  abstract protected void actionDone(final String action);

  public boolean hadError()
  {
    return hasError;
  }
  
}
