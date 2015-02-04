package naoscp.tools;

import com.jcraft.jsch.*;
import com.jcraft.jsch.ChannelSftp.*;
import static com.jcraft.jsch.Logger.ERROR;
import static com.jcraft.jsch.Logger.FATAL;
import static com.jcraft.jsch.Logger.INFO;
import static com.jcraft.jsch.Logger.WARN;
import java.io.*;
import java.util.*;
import java.util.Arrays;
import java.util.logging.Level;

/**
 *
 * @author Florian Holzhauer
 * @author Heinrich Mellmann
 */
public class Scp {

    private ChannelSftp channel;
    private Session session;
    private SftpProgressMonitor progressMonitor;
    
    public Scp(String ip, String userName, String password) throws JSchException {
        this(ip, userName, new SimpleUserInfo(password));
    }

    public Scp(String ip, String userName, UserInfo ui) throws JSchException {
        java.util.logging.Logger.getGlobal().log(Level.INFO, "connecting to " + userName + "@" + ip);
        
        java.util.Properties config = new java.util.Properties();
        config.put("StrictHostKeyChecking", "no");

        JSch.setLogger(new SimpleLogger());
        
        JSch jsch = new JSch();
        session = jsch.getSession(userName, ip, 22);
        session.setConfig(config);
        session.setUserInfo(ui);
        session.connect();

        channel = (ChannelSftp)session.openChannel("sftp");
        channel.connect();
        
    }

    public void disconnect() {
        channel.disconnect();
        session.disconnect();
    }

    public void setProgressMonitor(SftpProgressMonitor progressMonitor) {
        this.progressMonitor = progressMonitor;
    }
    
    public void run(String cmd) throws IOException, JSchException
    {
        java.util.logging.Logger.getGlobal().log(Level.INFO, "run: '" + cmd + "'");
        
        ChannelExec c = (ChannelExec)session.openChannel("exec");
        c.setCommand(cmd);
        c.setOutputStream(new LogStream(Level.FINE));
        c.setErrStream(new LogStream(Level.SEVERE));
        c.connect();
        // block until the execution is done
        while(!c.isClosed()) {
          try {
            Thread.sleep(100);
          } catch(InterruptedException ex) {}
        }
        
        java.util.logging.Logger.getGlobal().log(Level.INFO, "'" + cmd + "' exits with status " + c.getExitStatus());
    }

    /**
     * rm -r <dst>/*
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
        if(src.isDirectory())
        {
            File files[] = src.listFiles();
            Arrays.sort(files);
            for (int i = 0, n = files.length; i < n; i++) {
                String newdst = dst + "/" + files[i].getName();
                mkdir(newdst);
                put(files[i], newdst);
            }
        }
        else
        {
            java.util.logging.Logger.getGlobal().log(Level.FINE, "put " + dst + "");
            if(progressMonitor == null ) {
                channel.put(src.getAbsolutePath(), dst);
            } else {
                channel.put(src.getAbsolutePath(), dst, progressMonitor);
            }
        }
    }
    
    public void mkdir(String dst) throws SftpException {
        try {
            try {
                SftpATTRS attr = channel.stat(dst);
                if(!attr.isDir()) {
                    throw new SftpException(ChannelSftp.SSH_FX_FAILURE, "Not a directory " + dst + "( " + attr + ")");
                }
            } catch(SftpException ex) {
                if(ex.id == ChannelSftp.SSH_FX_NO_SUCH_FILE) {
                    channel.mkdir(dst);
                } else {
                    throw ex;
                }
            }
        } catch(SftpException ex) {
            throw new SftpException(ex.id, "Cannot create directory " + dst + ": " + ex.getMessage());
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

     protected class SimpleLogger implements Logger {

        @Override
        public boolean isEnabled(int level) {
            switch (level) {
                //case DEBUG: return true;
                //case INFO: return true;
                case WARN: return true;
                case ERROR: return true;
                case FATAL: return true;
            }
            return false;
        }

        Level getLevel(int level) {
            switch (level) {
                //case DEBUG: return true;
                case INFO: return Level.INFO;
                case WARN: return Level.WARNING;
                case ERROR: return Level.SEVERE;
                case FATAL: return Level.SEVERE;
            }
            return Level.INFO;
        }

        @Override
        public void log(int level, String message) {
            java.util.logging.Logger.getGlobal().log(getLevel(level), message);
        }
    }
     
    protected class LogStream extends OutputStream
    {
        private final StringBuilder buffer = new StringBuilder();
        private final Level level;
        
        public LogStream(Level level) {
            super();
            this.level = level;
        }
        
        @Override
        public void write(int i) throws IOException {
            if(i == '\n') {
                flush();
            } else {
                buffer.append((char)i);
            }
        }

        @Override
        public void flush() throws IOException {
            super.flush();
            java.util.logging.Logger.getGlobal().log(level, "  " + buffer.toString());
            buffer.setLength(0);
       }
    }
  
}//end class scp
