package naoscp.tools;

import com.jcraft.jsch.*;
import com.jcraft.jsch.ChannelSftp.*;
import static com.jcraft.jsch.Logger.DEBUG;
import static com.jcraft.jsch.Logger.ERROR;
import static com.jcraft.jsch.Logger.FATAL;
import static com.jcraft.jsch.Logger.INFO;
import static com.jcraft.jsch.Logger.WARN;
import java.io.*;
import java.util.*;
import java.util.Arrays;

/**
 *
 * @author Florian Holzhauer
 * @author Heinrich Mellmann
 */
public class Scp {

    private ChannelSftp channel;
    private Session session;

    public Scp(String ip, String userName, String password) throws JSchException {
        this(ip, userName, new SimpleUserInfo(password));
    }

    public Scp(String ip, String userName, UserInfo ui) throws JSchException {
        java.util.Properties config = new java.util.Properties();
        config.put("StrictHostKeyChecking", "no");

        JSch.setLogger(new Logger());
        
        JSch jsch = new JSch();
        session = jsch.getSession(userName, ip, 22);
        session.setConfig(config);
        session.setUserInfo(ui);
        session.connect();

        Channel c = session.openChannel("sftp");
        c.connect();
        channel = (ChannelSftp) c;
    }

    public void disconnect() {
        channel.disconnect();
        session.disconnect();
    }

    /**
     * rm -r <dst>/
     *
     *
     * @param dst String directory to delete
     */
    public void cleardir(String dst) throws SftpException {
        Vector v = channel.ls(dst);
        if (v != null) {
            for (int i = 0; i < v.size(); i++) {
                Object obj = v.elementAt(i);
                if (obj instanceof LsEntry) {
                    LsEntry lsEntry = (LsEntry) obj;
                    if (!lsEntry.getFilename().equals(".") && !lsEntry.getFilename().equals("..")) {
                        String child_dst = dst + "/" + lsEntry.getFilename();
                        if (lsEntry.getAttrs().isDir()) {
                            cleardir(child_dst);
                            channel.rmdir(child_dst);
                        } else {
                            channel.rm(child_dst);
                        }
                    }
                }
            }
        }
    }

    /**
     * Recursively get via Sftp
     *
     * @param dst String Local Location (a.k.a destination)
     * @param src String Remote Location
     */
    public void get(String src, File dst) throws SftpException, FileNotFoundException {
        if (src.endsWith("/")) {
            src = src.substring(0, src.length() - 1);
        }
        Vector v = channel.ls(src);
        if (v != null) {
            for (int i = 0; i < v.size(); i++) {
                Object obj = v.elementAt(i);
                if (obj instanceof LsEntry) {
                    LsEntry lsEntry = (LsEntry) obj;
                    if (!lsEntry.getFilename().equals(".") && !lsEntry.getFilename().equals("..")) {
                        String child_src = src + "/" + lsEntry.getFilename();
                        File child_dst = new File(dst, lsEntry.getFilename());
                        if (lsEntry.getAttrs().isDir()) {
                            child_dst.mkdirs();
                            get(child_src, child_dst);
                        } else {
                            FileOutputStream fos = new FileOutputStream(child_dst);
                            channel.get(child_src, fos);
                        }
                    }
                }
            }
        }
    }

    /**
     * recursive put via sftp
     *
     * @param src File Local Sources
     * @param dst String Remote Destination
     * @throws com.jcraft.jsch.SftpException
     */
    public void put(File src, String dst) throws SftpException {
        File files[] = src.listFiles();
        Arrays.sort(files);
        for (int i = 0, n = files.length; i < n; i++) {
            String newdst = dst + "/" + files[i].getName();
            if (files[i].isDirectory()) {
                channel.mkdir(newdst);
                put(files[i], newdst);
            } else {
                channel.put(files[i].getAbsolutePath(), newdst);
            }
        }
    }

    public static class SimpleUserInfo implements UserInfo, UIKeyboardInteractive {

        private final String pwd;

        public SimpleUserInfo(String pwd) {
            this.pwd = pwd;
        }

        @Override
        public String getPassword() {
            return this.pwd;
        }

        @Override
        public boolean promptYesNo(String str) {
            return true;
        }

        @Override
        public String getPassphrase() {
            return null;
        }

        @Override
        public boolean promptPassphrase(String message) {
            return true;
        }

        @Override
        public boolean promptPassword(String message) {
            return true;
        }

        @Override
        public void showMessage(String message) {
        }

        @Override
        public String[] promptKeyboardInteractive(
                String destination,
                String name,
                String instruction,
                String[] prompt,
                boolean[] echo
        ) {
            return new String[]{getPassword()};
        }
    }//end class SimpleUserInfo

     protected class Logger implements com.jcraft.jsch.Logger 
  {
    HashMap<Integer, String> name = new HashMap<Integer, String>();
    {
      name.put(new Integer(DEBUG), "DEBUG: ");
      name.put(new Integer(INFO), "INFO: ");
      name.put(new Integer(WARN), "WARN: ");
      name.put(new Integer(ERROR), "ERROR: ");
      name.put(new Integer(FATAL), "FATAL: ");
    }

    @Override
    public boolean isEnabled(int level)
    {
      return true;
    }

    @Override
    public void log(int level, String message)
    {
      System.err.print(name.get(new Integer(level)));
      System.err.println(message);
    }
  }   
}//end class scp
