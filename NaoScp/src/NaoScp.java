/*
 *
 * Warning - this is a "pre alpha version" and is largely untested
 *
 * Code and UI might need some cleanup, too (e.g. similar functions sometimes
 * expect String, sometimes File as parameter, which is ugly, but worksfornow [tm]
 *
 * Todo:
 *  * Nicer Error handling
 *  * if "default directory structure" on robot is missing, try to fix this instead of Exception
 *  * ssh-button?
 * 
 * @author Florian Holzhauer <fh-hu@fholzhauer.de> 2009
 * 
 */

import com.jcraft.jsch.*;
import com.jcraft.jsch.ChannelSftp.*;
import java.awt.Color;
import javax.swing.*;
import java.io.*;
import java.util.*;
import java.util.regex.*;
// import java.util.Arrays;
import java.net.*;
import java.text.*;

public class NaoScp extends javax.swing.JFrame
{

  private boolean fhIsTesting = false;
  // if true, all ActionInfo is sent to log
  // @see actionInfo()
  private boolean logActionInfo = true;
  private boolean debugVersion = true;
  private HashMap copyDone = new HashMap();
  private HashMap hadErrors = new HashMap();
  private HashMap backups = new HashMap();

  @SuppressWarnings("unchecked")
  public NaoScp()
  {
    initComponents();

    this.noBackup.setVisible(debugVersion);
    this.noBackup.setEnabled(debugVersion);
    this.progressBar.setVisible(false);
    this.progressBar.setEnabled(false);

    String value = System.getenv("NAOTH_BZR");
    
    if(value != null)
    {
      value = value + "/NaoTHSoccer";
      setDirectory(value);
    }
    else
    {
      String ResourceName="NaoScp.class";
      String programPath = URLDecoder.decode(this.getClass().getClassLoader().getResource(ResourceName).getPath());
      programPath = programPath.replace("file:", "");
      programPath = programPath.replace("/NaoScp/dist/NaoScp.jar!/NaoScp.class", "");
      programPath = programPath.replace("/NaoScp/build/classes/NaoScp.class", "") + "/NaoController";
      File ProgramDir = new File(programPath);
      if(ProgramDir.exists())
      {
        setDirectory(ProgramDir.getAbsolutePath());
      }
   }

    if(fhIsTesting)
    {
      remotePrefix = "/Users/robotest/deploy";
      subnetFieldLAN.setText("127.0.0");
      sshUser.setText("robotest");
      sshPassword.setText("robotest");
    }
  }

  public static void main(String args[])
  {
    java.awt.EventQueue.invokeLater(new Runnable()
    {

      public void run()
      {
        new NaoScp().setVisible(true);
      }
    });
  }
  //Timestamp-Prefix used for the nameing scheme in Deploy/in/
  private String backupTimestamp;
  //Only for debugging purposes
  private String remotePrefix = "";

  public String remoteRootPath(int i)
  {
    if(fhIsTesting)
    {
      return remotePrefix + "/" + String.valueOf(i);
    }
    return remotePrefix;
  }

  public String configPath()
  {
    return "/home/nao/naoqi/Config";
  }

  public String libnaoPath()
  {
    return "/home/nao/naoqi/lib/naoqi";
  }
  
  public String binPath()
  {
    return "/home/nao/bin/";
  }

  public String localDeployRootPath()
  {
    return jDirPathLabel.getText() + "/Deploy";
  }

  public String localDeployOutPath(int i)
  {
    return localDeployRootPath() + "/out/" + String.valueOf(i);
  }

  public String localDeployInPath(int i, int iNaoByte)
  {
    return localDeployRootPath() + "/in/" + backupTimestamp + "-" + String.valueOf(i) + "-" + String.valueOf(iNaoByte);
  }

  public String localConfigPath()
  {
    return jDirPathLabel.getText() + "/Config";
  }

  public String localLibnaothPath()
  {
    return jDirPathLabel.getText() + "/dist/Nao";
  }

    private void copyButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_copyButtonActionPerformed

      setFormEnabled(false);
      jLogWindow.setText("");

      backupTimestamp = String.valueOf(System.currentTimeMillis());
      prepareDeploy();

      String sNaoOneLanIp = "";
      String sNaoTwoLanIp = "";
      String sNaoThreeLanIp = "";
      String sNaoFourLanIp = "";
      String sNaoOneWlanIp = "";
      String sNaoTwoWlanIp = "";
      String sNaoThreeWlanIp = "";
      String sNaoFourWlanIp = "";

      if(subnetFieldLAN.getText().endsWith("."))
      {
        sNaoOneLanIp = subnetFieldLAN.getText() + naoByte1.getText();
        sNaoTwoLanIp = subnetFieldLAN.getText() + naoByte2.getText();
        sNaoThreeLanIp = subnetFieldLAN.getText() + naoByte3.getText();
        sNaoFourLanIp = subnetFieldLAN.getText() + naoByte4.getText();
        sNaoOneWlanIp = subnetFieldWLAN.getText() + naoByte1.getText();
        sNaoTwoWlanIp = subnetFieldWLAN.getText() + naoByte2.getText();
        sNaoThreeWlanIp = subnetFieldWLAN.getText() + naoByte3.getText();
        sNaoFourWlanIp = subnetFieldWLAN.getText() +  naoByte4.getText();
      }
      else
      {
        sNaoOneLanIp = subnetFieldLAN.getText() + "." + naoByte1.getText();
        sNaoTwoLanIp = subnetFieldLAN.getText() + "." + naoByte2.getText();
        sNaoThreeLanIp = subnetFieldLAN.getText() + "." + naoByte3.getText();
        sNaoFourLanIp = subnetFieldLAN.getText() + "." + naoByte4.getText();
        sNaoOneWlanIp = subnetFieldWLAN.getText() + "." + naoByte1.getText();
        sNaoTwoWlanIp = subnetFieldWLAN.getText() + "." + naoByte2.getText();
        sNaoThreeWlanIp = subnetFieldWLAN.getText() + "." + naoByte3.getText();
        sNaoFourWlanIp = subnetFieldWLAN.getText() + "." + naoByte4.getText();
      }

      int iNaoByte1 = Integer.parseInt(naoByte1.getText());
      int iNaoByte2 = Integer.parseInt(naoByte2.getText());
      int iNaoByte3 = Integer.parseInt(naoByte3.getText());
      int iNaoByte4 = Integer.parseInt(naoByte4.getText());

      copyDone.put(1, new Boolean(false));
      copyDone.put(2, new Boolean(false));
      copyDone.put(3, new Boolean(false));
      copyDone.put(4, new Boolean(false));
      hadErrors.put(1, new Boolean(false));
      hadErrors.put(2, new Boolean(false));
      hadErrors.put(3, new Boolean(false));
      hadErrors.put(4, new Boolean(false));

      if(iNaoByte1 < 256 && iNaoByte1 > -1)
      {
        Copier cLan = new Copier(sNaoOneLanIp, 1, iNaoByte1);
        Copier cWlan = new Copier(sNaoOneWlanIp, 1, iNaoByte1);
        if(cLan.testConnection())
        {
          cLan.execute();
        }
        else if(cWlan.testConnection())
        {
          cWlan.execute();
        }
        else
        {
          log("Nao 1 per LAN & WLAN nicht erreichbar");
          haveError(1, "Nao " + iNaoByte1 + " per LAN & WLAN nicht erreichbar");
          copyIsDone(1);
        }
      }
      else
      {
        copyIsDone(1);
      }

      if(iNaoByte2 < 256 && iNaoByte2 > -1)
      {
        Copier cLan = new Copier(sNaoTwoLanIp, 2, iNaoByte2);
        Copier cWlan = new Copier(sNaoTwoWlanIp, 2, iNaoByte2);
        if(cLan.testConnection())
        {
          cLan.execute();
        }
        else if(cWlan.testConnection())
        {
          cWlan.execute();
        }
        else
        {
          log("Nao 2 per LAN & WLAN nicht erreichbar");
          haveError(2, "Nao " + iNaoByte2 + " per LAN & WLAN nicht erreichbar");
          copyIsDone(2);
        }
      }
      else
      {
        copyIsDone(2);
      }

      if(iNaoByte3 < 256 && iNaoByte3 > -1)
      {
        Copier cLan = new Copier(sNaoThreeLanIp, 3, iNaoByte3);
        Copier cWlan = new Copier(sNaoThreeWlanIp, 3, iNaoByte3);
        if(cLan.testConnection())
        {
          cLan.execute();
        }
        else if(cWlan.testConnection())
        {
          cWlan.execute();
        }
        else
        {
          log("Nao 3 per LAN & WLAN nicht erreichbar");
          haveError(3, "Nao " + iNaoByte3 + " over LAN and WLAN unreachable");
          copyIsDone(3);
        }
      }
      else
      {
        copyIsDone(3);
      }

      if(iNaoByte4 < 256 && iNaoByte4 > -1)
      {
        Copier cLan = new Copier(sNaoFourLanIp, 4, iNaoByte4);
        Copier cWlan = new Copier(sNaoFourWlanIp, 4, iNaoByte4);
        if(cLan.testConnection())
        {
          cLan.execute();
        }
        else if(cWlan.testConnection())
        {
          cWlan.execute();
        }
        else
        {
          log("Nao 4 per LAN & WLAN nicht erreichbar");
          haveError(4, "Nao " + iNaoByte4 + " over LAN and WLAN unreachable");
          copyIsDone(4);
        }
      }
      else
      {
        copyIsDone(4);
      }
    }//GEN-LAST:event_copyButtonActionPerformed

  public void haveError(int i, String error)
  {
    hadErrors.put(i, new Boolean(true));
  }

  public void copyIsDone(int i)
  {
    copyDone.put(i, new Boolean(true));
    Boolean done1 = (Boolean) copyDone.get(1);
    Boolean done2 = (Boolean) copyDone.get(2);
    Boolean done3 = (Boolean) copyDone.get(3);
    Boolean done4 = (Boolean) copyDone.get(4);
    Boolean err1 = (Boolean) hadErrors.get(1);
    Boolean err2 = (Boolean) hadErrors.get(2);
    Boolean err3 = (Boolean) hadErrors.get(3);
    Boolean err4 = (Boolean) hadErrors.get(4);

    if(done1 && done2 && done3 && done4)
    {
      String add = "";
      if(err1 || err2 || err3 || err4)
      {
        add = " - error with Nao ";
        if(err1)
        {
          add = add + " 1 ";
        }
        if(err2)
        {
          add = add + " 2 ";
        }
        if(err3)
        {
          add = add + " 3 ";
        }
        if(err4)
        {
          add = add + " 4 ";
        }
        add = add + " - please check Logs.";
      }
      setFormEnabled(true);
      resetBackupList();
      JOptionPane.showMessageDialog(null, "Copy done" + add);
    }
  }

  private void setFormEnabled(boolean enable)
  {
      copyButton.setEnabled(enable);
//      copyConfig.setEnabled(enable);
//      copyLib.setEnabled(enable);
//      copyLogs.setEnabled(enable);
      if(jBackupBox.getItemCount() > 1 || !enable)
      {
        jBackupBox.setEnabled(enable);
      }
      jColorBox.setEnabled(enable);
      jCommentTextArea.setEnabled(enable);
      jDirChooser.setEnabled(enable);
      jSchemeBox.setEnabled(enable);
      lblTeamCommWLAN.setEnabled(enable);
      jTeamCommPort.setEnabled(enable);
      jTeamNumber.setEnabled(enable);
      naoByte1.setEnabled(enable);
      naoByte2.setEnabled(enable);
      naoByte3.setEnabled(enable);
      naoByte4.setEnabled(enable);
      sshPassword.setEnabled(enable);
      sshUser.setEnabled(enable);
      subnetFieldLAN.setEnabled(enable);
      subnetFieldWLAN.setEnabled(enable);
  }


  /**
   * Staging - prepares "deploy out dir" with the files to be copied to the robots
   * copies files, creates scheme.cfg, modifies other cfgs according to UI settings
   * @return
   */
  private boolean prepareDeploy()
  {
    actionInfo("preparing deploy dir");

    File deployOutDir = new File(localDeployRootPath());

    if( ! deployOutDir.isDirectory())
    {
      if( ! deployOutDir.mkdirs())
      {
        log("Error: Could not create deploy out directory");
        return false;
      }
    }

    if( ! deployOutDir.canWrite())
    {
      log("Error: Cant write to deploy out dir");
      return false;
    }

    for(int i = 1; i <= 4; i ++)
    {
      int iNaoByte = -1;
      switch(i)
      {
        case 1:
         iNaoByte = Integer.parseInt(naoByte1.getText());
         break;

        case 2:
         iNaoByte = Integer.parseInt(naoByte2.getText());
         break;

        case 3:
         iNaoByte = Integer.parseInt(naoByte3.getText());
         break;

        case 4:
         iNaoByte = Integer.parseInt(naoByte4.getText());
         break;

      }

      if(iNaoByte < 256 && iNaoByte > -1)
      {
        String naoNumber = String.valueOf(i);
        String currentDeployPath = localDeployOutPath(i);
        File currentDeployDir = new File(currentDeployPath);

        if(currentDeployDir.isDirectory())
        {
          deleteDir(currentDeployDir);
        }

        currentDeployDir.mkdirs();
        File localLibPath = new File(localDeployOutPath(i) + libnaoPath());
        localLibPath.mkdirs();
        File localBinPath = new File(localDeployOutPath(i) + binPath());
        localBinPath.mkdirs();

        if(copyLib.isSelected())
        {
          File localLib = new File(
          localDeployOutPath(i) + libnaoPath() + "/libnaoth.so");
          if(localLib.exists())
          {
            localLib.delete();
          }
          try
          {
            FileOutputStream fos = new FileOutputStream(localDeployOutPath(i) + libnaoPath() + "/comment.cfg");
            DataOutputStream out = new DataOutputStream(fos);
            out.writeBytes(jCommentTextArea.getText());
            fos.close();
          }
          catch(IOException ioe)
          {
            log("I/O Error in prepareDeploy- " + ioe.toString());
          }          

          copyFiles(new File(localLibnaothPath() + "/libnaoth.so"), localLib);
        }
        
        if(copyExe.isSelected())
        {
          File localExe = new File(
          localDeployOutPath(i) + binPath() + "/naoth");
          if(localExe.exists())
          {
            localExe.delete();
          }
                    
          copyFiles(new File(localLibnaothPath() + "/naoth"), localExe);
        }
        
        if(copyConfig.isSelected())
        {
          String myConfigPath = localDeployOutPath(i) + configPath();
          File myConfigDir = new File(myConfigPath);
          myConfigDir.mkdirs();

          String myConfigPathIn = localDeployInPath(i, iNaoByte) + "/Config";
          File myConfigDirIn = new File(myConfigPathIn);
          myConfigDirIn.mkdirs();

          copyFiles(new File(localConfigPath()), myConfigDir);
          writePlayerCfg(new File(myConfigPath + "/private/player.cfg"), naoNumber);
          writeTeamcommCfg(new File(myConfigPath + "/private/teamcomm.cfg"), naoNumber);
          writeScheme(new File(myConfigPath + "/scheme.cfg"));
        }
      }
    }

    actionInfo("finished preparing deploy dir");

    return true;
  }//end prepareDeploy

  private String readFile(String fileName)
  {
    StringBuilder content = new StringBuilder();

    try {
      //use buffering, reading one line at a time
      //FileReader always assumes default encoding is OK!
      BufferedReader input =  new BufferedReader(new FileReader(fileName));
      try {
        String line = null; //not declared within while loop
        /*
        * readLine is a bit quirky :
        * it returns the content of a line MINUS the newline.
        * it returns null only for the END of the stream.
        * it returns an empty String if two newlines appear in a row.
        */
        while (( line = input.readLine()) != null){
          content.append(line);
          content.append("\n");
        }
      }
      finally {
        input.close();
      }
    }
    catch (IOException ex)
    {
      log("I/O Error in readFile- " + fileName + "\n" + ex.toString());
    }

    return content.toString();
  }

  private void setDirectory(String directoryName)
  {
    jDirPathLabel.setText(directoryName);
    setSchemes();
    copyButton.setEnabled(true);
    setFormData();
  }

  private void setCommentText()
  {
    String comment = "";
    if(jBackupBox.getItemCount() > 1)
    {
      comment = readFile(localDeployRootPath() + "/in/" + backups.get(jBackupBox.getSelectedItem()) + "/comment.cfg");
    }
    jCommentTextArea.setText(comment);
  }

  private void resetBackupList()
  {
    String deployInPath = localDeployRootPath() + "/in";
    File deployInDir = new File(deployInPath);
    jBackupBox.removeAllItems();
    jBackupBox.addItem("no backups to copy");
    if(deployInDir.exists())
    {
      FileFilter filter;
      filter = new FileFilter()
                {
                  public boolean accept(File f)
                  {
                      return f.isDirectory();
                  }
                  public String getDescription()
                  {
                      return "Directory";
                  }
                };

      File[] backupFiles = deployInDir.listFiles(filter);
      Arrays.sort(backupFiles);

      String[] backupItems = new String[backupFiles.length];
      
      for(int i = backupFiles.length - 1; i >= 0; i--)
      {
        //System.out.println(backups[i]);
        String entry = backupFiles[i].getName();
        Boolean entryAvaliable = false;
        String entryTag = "";

        if(copyConfig.isSelected())
        {
            File configDir = new File(localDeployRootPath() + "/in/" + entry + "/Config/");
            if(configDir.exists() && configDir.isDirectory())
            {
              entryAvaliable = true;
              entryTag += "C";
            }
        }

        if(copyLib.isSelected())
        {
            File libFile = new File(localDeployRootPath() + "/in/" + entry + "/libnaoth.so");
            if(libFile.exists() && libFile.isFile())
            {
              entryAvaliable = true;
              entryTag += "L";
            }
        }

        if(copyExe.isSelected())
        {
            File exeFile = new File(localDeployRootPath() + "/in/" + entry + "/naoth");
            if(exeFile.exists() && exeFile.isFile())
            {
              entryAvaliable = true;
              entryTag += "E";
            }
        }

        boolean error = false;

        if(entryAvaliable)
        {
          String[] split = entry.split("-");
          Long time = Long.parseLong(split[0]);
          Date date = new Date(time);
          Format formatter;
          formatter = new SimpleDateFormat("dd.MM.yyyy HH:mm:ss");
          if(split.length > 2)
          {
            backupItems[i] = "Nao " + split[2] + " (" + split[1] + ") (" + entryTag + "): " + formatter.format(date);
          }
          else if(split.length > 1)
          {
            backupItems[i] = "Nao (" + split[1] + ") (" + entryTag + "): " + formatter.format(date);
          }
          else
          {
            error = true;
          }
          if(!error)
          {
            backups.put(backupItems[i], entry);
            jBackupBox.addItem(backupItems[i]);
          }
        }
      }
      if(jBackupBox.getItemCount() > 1)
      {
        jBackupBox.setEnabled(true);
      }
      else
      {
        jBackupBox.setEnabled(false);
      }

    }
  }

  private void setFormData()
  {
    resetBackupList();

    String inString = readFile(localConfigPath() + "/scheme.cfg");
    if("".equals(inString))
    {
      inString = "Nao";
    }
    jSchemeBox.setSelectedItem(inString);

  }



  /**
   * selects the NaoController directory, populates schemes Dir
   * @param evt
   */
    private void jDirChooserPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jDirChooserPerformed
      JFileChooser chooser = new JFileChooser();
      chooser.setCurrentDirectory(new java.io.File("."));
      chooser.setDialogTitle("Select NaoController Directory");
      chooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
      chooser.setAcceptAllFileFilterUsed(false);
      if(chooser.showOpenDialog(this) == JFileChooser.APPROVE_OPTION)
      {
        setDirectory(String.valueOf(chooser.getSelectedFile()));
      }
    }//GEN-LAST:event_jDirChooserPerformed

  /**
   * populates the Schemes-Dropdown with the Schemes found in the schemedir
   *
   * @return
   */
  private boolean setSchemes()
  {
    File configPath = new File(localConfigPath());
    if( ! configPath.isDirectory())
    {
      return false;
    }

    File schemePath = new File(localConfigPath() + "/scheme");
    if( ! schemePath.isDirectory())
    {
      return false;
    }

    File files[];

    files = schemePath.listFiles();
    Arrays.sort(files);
    jSchemeBox.removeAllItems();
    for(int i = 0, n = files.length; i < n; i ++)
    {
      if(files[i].isDirectory())
      {
        jSchemeBox.addItem(files[i].getName());
      }
    }
    return true;
  }//end setSchemes

  private void actionInfo(String debugtext)
  {
    jCopyStatus.setText(debugtext);
    if(logActionInfo)
    {
      log(debugtext);
    }
  }//end actionInfo

  private void log(String logtext)
  {
    jLogWindow.append(logtext + "\n");
    jLogWindow.setCaretPosition(jLogWindow.getText().length() - 1);
    System.out.println(logtext);
  }//end log

  /**
   * recursively delete local dir
   * @param dir File dir
   */
  private void deleteDir(File dir)
  {
    if(dir.isDirectory())
    {
      File fileList[] = dir.listFiles();
      for(int index = 0; index < fileList.length; index ++)
      {
        File file = fileList[index];
        deleteDir(file);
      }
    }
    dir.delete();
  }//end deleteDir

  /**
   * recursively copy local files, skips .svn
   *
   * @param src File source
   * @param dest File destination
   */
  private void copyFiles(File src, File dest)
  {
    if( ! src.exists() ||  ! src.canRead() || src.getName().equals(".svn"))
    {
      return;
    }
    if(src.isDirectory())
    {
      if( ! dest.exists())
      {
        if( ! dest.mkdirs())
        {
          log(
          "copyFiles: Could not create direcotry: " + dest.getAbsolutePath() + ".");
          return;
        }
      }
      String list[] = src.list();
      for(int i = 0; i < list.length; i ++)
      {
        File dest1 = new File(dest, list[i]);
        File src1 = new File(src, list[i]);
        copyFiles(src1, dest1);
      }
    }
    else
    {
      try
      {
        byte[] buffer = new byte[4096]; //Buffer 4K at a time (you can change this).
        int bytesRead;

        FileInputStream fin = new FileInputStream(src);
        FileOutputStream fout = new FileOutputStream(dest);

        while((bytesRead = fin.read(buffer)) >= 0)
        {
          fout.write(buffer, 0, bytesRead);
        }
        fin.close();
        fout.close();
      }
      catch(IOException e)
      {
        e.printStackTrace();
        log("copyFiles: Unable to copy file: " + src.getAbsolutePath() + " to " + dest.
        getAbsolutePath() + ".");
      }
    }
  }

  /**
   * Creates scheme.cfg with the scheme
   * @param schemeCfg File
   */
  private void writeScheme(File schemeCfg)
  {
    
    String outString = jSchemeBox.getSelectedItem().toString();
    if(!"n/a".equals(outString))
    {
      try
      {
        BufferedWriter out = new BufferedWriter(new FileWriter(schemeCfg));
        out.write(outString);
        out.close();
      }
      catch(IOException ioe)
      {
        log("I/O Error in writeScheme- " + ioe.toString());
      }
    }
  }

  private void writePlayerCfg(File configFile, String playerNumber)
  {
    StringBuilder c = new StringBuilder();
    
    c.append("[player]\n");
    
    c.append("PlayerNumber=");
    c.append(playerNumber);
    c.append("\n");
    
    c.append("TeamNumber=");
    c.append(jTeamNumber.getText());
    c.append("\n");
    
    c.append("TeamColor=");
    c.append(jColorBox.getSelectedItem());
    c.append("\n");
    
    try
    {
      BufferedWriter out = new BufferedWriter(new FileWriter(configFile));
      out.write(c.toString());
      out.close();
    }
    catch(IOException ex)
    {
      log("I/O Error in writePlayerCfg- " + ex.getMessage());
    }
  }
  
  private void writeTeamcommCfg(File configFile, String playerNumber)
  {
    
    StringBuilder c = new StringBuilder();
    
    c.append("[teamcomm]\n");
   
    c.append("port=");
    c.append(jTeamCommPort.getText());
    c.append("\n");
    
    c.append("interface=wlan0\n");
    
    /*
    c.append("wlan=");
    c.append(lblTeamCommWLAN.getText());
    c.append("\n");
    
    c.append("lan=");
    c.append(lblTeamCommLAN.getText());
    c.append("\n");
    */
    
    try
    {
      BufferedWriter out = new BufferedWriter(new FileWriter(configFile));
      out.write(c.toString());
      out.close();
    }
    catch(IOException ex)
    {
      log("I/O Error in writeTeamcommCfg- " + ex.getMessage());
    }
  }
  

    private void jColorBoxActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jColorBoxActionPerformed
    }//GEN-LAST:event_jColorBoxActionPerformed

    private void naoByte2ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_naoByte2ActionPerformed
    }//GEN-LAST:event_naoByte2ActionPerformed

    private void naoByte1ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_naoByte1ActionPerformed
    }//GEN-LAST:event_naoByte1ActionPerformed

    private void naoByte3ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_naoByte3ActionPerformed
    }//GEN-LAST:event_naoByte3ActionPerformed

    private void lblTeamCommWLANActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_lblTeamCommWLANActionPerformed
    }//GEN-LAST:event_lblTeamCommWLANActionPerformed

    private void sshUserActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_sshUserActionPerformed
    }//GEN-LAST:event_sshUserActionPerformed

  // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
  private void initComponents() {

    jLabel12 = new javax.swing.JLabel();
    jSettingsPanel = new javax.swing.JPanel();
    jLabel3 = new javax.swing.JLabel();
    jColorBox = new javax.swing.JComboBox();
    jLabel2 = new javax.swing.JLabel();
    jTeamNumber = new javax.swing.JTextField();
    jLabel5 = new javax.swing.JLabel();
    lblTeamCommWLAN = new javax.swing.JTextField();
    jLabel9 = new javax.swing.JLabel();
    jTeamCommPort = new javax.swing.JTextField();
    jLabel10 = new javax.swing.JLabel();
    sshUser = new javax.swing.JTextField();
    jLabel11 = new javax.swing.JLabel();
    sshPassword = new javax.swing.JPasswordField();
    jLabel4 = new javax.swing.JLabel();
    jSchemeBox = new javax.swing.JComboBox();
    jButtonRefreshData = new javax.swing.JButton();
    lblTeamCommLAN = new javax.swing.JTextField();
    jLabel18 = new javax.swing.JLabel();
    jActionsPanel = new javax.swing.JPanel();
    jLabel1 = new javax.swing.JLabel();
    subnetFieldLAN = new javax.swing.JTextField();
    jLabel6 = new javax.swing.JLabel();
    naoByte1 = new javax.swing.JTextField();
    jLabel7 = new javax.swing.JLabel();
    naoByte2 = new javax.swing.JTextField();
    naoByte3 = new javax.swing.JTextField();
    jLabel8 = new javax.swing.JLabel();
    copyButton = new javax.swing.JButton();
    jDirChooser = new javax.swing.JButton();
    jSeparator1 = new javax.swing.JSeparator();
    jDirPathLabel = new javax.swing.JLabel();
    jLabel13 = new javax.swing.JLabel();
    jLabel14 = new javax.swing.JLabel();
    jLabel15 = new javax.swing.JLabel();
    jBackupBox = new javax.swing.JComboBox();
    naoByte4 = new javax.swing.JTextField();
    jLabel17 = new javax.swing.JLabel();
    jLabel16 = new javax.swing.JLabel();
    jLabel19 = new javax.swing.JLabel();
    subnetFieldWLAN = new javax.swing.JTextField();
    jScrollPane1 = new javax.swing.JScrollPane();
    jLogWindow = new javax.swing.JTextArea();
    jCopyStatus = new javax.swing.JLabel();
    copyConfig = new javax.swing.JCheckBox();
    copyLogs = new javax.swing.JCheckBox();
    noBackup = new javax.swing.JCheckBox();
    progressBar = new javax.swing.JProgressBar();
    copyLib = new javax.swing.JCheckBox();
    jScrollPane2 = new javax.swing.JScrollPane();
    jCommentTextArea = new javax.swing.JTextArea();
    cbRestartNaoth = new javax.swing.JCheckBox();
    copyExe = new javax.swing.JCheckBox();

    setDefaultCloseOperation(javax.swing.WindowConstants.EXIT_ON_CLOSE);
    setTitle("NaoSCP");

    jLabel12.setFont(new java.awt.Font("Lucida Grande", 0, 8));
    jLabel12.setText("v0.4");

    jSettingsPanel.setBackground(new java.awt.Color(204, 204, 255));

    jLabel3.setText("Color:");

    jColorBox.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "red", "blue" }));
    jColorBox.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        jColorBoxActionPerformed(evt);
      }
    });

    jLabel2.setText("TeamNr:");

    jTeamNumber.setText("13");

    jLabel5.setText("TeamComm:");

    lblTeamCommWLAN.setText("192.168.13.255");
    lblTeamCommWLAN.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        lblTeamCommWLANActionPerformed(evt);
      }
    });

    jLabel9.setText("/");

    jTeamCommPort.setText("10700");

    jLabel10.setText("ssh:");

    sshUser.setText("nao");
    sshUser.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        sshUserActionPerformed(evt);
      }
    });

    jLabel11.setText(":");

    sshPassword.setText("nao");

    jLabel4.setText("Scheme:");

    jSchemeBox.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "n/a" }));

    jButtonRefreshData.setText("Refresh");
    jButtonRefreshData.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        jButtonRefreshDataActionPerformed(evt);
      }
    });

    lblTeamCommLAN.setText("192.168.0.255");
    lblTeamCommLAN.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        lblTeamCommLANActionPerformed(evt);
      }
    });

    jLabel18.setText(":");

    org.jdesktop.layout.GroupLayout jSettingsPanelLayout = new org.jdesktop.layout.GroupLayout(jSettingsPanel);
    jSettingsPanel.setLayout(jSettingsPanelLayout);
    jSettingsPanelLayout.setHorizontalGroup(
      jSettingsPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
      .add(jSettingsPanelLayout.createSequentialGroup()
        .addContainerGap()
        .add(jSettingsPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
          .add(jLabel5)
          .add(jLabel3)
          .add(jLabel4)
          .add(jLabel10))
        .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
        .add(jSettingsPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
          .add(jSettingsPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.TRAILING, false)
            .add(org.jdesktop.layout.GroupLayout.LEADING, jSettingsPanelLayout.createSequentialGroup()
              .add(jColorBox, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 89, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
              .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
              .add(jLabel2)
              .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
              .add(jTeamNumber, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 36, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
            .add(jSettingsPanelLayout.createSequentialGroup()
              .add(jSettingsPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
                .add(jSettingsPanelLayout.createSequentialGroup()
                  .add(lblTeamCommWLAN, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 119, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
                  .add(3, 3, 3)
                  .add(jLabel9)
                  .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                  .add(lblTeamCommLAN, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
                .add(sshUser, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 231, Short.MAX_VALUE))
              .add(4, 4, 4)
              .add(jSettingsPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
                .add(jSettingsPanelLayout.createSequentialGroup()
                  .add(jLabel11)
                  .addPreferredGap(org.jdesktop.layout.LayoutStyle.UNRELATED)
                  .add(sshPassword, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 53, Short.MAX_VALUE))
                .add(jSettingsPanelLayout.createSequentialGroup()
                  .add(jLabel18)
                  .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                  .add(jSettingsPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
                    .add(jButtonRefreshData, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .add(jTeamCommPort, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 101, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))))
              .add(65, 65, 65)))
          .add(jSchemeBox, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 352, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
        .addContainerGap())
    );
    jSettingsPanelLayout.setVerticalGroup(
      jSettingsPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
      .add(org.jdesktop.layout.GroupLayout.TRAILING, jSettingsPanelLayout.createSequentialGroup()
        .add(jSettingsPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
          .add(jLabel4, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 17, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
          .add(jSchemeBox, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
        .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
        .add(jSettingsPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
          .add(jLabel3, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
          .add(jColorBox)
          .add(jTeamNumber)
          .add(jLabel2, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 17, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
          .add(jButtonRefreshData))
        .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
        .add(jSettingsPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
          .add(jLabel5, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 20, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
          .add(lblTeamCommWLAN, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
          .add(jLabel9, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 26, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
          .add(lblTeamCommLAN, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
          .add(jLabel18, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 26, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
          .add(jTeamCommPort, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
        .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
        .add(jSettingsPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
          .add(jLabel10, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 14, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
          .add(sshUser, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
          .add(jLabel11, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 26, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
          .add(sshPassword, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
        .addContainerGap())
    );

    jLabel1.setText("SubNet");

    subnetFieldLAN.setText("10.0.0");
    subnetFieldLAN.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        subnetFieldLANActionPerformed(evt);
      }
    });

    jLabel6.setText("1:");

    naoByte1.setText("-1");
    naoByte1.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        naoByte1ActionPerformed(evt);
      }
    });

    jLabel7.setText("2:");

    naoByte2.setText("-1");
    naoByte2.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        naoByte2ActionPerformed(evt);
      }
    });

    naoByte3.setText("-1");
    naoByte3.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        naoByte3ActionPerformed(evt);
      }
    });

    jLabel8.setText("3:");

    copyButton.setText("Copy");
    copyButton.setEnabled(false);
    copyButton.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        copyButtonActionPerformed(evt);
      }
    });

    jDirChooser.setText("...");
    jDirChooser.setActionCommand("jDirChoose");
    jDirChooser.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        jDirChooserPerformed(evt);
      }
    });

    jDirPathLabel.setHorizontalAlignment(javax.swing.SwingConstants.LEFT);
    jDirPathLabel.setText("SET DIRECTORY ~ /path/to/NaoTH-2011-Repository/NaoTHSoccer");
    jDirPathLabel.setToolTipText("NaoController project directory (e.g., \"D:\\NaoTH-2009\\Projects\\NaoController\")");

    jLabel13.setText("LAN:");

    jLabel14.setText("WLAN:");

    jLabel15.setText("Version to Upload:");

    jBackupBox.setEnabled(false);
    jBackupBox.addItemListener(new java.awt.event.ItemListener() {
      public void itemStateChanged(java.awt.event.ItemEvent evt) {
        jBackupBoxItemStateChanged(evt);
      }
    });

    naoByte4.setText("-1");
    naoByte4.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        naoByte4ActionPerformed(evt);
      }
    });

    jLabel17.setText("4:");

    jLabel16.setText("Comment:");

    jLabel19.setText("SubNet");

    subnetFieldWLAN.setText("192.168.13");
    subnetFieldWLAN.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        subnetFieldWLANActionPerformed(evt);
      }
    });

    org.jdesktop.layout.GroupLayout jActionsPanelLayout = new org.jdesktop.layout.GroupLayout(jActionsPanel);
    jActionsPanel.setLayout(jActionsPanelLayout);
    jActionsPanelLayout.setHorizontalGroup(
      jActionsPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
      .add(jActionsPanelLayout.createSequentialGroup()
        .add(jActionsPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
          .add(jActionsPanelLayout.createSequentialGroup()
            .addContainerGap()
            .add(jSeparator1, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 689, Short.MAX_VALUE))
          .add(jActionsPanelLayout.createSequentialGroup()
            .add(jDirChooser)
            .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
            .add(jDirPathLabel, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 665, Short.MAX_VALUE))
          .add(jActionsPanelLayout.createSequentialGroup()
            .add(496, 496, 496)
            .add(jLabel16))
          .add(jActionsPanelLayout.createSequentialGroup()
            .addContainerGap()
            .add(jActionsPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
              .add(jActionsPanelLayout.createSequentialGroup()
                .add(jActionsPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.TRAILING)
                  .add(jActionsPanelLayout.createSequentialGroup()
                    .add(jLabel13)
                    .add(27, 27, 27)
                    .add(jLabel1))
                  .add(jActionsPanelLayout.createSequentialGroup()
                    .add(jLabel14)
                    .addPreferredGap(org.jdesktop.layout.LayoutStyle.UNRELATED)
                    .add(jLabel19)))
                .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                .add(jActionsPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.TRAILING, false)
                  .add(subnetFieldWLAN)
                  .add(subnetFieldLAN, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 94, Short.MAX_VALUE))
                .add(24, 24, 24)
                .add(jLabel6)
                .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                .add(naoByte1, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 38, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                .add(jLabel7)
                .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                .add(naoByte2, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 38, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                .add(jLabel8)
                .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                .add(naoByte3, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 38, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                .add(jLabel17, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 15, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                .add(naoByte4, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 35, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
                .add(18, 18, 18)
                .add(copyButton, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 152, Short.MAX_VALUE))
              .add(jActionsPanelLayout.createSequentialGroup()
                .add(jLabel15)
                .add(18, 18, 18)
                .add(jBackupBox, 0, 553, Short.MAX_VALUE)))))
        .addContainerGap())
    );
    jActionsPanelLayout.setVerticalGroup(
      jActionsPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
      .add(jActionsPanelLayout.createSequentialGroup()
        .add(jActionsPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
          .add(jActionsPanelLayout.createSequentialGroup()
            .add(jActionsPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
              .add(jLabel1)
              .add(subnetFieldLAN, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
              .add(jLabel13))
            .add(14, 14, 14)
            .add(jActionsPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
              .add(subnetFieldWLAN, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
              .add(jLabel19)
              .add(jLabel14)))
          .add(jActionsPanelLayout.createSequentialGroup()
            .add(24, 24, 24)
            .add(jActionsPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
              .add(naoByte1, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
              .add(jLabel7)
              .add(naoByte2, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
              .add(jLabel8)
              .add(jLabel6)
              .add(naoByte3, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
              .add(jLabel17)
              .add(naoByte4, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
              .add(copyButton, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))))
        .add(18, 18, 18)
        .add(jActionsPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
          .add(jLabel15)
          .add(jBackupBox, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
        .addPreferredGap(org.jdesktop.layout.LayoutStyle.UNRELATED)
        .add(jSeparator1, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
        .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
        .add(jActionsPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
          .add(jDirChooser, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 18, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
          .add(jDirPathLabel, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 18, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
        .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
        .add(jLabel16))
    );

    jLogWindow.setColumns(20);
    jLogWindow.setEditable(false);
    jLogWindow.setFont(new java.awt.Font("Lucida Grande", 0, 10));
    jLogWindow.setRows(5);
    jScrollPane1.setViewportView(jLogWindow);

    jCopyStatus.setFont(new java.awt.Font("Lucida Grande", 0, 10));
    jCopyStatus.setText("idle...");

    copyConfig.setSelected(true);
    copyConfig.setText("copyConfig");
    copyConfig.addItemListener(new java.awt.event.ItemListener() {
      public void itemStateChanged(java.awt.event.ItemEvent evt) {
        copyConfigItemStateChanged(evt);
      }
    });

    copyLogs.setText("copyLogs");
    copyLogs.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        copyLogsActionPerformed(evt);
      }
    });

    noBackup.setText("NO BACKUP");
    noBackup.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        noBackupActionPerformed(evt);
      }
    });

    progressBar.setEnabled(false);

    copyLib.setSelected(true);
    copyLib.setText("copyLibNaoTH");
    copyLib.addItemListener(new java.awt.event.ItemListener() {
      public void itemStateChanged(java.awt.event.ItemEvent evt) {
        copyLibItemStateChanged(evt);
      }
    });

    jCommentTextArea.setColumns(15);
    jCommentTextArea.setRows(4);
    jScrollPane2.setViewportView(jCommentTextArea);

    cbRestartNaoth.setText("restartNaoTH");
    cbRestartNaoth.addItemListener(new java.awt.event.ItemListener() {
      public void itemStateChanged(java.awt.event.ItemEvent evt) {
        cbRestartNaothItemStateChanged(evt);
      }
    });
    cbRestartNaoth.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        cbRestartNaothActionPerformed(evt);
      }
    });

    copyExe.setSelected(true);
    copyExe.setText("copyNaoTH(exe)");
    copyExe.addItemListener(new java.awt.event.ItemListener() {
      public void itemStateChanged(java.awt.event.ItemEvent evt) {
        copyExeItemStateChanged(evt);
      }
    });

    org.jdesktop.layout.GroupLayout layout = new org.jdesktop.layout.GroupLayout(getContentPane());
    getContentPane().setLayout(layout);
    layout.setHorizontalGroup(
      layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
      .add(org.jdesktop.layout.GroupLayout.TRAILING, layout.createSequentialGroup()
        .addContainerGap()
        .add(layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
          .add(layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING, false)
            .add(jLabel12)
            .add(layout.createSequentialGroup()
              .add(jSettingsPanel, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 482, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
              .addPreferredGap(org.jdesktop.layout.LayoutStyle.UNRELATED)
              .add(jScrollPane2, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 219, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
            .add(jActionsPanel, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
          .add(layout.createSequentialGroup()
            .add(copyConfig)
            .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
            .add(copyLib)
            .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
            .add(copyExe)
            .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED, 43, Short.MAX_VALUE)
            .add(cbRestartNaoth)
            .addPreferredGap(org.jdesktop.layout.LayoutStyle.UNRELATED)
            .add(copyLogs)
            .addPreferredGap(org.jdesktop.layout.LayoutStyle.UNRELATED)
            .add(noBackup)))
        .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
        .add(layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
          .add(layout.createSequentialGroup()
            .add(jScrollPane1, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 239, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
            .add(12, 12, 12))
          .add(layout.createSequentialGroup()
            .add(layout.createParallelGroup(org.jdesktop.layout.GroupLayout.TRAILING)
              .add(org.jdesktop.layout.GroupLayout.LEADING, jCopyStatus, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 239, Short.MAX_VALUE)
              .add(progressBar, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 239, Short.MAX_VALUE))
            .addContainerGap())))
    );
    layout.setVerticalGroup(
      layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
      .add(layout.createSequentialGroup()
        .add(jLabel12)
        .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
        .add(layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING, false)
          .add(layout.createSequentialGroup()
            .add(jActionsPanel, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
            .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
            .add(layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING, false)
              .add(jScrollPane2)
              .add(jSettingsPanel, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)))
          .add(jScrollPane1))
        .addPreferredGap(org.jdesktop.layout.LayoutStyle.UNRELATED)
        .add(layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
          .add(layout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
            .add(copyConfig)
            .add(copyLib)
            .add(copyLogs)
            .add(cbRestartNaoth)
            .add(copyExe)
            .add(noBackup, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 23, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
          .add(layout.createSequentialGroup()
            .add(jCopyStatus)
            .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
            .add(progressBar, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)))
        .add(12, 12, 12))
    );

    pack();
  }// </editor-fold>//GEN-END:initComponents

    private void subnetFieldLANActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_subnetFieldLANActionPerformed
      // TODO add your handling code here:
    }//GEN-LAST:event_subnetFieldLANActionPerformed

    private void copyLogsActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_copyLogsActionPerformed
      // TODO add your handling code here:
}//GEN-LAST:event_copyLogsActionPerformed

    private void noBackupActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_noBackupActionPerformed
    }//GEN-LAST:event_noBackupActionPerformed

    private void jBackupBoxItemStateChanged(java.awt.event.ItemEvent evt) {//GEN-FIRST:event_jBackupBoxItemStateChanged
      
      String itemValue = evt.getItem().toString();

      if(jBackupBox.getItemCount() > 0 && jBackupBox.getSelectedItem().equals(itemValue) && !itemValue.equals(jBackupBox.getItemAt(0)))
      {
        setCommentText();
      }
    }//GEN-LAST:event_jBackupBoxItemStateChanged

    private void copyConfigItemStateChanged(java.awt.event.ItemEvent evt) {//GEN-FIRST:event_copyConfigItemStateChanged
      resetBackupList();
    }//GEN-LAST:event_copyConfigItemStateChanged

    private void copyLibItemStateChanged(java.awt.event.ItemEvent evt) {//GEN-FIRST:event_copyLibItemStateChanged
      resetBackupList();
    }//GEN-LAST:event_copyLibItemStateChanged

    private void naoByte4ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_naoByte4ActionPerformed
      // TODO add your handling code here:
    }//GEN-LAST:event_naoByte4ActionPerformed

    private void jButtonRefreshDataActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButtonRefreshDataActionPerformed
      setFormData();
    }//GEN-LAST:event_jButtonRefreshDataActionPerformed

    private void cbRestartNaothItemStateChanged(java.awt.event.ItemEvent evt) {//GEN-FIRST:event_cbRestartNaothItemStateChanged
        // TODO add your handling code here:
    }//GEN-LAST:event_cbRestartNaothItemStateChanged

    private void cbRestartNaothActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_cbRestartNaothActionPerformed
        // TODO add your handling code here:
    }//GEN-LAST:event_cbRestartNaothActionPerformed

    private void lblTeamCommLANActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_lblTeamCommLANActionPerformed
    {//GEN-HEADEREND:event_lblTeamCommLANActionPerformed
      // TODO add your handling code here:
    }//GEN-LAST:event_lblTeamCommLANActionPerformed

    private void copyExeItemStateChanged(java.awt.event.ItemEvent evt) {//GEN-FIRST:event_copyExeItemStateChanged
      // TODO add your handling code here:
    }//GEN-LAST:event_copyExeItemStateChanged

    private void subnetFieldWLANActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_subnetFieldWLANActionPerformed
      // TODO add your handling code here:
    }//GEN-LAST:event_subnetFieldWLANActionPerformed

  // Variables declaration - do not modify//GEN-BEGIN:variables
  private javax.swing.JCheckBox cbRestartNaoth;
  private javax.swing.JButton copyButton;
  private javax.swing.JCheckBox copyConfig;
  private javax.swing.JCheckBox copyExe;
  private javax.swing.JCheckBox copyLib;
  private javax.swing.JCheckBox copyLogs;
  private javax.swing.JPanel jActionsPanel;
  private javax.swing.JComboBox jBackupBox;
  private javax.swing.JButton jButtonRefreshData;
  private javax.swing.JComboBox jColorBox;
  private javax.swing.JTextArea jCommentTextArea;
  private javax.swing.JLabel jCopyStatus;
  private javax.swing.JButton jDirChooser;
  private javax.swing.JLabel jDirPathLabel;
  private javax.swing.JLabel jLabel1;
  private javax.swing.JLabel jLabel10;
  private javax.swing.JLabel jLabel11;
  private javax.swing.JLabel jLabel12;
  private javax.swing.JLabel jLabel13;
  private javax.swing.JLabel jLabel14;
  private javax.swing.JLabel jLabel15;
  private javax.swing.JLabel jLabel16;
  private javax.swing.JLabel jLabel17;
  private javax.swing.JLabel jLabel18;
  private javax.swing.JLabel jLabel19;
  private javax.swing.JLabel jLabel2;
  private javax.swing.JLabel jLabel3;
  private javax.swing.JLabel jLabel4;
  private javax.swing.JLabel jLabel5;
  private javax.swing.JLabel jLabel6;
  private javax.swing.JLabel jLabel7;
  private javax.swing.JLabel jLabel8;
  private javax.swing.JLabel jLabel9;
  private javax.swing.JTextArea jLogWindow;
  private javax.swing.JComboBox jSchemeBox;
  private javax.swing.JScrollPane jScrollPane1;
  private javax.swing.JScrollPane jScrollPane2;
  private javax.swing.JSeparator jSeparator1;
  private javax.swing.JPanel jSettingsPanel;
  private javax.swing.JTextField jTeamCommPort;
  private javax.swing.JTextField jTeamNumber;
  private javax.swing.JTextField lblTeamCommLAN;
  private javax.swing.JTextField lblTeamCommWLAN;
  private javax.swing.JTextField naoByte1;
  private javax.swing.JTextField naoByte2;
  private javax.swing.JTextField naoByte3;
  private javax.swing.JTextField naoByte4;
  private javax.swing.JCheckBox noBackup;
  private javax.swing.JProgressBar progressBar;
  private javax.swing.JPasswordField sshPassword;
  private javax.swing.JTextField sshUser;
  private javax.swing.JTextField subnetFieldLAN;
  private javax.swing.JTextField subnetFieldWLAN;
  // End of variables declaration//GEN-END:variables

  class Copier extends SwingWorker<Boolean, File>
  {

    private String Ip;
    private int Number;
    private int iNaoByte;
    private Session session;

    public Copier(String Ip, int Number, int iNaoByte)
    {
      this.Ip = Ip;
      this.Number = Number;
      this.iNaoByte = iNaoByte;
    }

    protected Boolean doInBackground()
    {
      try
      {
        if(session == null || !session.isConnected())
          session = naoSsh(Ip);
        
        if(debugVersion && noBackup.isSelected())
        {
          // NO BACKUP!!!
          jLogWindow.setBackground(Color.pink);
          minimalBackupNao(session, Number, iNaoByte);
        }
        else
        {
          jLogWindow.setBackground(Color.white);
          backupNao(session, Number, iNaoByte);
        }
        writeNao(session, Number);
        restartNaoTH(session, Number);
        actionInfo("Done with Nao " + Number);
        copyIsDone(Number);
      }
      catch(Exception e)
      {
        log(e.toString());
        haveError(Number, e.toString());
        copyIsDone(Number);
        return false;
      }
      return true;
    }

    private void backupNao(Session session, int Number, int iNaoByte)
    {
      String exceptionHelper = "during backup init";
      actionInfo("Starting Backup Session for Nao " + String.valueOf(Number));
      try
      {
        Channel channel = session.openChannel("sftp");
        channel.connect();
        ChannelSftp c = (ChannelSftp) channel;

        // copy a backup
        if(jBackupBox.getSelectedIndex() != 0)
        {
          if(copyLib.isSelected())
          {
            File libFile = new File(localDeployRootPath() + "/in/" + backups.get(jBackupBox.getSelectedItem()) + "/libnaoth.so");
            if(!libFile.exists() || !libFile.isFile())
            {
              log("selected backup contains no libnaoth.so file");
              copyLib.setSelected(false);
            }
          }

          if(copyExe.isSelected())
          {
            File libFile = new File(localDeployRootPath() + "/in/" + backups.get(jBackupBox.getSelectedItem()) + "/libnaoth.so");
            if(!libFile.exists() || !libFile.isFile())
            {
              log("selected backup contains no libnaoth.so file");
              copyLib.setSelected(false);
            }
          }

          if(copyConfig.isSelected())
          {
            File exeDir = new File(localDeployRootPath() + "/in/" + backups.get(jBackupBox.getSelectedItem()) + "/naoth");
            if(!exeDir.exists() || !exeDir.isDirectory())
            {
              log("selected backup contains no Config directory");
              copyConfig.setSelected(false);
            }
          }
        }

        try
        {
          if(copyLib.isSelected() || copyExe.isSelected() || copyConfig.isSelected())
          {
            exceptionHelper = "mkdir local " + localDeployInPath(Number, iNaoByte);
            File backup = new File(localDeployInPath(Number, iNaoByte));
            backup.mkdirs();
          }

          if(copyLib.isSelected())
          {
            actionInfo("get libnaoth.so");

            exceptionHelper = "get from robot " + remoteRootPath(Number) + libnaoPath() + "/libnaoth.so to "
                            + localDeployInPath(Number, iNaoByte) + "/libnaoth.so";
            c.get
            (
              remoteRootPath(Number) + libnaoPath() + "/libnaoth.so",
              new FileOutputStream(localDeployInPath(Number, iNaoByte) + "/libnaoth.so"),
              new MyProgressMonitor(progressBar)
            );
            
            c.get
            (
              remoteRootPath(Number) + libnaoPath() + "/comment.cfg",
              new FileOutputStream(localDeployInPath(Number, iNaoByte) + "/comment.cfg"),
              new MyProgressMonitor(progressBar)
            );
          }

          if(copyExe.isSelected())
          {
            actionInfo("get naoth (exe)");
            c.get
            (
              remoteRootPath(Number) + binPath() + "/naoth",
              new FileOutputStream(localDeployInPath(Number, iNaoByte) + "/naoth"),
              new MyProgressMonitor(progressBar)
            );
            
          }
        }
        catch(Exception e)
        {
          actionInfo("Error - Nao " + String.valueOf(Number) + ": " + e.toString());
          log("Exception in backupNao (" + String.valueOf(Number) + "): " + e.toString() + "(" + exceptionHelper + ")");
        }

        try
        {
          if(copyConfig.isSelected())
          {
            exceptionHelper = "get recursive from robot " + remoteRootPath(Number) + configPath() + " to "
                            + localDeployInPath(Number, iNaoByte) + "/Config";
            recursiveSftpGet(c, localDeployInPath(Number, iNaoByte) + "/Config", remoteRootPath(Number) + configPath());
          }
        }
        catch(Exception e)
        {
          actionInfo("Error - Nao " + String.valueOf(Number) + ": " + e.toString());
          log("Exception in backupNao (" + String.valueOf(Number) + "): " + e.toString() + "(" + exceptionHelper + ")");
        }

        try
        {
          if(copyLogs.isSelected())
          {
            exceptionHelper = "get recursive from robot " + remoteRootPath(Number) + "/tmp/nao.log" + " to "
                            + localDeployInPath(Number, iNaoByte) + "/nao.log";
            recursiveSftpGet(c, localDeployInPath(Number, iNaoByte) + "/nao.log", remoteRootPath(Number) + "/tmp/nao.log");
          }
        }
        catch(Exception e)
        {
          actionInfo("Error - Nao " + String.valueOf(Number) + ": " + e.toString());
          log("Exception in backupNao (" + String.valueOf(Number) + "): " + e.toString() + "(" + exceptionHelper + ")");
        }

        channel.disconnect();
      }
      catch(Exception e)
      {
        actionInfo("Error - Nao " + String.valueOf(Number) + ": " + e.toString());
        log("Exception in backupNao (" + String.valueOf(Number) + "): " + e.toString() + "(" + exceptionHelper + ")");
      }
    }

    private void minimalBackupNao(Session session, int Number, int iNaoByte)
    {
      String exceptionHelper = "during backup init";
      actionInfo("Starting Backup Session for Nao " + String.valueOf(Number));
      try
      {
        String myConfigPathIn = localDeployInPath(Number, iNaoByte) + "/MinimalConfig/";
        File myConfigDirIn = new File(myConfigPathIn);
        myConfigDirIn.mkdirs();

        Channel channel = session.openChannel("sftp");
        channel.connect();
        ChannelSftp c = (ChannelSftp) channel;
        exceptionHelper = "get recursive minimal backup from robot " + remoteRootPath(Number) + configPath() + " to "
                            + localDeployInPath(Number, iNaoByte) + "/Config";
        String regex = "[0-9A-Za-z]{2}_[0-9A-Za-z]{2}_[0-9A-Za-z]{2}_[0-9A-Za-z]{2}_[0-9A-Za-z]{2}_[0-9A-Za-z]{2}";
        regexRecursiveSftpGet(c, myConfigPathIn,
                        remoteRootPath(Number) + configPath(),
                        regex);
      }
      catch(Exception e)
      {
        actionInfo("Error - Nao " + String.valueOf(Number) + ": " + e.toString());
        log("Exception in backupNao (" + String.valueOf(Number) + "): " + e.toString() + "(" + exceptionHelper + ")");
      }
    }


    public boolean testConnection()
    {
      boolean state = false;
      try
      {
        InetAddress naoIp = InetAddress.getByName(Ip);
        actionInfo("Try to reach " + Ip);
        if(naoIp.isReachable(2500))
        {
          Session session = naoSsh(Ip);
          Channel channel = session.openChannel("sftp");
          channel.connect();
          ChannelSftp c = (ChannelSftp) channel;
          state = c.isConnected();
          c.disconnect();
        }
        else
        {
          actionInfo(Ip + " unreachable");
        }
      }
      catch(Exception e)
      {
      }
      return state;
    }

    /**
     * copy new files to nao
     *
     * @param session jsch session
     * @param Number nao number
     */
    private boolean writeNao(Session session, int Number)
    {
      actionInfo("initialization writing part");
      try
      {
        Channel channel = session.openChannel("sftp");
        channel.connect();
        ChannelSftp c = (ChannelSftp) channel;

        String localLibPath = localDeployOutPath(Number) + libnaoPath() + "/";
        String remoteLibPath = remoteRootPath(Number) + libnaoPath() + "/";
        
        String localBinPath = localDeployOutPath(Number) + binPath() + "/";
        String remoteBinPath = remoteRootPath(Number) + binPath() + "/";

        String localConfigPath = localDeployOutPath(Number) + configPath();
        String remoteConfigPath = remoteRootPath(Number) + configPath();

        if(!jBackupBox.getSelectedItem().equals(jBackupBox.getItemAt(0)))
        {
          if(copyLib.isSelected())
          {
            File libFile = new File(localDeployRootPath() + "/in/" + backups.get(jBackupBox.getSelectedItem()) + "/libnaoth.so");
            if(libFile.exists() && libFile.isFile())
            {
              localLibPath = localDeployRootPath() + "/in/" + backups.get(jBackupBox.getSelectedItem()) + "/";
              log("writing libnaoth.so file from Backup " + jBackupBox.getSelectedItem() + " to Nao " + Number);
            }
            else
            {
              log("selected backup contains no libnaoth.so file " + localLibPath);
              copyLib.setSelected(false);
            }
          }
        }

        if(copyLib.isSelected())
        {
          try
          {
            c.rm(remoteRootPath(Number) + libnaoPath() + "libnaoth.so");
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
            c.rm(remoteRootPath(Number) + libnaoPath() + "comment.cfg");
          }
          catch(SftpException ex)
          {
            // if the file is not there its ok
            if(ex.id != ChannelSftp.SSH_FX_NO_SUCH_FILE)
            {
              throw ex;
            }
          }//end try

          actionInfo("put libnaoth.so");
          c.put
            (
              localLibPath + "libnaoth.so",
              remoteLibPath + "libnaoth.so",
              new MyProgressMonitor(progressBar)
            );          
        }

        if(copyExe.isSelected())
        {
          try
          {
            c.rm(remoteRootPath(Number) + binPath() + "/naoth");
          }
          catch(SftpException ex)
          {
            // if the file is not there its ok
            if(ex.id != ChannelSftp.SSH_FX_NO_SUCH_FILE)
            {
              throw ex;
            }
          }//end try

          actionInfo("put naoth (exe)");

          c.put
            (
              localBinPath + "naoth",
              remoteBinPath + "naoth",
              new MyProgressMonitor(progressBar)
            );
          // 770
          c.chmod(504, remoteBinPath + "naoth");

        }
        
        if(!jBackupBox.getSelectedItem().equals(jBackupBox.getItemAt(0)))
        {
          if(copyConfig.isSelected())
          {
            File configDir = new File(localDeployRootPath() + "/in/" + backups.get(jBackupBox.getSelectedItem()) + "/Config/");
            if(configDir.exists() && configDir.isDirectory())
            {
              localConfigPath = configDir.getAbsolutePath();
              log("writing Config directory from Backup " + jBackupBox.getSelectedItem() + " to Nao " + Number);
            }
            else
            {
              log("selected backup contains no Config directory " + localConfigPath);
              copyConfig.setSelected(false);
            }
          }
        }

        if(copyConfig.isSelected())
        {
          File localConfigFiles = new File(localConfigPath);

          rmDirSftp(c, remoteRootPath(Number) + configPath() + "/general");
          rmDirSftp(c, remoteRootPath(Number) + configPath() + "/robots");
          rmDirSftp(c, remoteRootPath(Number) + configPath() + "/scheme");
          rmDirSftp(c, remoteRootPath(Number) + configPath() + "/private");

          // create if it is not existing
          String remooteConfigDst = remoteConfigPath;
          //c.mkdir(remooteConfigDst);

          recursiveSftpPut(c, localConfigFiles, remooteConfigDst);
        }
        channel.disconnect();
      }
      catch(Exception e)
      {
        log("Exception in writeNao (" + String.valueOf(Number) + "): " + e.toString());
        return false;
      }
      return true;
    }


    /**
     * copy new files to nao
     *
     * @param session jsch session
     * @param Number nao number
     */
    private boolean restartNaoTH(Session session, int Number)
    {
      actionInfo("initialization writing part");
      try
      {
         if (cbRestartNaoth.isSelected())
         {
            Channel cmd = session.openChannel("exec");
            
            actionInfo("restarting naoqi");
            ((ChannelExec) cmd).setCommand("/etc/init.d/naoqi restart");
            
            actionInfo("waiting");
            
            Thread.sleep(5000);
            
            actionInfo("restarting naoth cognition process");
            ((ChannelExec) cmd).setCommand("/etc/init.d/naoth stop");
            ((ChannelExec) cmd).setCommand("/etc/init.d/naoth start");
            InputStream consoleOut = cmd.getInputStream();
            cmd.connect();
            byte[] buffer = new byte[4096];
            while(true) {
               if(consoleOut.available()>0) {
                  int ret = consoleOut.read(buffer, 0, 4096);
                  if(ret<0) {
                     break;
                  }
                  //System.out.println(buffer);
               }
               if(cmd.isClosed()) {
                  break;
               }
               try{Thread.sleep(100);}catch(Exception threadEx){}
            }
            cmd.disconnect();
          }
        }
        catch(Exception e)
        {
          actionInfo("Error - Nao " + String.valueOf(Number) + ": " + e.toString());
          log("Exception in restartNaoQi (" + String.valueOf(Number) + ")");
          return false;
        }
        return true;
    }

    /**
     * returns ssh-session object
     * @param Ip robot ip
     * @return session
     * @throws com.jcraft.jsch.JSchException
     */
    private Session naoSsh(String Ip) throws JSchException
    {
      actionInfo("Trying to connect to " + Ip);
      UserInfo ui = new MyUserInfo();
      java.util.Properties config = new java.util.Properties();
      config.put("StrictHostKeyChecking", "no");
      JSch jsch = new JSch();
      Session session = jsch.getSession(sshUser.getText(), Ip, 22);
      session.setConfig(config);
      session.setUserInfo(ui);
      session.connect();

      return session;
    }

    /**
     * rm -r via sftp
     * @param c ChannelSftp Object
     * @param dstDir String directory to delete
     */
    private void rmDirSftp(ChannelSftp c, String dstDir)
    {
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
                  rmDirSftp(c, dstDir + "/" + lsEntry.getFilename());
                  c.rmdir(dstDir + "/" + lsEntry.getFilename());
                }
                else
                {
                  actionInfo("remote rm " + lsEntry.getFilename());

                  c.rm(dstDir + "/" + lsEntry.getFilename());
                }
              }
            }
          }
        }
      }
      catch(Exception e)
      {
        log("Exception in rmDirSftp: (" + dstDir + ")" + e.toString());
      }
    }

    /**
     * Recursively get Directory via Sftp
     * @param c ChannelSftp Object
     * @param local String Local Location (a.k.a destination)
     * @param remote String Remote Location
     */
    private void recursiveSftpGet(ChannelSftp c, String local, String remote)
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
                  recursiveSftpGet(c, outputFileName, remote + "/" + lsEntry.
                  getFilename());
                }
                else
                {
                  actionInfo("get " + lsEntry.getFilename());
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
        log("Exception in recursiveSftpGet: " + e.toString());
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
    private void regexRecursiveSftpGet(ChannelSftp c, String local, String remote, String pattern)
    {
      try
      {
        if(remote.endsWith("/"))
        {
          remote = remote.substring(0, remote.length() - 1);
        }
        File target = new File(local);
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
                  regexRecursiveSftpGet(c, outputFileName, remote + "/" + lsEntry.getFilename(), pattern);
                } else if(match)
                {
                  File f2 = new File(f.getParent());
                  if(!f2.exists()) {
                      f2.mkdirs();
                  }
                  actionInfo("get " + lsEntry.getFilename());
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
        log("Exception in recursiveSftpGet: " + e.toString());
      }
    }

    /**
     * recursive Put via sftp
     * @param c ChannelSftp Object
     * @param src File Local Sources
     * @param dst String Remote Destination
     */
    private void recursiveSftpPut(ChannelSftp c, File src, String dst)
    {
      try
      {
        File files[] = src.listFiles();
        Arrays.sort(files);
        for(int i = 0, n = files.length; i < n; i ++)
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
              log("mkdir exception (" + newdst + ")");
            }
            recursiveSftpPut(c, files[i], newdst);
          }
          else
          {
            actionInfo("put " + files[i].getName());
            c.put(files[i].getAbsolutePath(), dst + "/" + files[i].getName());
          }

        }
      }
      catch(Exception e)
      {
        log("Exception in recursiveSftpPut: " + e.toString());
      }
    }

    public class MyUserInfo implements UserInfo
    {

      public String getPassword()
      {
        return sshPassword.getText();
      }

      public boolean promptYesNo(String str)
      {
        return true;
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
        return true;
      }

      public void showMessage(String message)
      {
      }
    }
  }

  class MyProgressMonitor implements SftpProgressMonitor
  {

    long count = 0;
    long max = 0;
    private long percent = -1;
    private JProgressBar progressbar;

    public MyProgressMonitor(JProgressBar progressbar)
    {
      this.progressbar = progressbar;
    }

    public void init(int op, String src, String dest, long max)
    {
      this.max = max;
      progressbar.setIndeterminate(false);
      progressbar.setValue(0);
      progressbar.setMinimum(0);
      progressbar.setMaximum(100);
      progressbar.setVisible(true);
      progressbar.setEnabled(true);
    }

    public boolean count(long count)
    {
      this.count += count;
      percent = this.count * 100 / max;
      progressbar.setValue((int) percent);
      return true;
    }

    public void end()
    {
      progressbar.setVisible(false);
      progressbar.setEnabled(false);
    }
  }//end MyProgressMonitor
}
