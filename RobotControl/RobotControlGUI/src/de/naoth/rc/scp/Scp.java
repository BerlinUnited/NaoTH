package de.naoth.rc.scp;


import com.jcraft.jsch.*;
import com.jcraft.jsch.ChannelSftp.*;
import java.io.*;
import java.util.*;
import java.util.Arrays;

/**
 *
 * @author fh
 */
public class Scp {

  public ChannelSftp c;
  private Channel channel;
  private Session session;

  public Scp(String Ip) throws JSchException
  {
      String userName = "nao";
      UserInfo ui = new MyUserInfo();
      
      java.util.Properties config = new java.util.Properties();
      config.put("StrictHostKeyChecking", "no");

      JSch jsch = new JSch();
      session = jsch.getSession(userName, Ip, 22);
      session.setConfig(config);
      session.setUserInfo(ui);
      session.connect();
      channel = session.openChannel("sftp");
      channel.connect();
      c = (ChannelSftp) channel;
  }

  public void shutdown() {
    c.disconnect();
    channel.disconnect();
    session.disconnect();
  }

  /**
  * rm -r via sftp
  * @param c ChannelSftp Object
  * @param dstDir String directory to delete
  */
  public void rmDirSftp(String dstDir)
  {
    try
    {
      Vector v = c.ls(dstDir);
      if(v != null)
      {
        for(int i = 0; i < v.size(); i++)
        {
          Object obj = v.elementAt(i);
          if(obj instanceof LsEntry)
          {
            LsEntry lsEntry = (LsEntry) obj;
            if(!lsEntry.getFilename().equals(".") && !lsEntry.getFilename().equals(".."))
            {
              if(lsEntry.getAttrs().isDir())
              {
                rmDirSftp(dstDir + "/" + lsEntry.getFilename());
                c.rmdir(dstDir + "/" + lsEntry.getFilename());
              }
              else
              {
                c.rm(dstDir + "/" + lsEntry.getFilename());
              }
            }
          }
        }
      }
    }
    catch(Exception e)
    {

    }
  }

  /**
  * Recursively get Directory via Sftp
  * @param c ChannelSftp Object
  * @param local String Local Location (a.k.a destination)
  * @param remote String Remote Location
  */
  public void recursiveSftpGet(String local, String remote)
  {
    try
    {
      if(remote.endsWith("/"))
      {
        remote = remote.substring(0, remote.length() - 1);
      }
      Vector v = c.ls(remote);
      if(v != null)
      {
        for(int i = 0; i < v.size(); i++)
        {
          Object obj = v.elementAt(i);
          if(obj instanceof LsEntry)
          {
            LsEntry lsEntry = (LsEntry) obj;
            if(!lsEntry.getFilename().equals(".") && !lsEntry.getFilename().equals(".."))
            {
              String outputFileName = local + "/" + lsEntry.getFilename();
              File f = new File(outputFileName);
              if(lsEntry.getAttrs().isDir())
              {
                f.mkdirs();
                recursiveSftpGet(outputFileName, remote + "/" + lsEntry.getFilename());
              }
              else
              {
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
    }
  }

  /**
  * recursive Put via sftp
  * @param c ChannelSftp Object
  * @param src File Local Sources
  * @param dst String Remote Destination
  */
  public void recursiveSftpPut(File src, String dst)
  {
    try
    {
      File files[] = src.listFiles();
      Arrays.sort(files);
      for(int i = 0, n = files.length; i < n; i++)
      {
        if(files[i].isDirectory())
        {
          String newdst = dst + "/" + files[i].getName();
          try
          {
            c.mkdir(newdst);
          }
          catch(Exception e)
          {
          }
          recursiveSftpPut(files[i], newdst);
        }
        else
        {
          c.put(files[i].getAbsolutePath(), dst + "/" + files[i].getName());
        }

      }
    }
    catch(Exception e)
    {
    }
  }
  

  public class MyUserInfo implements UserInfo
  {
    public String getPassword() { return "nao"; }
    public boolean promptYesNo(String str) { return true; }
    public String getPassphrase() { return null; }
    public boolean promptPassphrase(String message) { return true; }
    public boolean promptPassword(String message) { return true; }
    public void showMessage(String message) { }
  }//end class MyUserInfo
  
}//end class scp