package de.naoth.naoscp;

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
abstract class SshWorker extends SwingWorker<Boolean, File>
{
  NaoScpConfig config;

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

  public SshWorker(NaoScpConfig naoScpConfig, String ip, String sNaoNo, String sNaoByte)
  {
    config = naoScpConfig;
    config.addresses.clear();
    config.addresses.add(ip);
    init(sNaoNo, sNaoByte);
  }

  public SshWorker(NaoScpConfig naoScpConfig, String sNaoNo, String sNaoByte)
  {
    config = naoScpConfig;
    init(sNaoNo, sNaoByte);
  }
  
  private void init(String sNaoNo, String sNaoByte)
  {
    if(config.addresses.size() > 0)
    {
      config.actIp = config.addresses.get(0);
    }
    else
    {
      config.actIp = "";
    }
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
    session = connect(session, config.actIp);
    return (session != null && session.isConnected());
  }

  protected Session connect(Session s, String ip) throws JSchException
  {
    if(s == null || !s.isConnected())
    {
      setInfo("Trying to connect to " + ip);
      s = naoSsh(ip);
    }
    return s;
  }

  protected boolean disconnect() throws JSchException
  {
    return disconnect(session);
  }

  protected boolean disconnect(Session s) throws JSchException
  {
    if(s != null && s.isConnected())
    {
      s.disconnect();
    }
    return (s == null || !s.isConnected());
  }

  protected boolean openChannel(String type) throws JSchException
  {
    channel = openChannel(session, channel, type);
    return !channel.isClosed();
  }

  protected Channel openChannel(Session s, Channel c, String type) throws JSchException
  {
    if(c == null || !c.isConnected())
    {
      c = s.openChannel(type);
    }
    return c;
  }

  protected boolean disconnectChannel() throws JSchException
  {
    return disconnectChannel(channel);
  }
  
  protected boolean disconnectChannel(Channel c) throws JSchException
  {
    if(c != null && c.isConnected())
    {
      c.disconnect();
    }
    return (c == null || !c.isConnected());
  }

  /**
   * returns ssh-session object
   * @param Ip robot ip
   * @return session
   * @throws com.jcraft.jsch.JSchException
   */
  protected Session naoSsh(String Ip) throws JSchException
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

    Session s = jsch.getSession(user, Ip, 22);
    s.setConfig(sshConfig);
    s.setUserInfo(ui);
    s.setPassword(password);
    s.connect();
    return s;
  }

  public boolean testAndConnect()
  {
    try
    {
      int idx = 0;      
      while(idx < config.addresses.size())
      {
        config.actIp = config.addresses.get(idx);
        idx++;
        InetAddress iAddr = InetAddress.getByName(config.actIp);
        System.out.println(idx);
        if(iAddr.isReachable(2500))
        {
          if(connect())
          {
            Channel c = null;
            c = openChannel(session, c, "sftp");
            if(!c.isClosed())
            {
              c.disconnect();
//              Thread.sleep(100);            
              return true;
            }
            else
            {
              disconnect();
              setInfo("[0;31mNao " + config.sNaoNo + " (" + config.actIp + ") could not open channel\n[0m");
            }
          }
        }
        else
        {
          setInfo("[0;31mNao " + config.sNaoNo + " (" + config.actIp + ") unreachable\n[0m");
        }
      }
    }
    catch(Exception e)
    {
      haveError("Exeption while testing connectivity (" + e.toString() + ")");
      return false;
    }
    hasError = true;
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
            if( !lsEntry.getFilename().equals("nao.info") && !lsEntry.getFilename().equals(".") &&  ! lsEntry.getFilename().equals(".."))
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
      setInfo("[0;31mException in rmDirSftp: (" + dstDir + ")" + e.toString() + "\n[0m");
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
                c.get(remote + "/" + lsEntry.getFilename(), foo, new ProgressMonitor(config.progressBar));
              }
            }
          }
        }
      }
    }
    catch(Exception e)
    {
      setInfo("[0;31mException in recursiveSftpGet: " + e.toString() + "\n[0m");
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
                c.get(remote + "/" + lsEntry.getFilename(), foo, new ProgressMonitor(config.progressBar));
              }
            }
          }
        }
      }
    }
    catch(Exception e)
    {
      setInfo("[0;31mException in recursiveSftpGet: " + e.toString() + "\n[0m");
    }    
  }

  protected boolean sftpCheckRemotePath(File src, String dst)
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
          setInfo("[0;31mException in recursiveSftpCheckPath - mkdir (" + e.toString() + ")\n[0m");
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
    if(src != null && src.exists())
    {
      ChannelSftp c = (ChannelSftp) channel;
      try
      {
        if(sftpCheckRemotePath(src, dst) && src.isDirectory())
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
          if(sftpCheckRemotePath(src, dst))
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
              c.put(src.getAbsolutePath(), dst, new ProgressMonitor(config.progressBar));
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
        setInfo("[0;31mException in recursiveSftpPut: " + e.toString() + "\n[0m");
      }
    }
    else
    {
      setInfo("[43mpossible Error:[0m file or directory '" + src.getName() + "' does not exist or is not accessable!\n[0m");
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
