package de.naoth.naoscp;

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


import java.awt.Color;
import java.awt.Component;
import java.io.*;
import java.net.Inet4Address;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.URLDecoder;
import java.text.Format;
import java.text.SimpleDateFormat;
import java.util.*;
import java.util.Collections;
import javax.jmdns.JmDNS;
import javax.jmdns.ServiceEvent;
import javax.swing.*;
import javax.swing.text.BadLocationException;
import javax.swing.text.Style;
import javax.swing.text.StyledDocument;


public class NaoScp extends NaoScpMainFrame
{
  // if true, all ActionInfo is sent to log
  // @see actionInfo()
  private boolean logActionInfo = true;
//  private boolean debugVersion = true;
  
  private HashMap<String, JTextField> networkConfigTags = new HashMap<String, JTextField>();

  private HashMap<Integer, JTextField> naoNumberFields = new HashMap<Integer, JTextField>();
  private HashMap<Integer, Boolean> scriptDone = new HashMap<Integer, Boolean>();
  private HashMap<Integer, Boolean> copyDone = new HashMap<Integer, Boolean>();
  private HashMap<Integer, Boolean> hadCopyErrors = new HashMap<Integer, Boolean>();
  private HashMap<Integer, Boolean> hadScriptErrors = new HashMap<Integer, Boolean>();
  private HashMap<Integer, Boolean> hadErrors = new HashMap<Integer, Boolean>();
  private HashMap<Integer, Integer> iNaoBytes = new HashMap<Integer, Integer>();
  private HashMap<Integer, String> sNaoLanIps = new HashMap<Integer, String>();
  private HashMap<Integer, String> sNaoWLanIps = new HashMap<Integer, String>();

  private ArrayList<JmDNS> jmdnsList = new ArrayList<JmDNS>();
//  private ArrayList<JmdnsServiceListener> jmdnsServiceListenerList = new ArrayList<JmdnsServiceListener>();
  private HashMap<String, ArrayList<InetAddress>> hostAdresses = new HashMap<String, ArrayList<InetAddress>>();
  private Map<String, NaoSshWrapper> services;
  private final DefaultListModel naoModel;
  
  private boolean showCopyDoneMsg = false;
  private boolean showScriptDoneMsg = false;
  private boolean showDoneMsg = true;
  
  private String setupPlayerNo;
  private String lastBashColorOption;
  
  @SuppressWarnings("unchecked")
  public NaoScp()
  {
    String laf = "javax.swing.plaf.metal.MetalLookAndFeel";
    try 
    {
        UIManager.setLookAndFeel(laf);
    }
    catch (Exception e) 
    {
        e.printStackTrace();
        System.exit(1);
    }

    initComponents();

    wlanBtnGroup.add(radioWPA);
    wlanBtnGroup.add(radioWEP);

    lastBashColorOption = "bash_0";
    logTextPane.setEditorKit(new WrapEditorKit());
    StyledDocument doc = logTextPane.getStyledDocument();
    addStylesToDocument(doc);

    config = new NaoScpConfig();
    config.debugVersion = true;

    updateConfig();

    this.cbNoBackup.setVisible(config.debugVersion);
    this.cbNoBackup.setEnabled(config.debugVersion);

    this.naoNumberFields.put(0, new JTextField());
    this.scriptDone.put(0, false);
    this.copyDone.put(0, false);
    this.hadCopyErrors.put(0, false);
    this.hadScriptErrors.put(0, false);
    this.hadErrors.put(0, false);
    this.sNaoLanIps.put(0, "");
    this.sNaoWLanIps.put(0, "");
    this.iNaoBytes.put(0, -1);    
    
    Component[] allComps = jActionsPanel.getComponents();
    for(int c = 0; c < allComps.length; c++)
    {
      String name = allComps[c].getName();
      if(name != null && allComps[c].getClass() == JTextField.class)
      {
        Integer naoNo = Integer.parseInt(name.replaceAll("[a-zA-Z]+", ""));
        this.naoNumberFields.put(naoNo, (JTextField) allComps[c]);
        this.scriptDone.put(naoNo, false);
        this.copyDone.put(naoNo, false);
        this.hadCopyErrors.put(naoNo, false);
        this.hadScriptErrors.put(naoNo, false);
        this.hadErrors.put(naoNo, false);
        this.sNaoLanIps.put(naoNo, "");
        this.sNaoWLanIps.put(naoNo, "");
        this.iNaoBytes.put(naoNo, -1);
      }
    }
    
    this.networkConfigTags.put("LAN_IP", subnetFieldLAN);
    this.networkConfigTags.put("LAN_BRD", broadcastFieldLAN);
    this.networkConfigTags.put("LAN_MASK", netmaskFieldLAN);
    this.networkConfigTags.put("WLAN_IP", subnetFieldWLAN);
    this.networkConfigTags.put("WLAN_BRD", broadcastFieldWLAN);
    this.networkConfigTags.put("WLAN_MASK", netmaskFieldWLAN);

    naoModel = new DefaultListModel();
    lstNaos.setModel(naoModel);
        
    services = new HashMap<String, NaoSshWrapper>();
    
//    try
//    {      
//      //get own interface ip addresses
//      for (Enumeration<NetworkInterface> en = NetworkInterface.getNetworkInterfaces(); en.hasMoreElements();) 
//      {
//        NetworkInterface intf = en.nextElement();
//        if(!intf.isLoopback() && intf.isUp())
//        {
//          ArrayList<InetAddress> interfaceAdresses = new ArrayList<InetAddress>();
//          for (Enumeration<InetAddress> enumIpAddr = intf.getInetAddresses(); enumIpAddr.hasMoreElements(); ) 
//          {
//            InetAddress addr = enumIpAddr.nextElement();
//            {
//              interfaceAdresses.add(addr);
//            }
//          }
//          hostAdresses.put(intf.getName(), interfaceAdresses);
//        }
//      }
//     
//      //hook jmdns service for each address
//      for(String intf : hostAdresses.keySet())
//      {
//        if(hostAdresses.get(intf).size() > 0)
//        {
//          JmDNS j = JmDNS.create(hostAdresses.get(intf).get(0), intf);
//          jmdnsList.add(j);
//          int idx = jmdnsList.indexOf(j);
//          JmdnsServiceListener listener = new JmdnsServiceListener(idx);
//          jmdnsList.get(idx).addServiceListener("_nao._tcp.local.", listener);
////          jmdnsServiceListenerList.add(listener);
//        }
//      }
//    }
//    catch (IOException ex)
//    {
//    }
       
    String value = System.getenv("NAOTH_BZR");
    
    if(value != null)
    {
      value = value + "/NaoTHSoccer";
      setDirectory(value);
    }
    else
    {
      try
      {
        String ResourceName="de/naoth/naoscp/NaoScp.class";
        String programPath = URLDecoder.decode(this.getClass().getClassLoader().getResource(ResourceName).getPath(), "UTF-8");
        programPath = programPath.replace("file:", "");
        //path replacement if NaoScp is being started from console directly
        programPath = programPath.replace("/NaoScp/dist/NaoScp_2.jar!/de/naoth/naoscp/NaoScp.class", "");
        //path replacement if NaoScp is started from IDE (Netbeans)
        programPath = programPath.replace("/NaoScp/build/classes/de/naoth/naoscp/NaoScp.class", "") + "/NaoTHSoccer";
        File ProgramDir = new File(programPath);
        if(ProgramDir.exists())
        {
          setDirectory(ProgramDir.getAbsolutePath());
        }
      }
      catch(UnsupportedEncodingException ueEx)
      {
        actionInfo("could not determine current work directory\n" + ueEx.getMessage());
      }
    }

    if(config.fhIsTesting)
    {
      config.remotePrefix = "/Users/robotest/deploy";
      subnetFieldLAN.setText("127.0.0");
      sshUser.setText("robotest");
      sshPassword.setText("robotest");
    }
    setActionBtnLabel();
  }

  public void addJmdnsListenerService(final ServiceEvent event, final int idx)
  {   
    SwingUtilities.invokeLater
    (
      new Runnable() 
      {
        public void run() 
        {
          jmdnsList.get(idx).requestServiceInfo(event.getType(), event.getName(), 1);
        }
      }
    );
  }
  
  public void resolveJmdnsListenerService(ServiceEvent event, int idx)
  {
    services.put(jmdnsList.get(idx).getName() + "_nao", new NaoSshWrapper(event.getInfo(), jmdnsList.get(idx).getName() + "_nao"));
    updateList();
  }
  
  public void removeJmdnsListenerService(ServiceEvent event, int idx)
  {
    services.remove(jmdnsList.get(idx).getName() + "_nao");
    updateList();
  }
  
  public void updateList()
  {
    synchronized (naoModel)
    {
      naoModel.clear();
      for(Map.Entry<String, NaoSshWrapper> entry : services.entrySet())
      {
        if (entry.getValue().isValid())
        {
          naoModel.addElement(entry.getValue());
        }
      }

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
  
  private void readNetworkConfig() throws FileNotFoundException, IOException
  {
    FileReader fReader = new FileReader(config.localSetupScriptPath() + "/network.conf");
    BufferedReader bReader = new BufferedReader(fReader);

    actionInfo("reading NaoConfigFiles/network.conf");
    String line = bReader.readLine();
    while(line != null)
    {
      String[] param = line.split("=");
      if(param.length == 2)
      {
        String key = param[0];
        String val = param[1];
        if (this.networkConfigTags.containsKey(key))
        {
          String[] value = val.split("\\.");
          if(value.length > 2)
          {
            this.networkConfigTags.get(key).setText(val);
          }
        }
      }          
      line = bReader.readLine();          
    }
    fReader.close();
  }
  
  private void writeNetworkConfig() throws IOException
  {
    File file = new File(config.localSetupScriptPath() + "/network.conf");
    if(!file.isFile())
    {
      file.createNewFile();
    }

    FileWriter fWriter = new FileWriter(config.localSetupScriptPath() + "/network.conf");

    if(subnetFieldLAN.getText().endsWith("."))
    {
      fWriter.write("LAN_IP=" + subnetFieldLAN.getText() + "\n");
    }
    else
    {
      fWriter.write("LAN_IP=" + subnetFieldLAN.getText() + ".\n");
    }
    fWriter.write("LAN_BRD=" + broadcastFieldLAN.getText() + "\n");
    fWriter.write("LAN_MASK=" + netmaskFieldLAN.getText() + "\n");

    if(subnetFieldWLAN.getText().endsWith("."))
    {
      fWriter.write("WLAN_IP=" + subnetFieldWLAN.getText() + "\n");
    }
    else
    {
      fWriter.write("WLAN_IP=" + subnetFieldWLAN.getText() + ".\n");
    }
    fWriter.write("WLAN_BRD=" + broadcastFieldWLAN.getText() + "\n");
    fWriter.write("WLAN_MASK=" + netmaskFieldWLAN.getText() + "\n");

    fWriter.close();
  }
  
  private void initNetworkConfig()
  {
    try
    {    
      try
      {      
        readNetworkConfig();
      }
      catch(FileNotFoundException fnfEx)
      {
        writeNetworkConfig();
      }
    }
    catch(IOException ioEx)
    {
      actionInfo("network config file not read and writeable");
    }
    
  }
  
  private boolean checkNaoIps(Boolean init)
  {
    int naoIpCount = 0;
    for(Integer naoNo : this.iNaoBytes.keySet())
    {
      if(naoNo > 0)
      {
        iNaoBytes.put(naoNo, Integer.parseInt(naoNumberFields.get(naoNo).getText()));
      }
      else
      {
        if(!init && naoNo == 0)
        {
          iNaoBytes.put(naoNo, -1);
        }
      }
    }
    
    for(Integer naoNo : this.sNaoLanIps.keySet())
    {
      if(naoNo > 0 )
      {
        if(iNaoBytes.get(naoNo) < 256 && iNaoBytes.get(naoNo) > -1 )
        {
          if(subnetFieldLAN.getText().endsWith("."))
          {
            sNaoLanIps.put(naoNo, subnetFieldLAN.getText() + iNaoBytes.get(naoNo));
          }
          else
          {
            sNaoLanIps.put(naoNo, subnetFieldLAN.getText() + "." + iNaoBytes.get(naoNo));
          }
          if(subnetFieldWLAN.getText().endsWith("."))
          {
            sNaoWLanIps.put(naoNo, subnetFieldWLAN.getText() + iNaoBytes.get(naoNo));
          }
          else
          {
            sNaoWLanIps.put(naoNo, subnetFieldWLAN.getText() + "." + iNaoBytes.get(naoNo));
          }
          naoIpCount++;
        }
        else
        {
          sNaoLanIps.put(naoNo, "");
          sNaoWLanIps.put(naoNo, "");
        }
      }
      else
      {
        sNaoLanIps.put(naoNo, "");
        sNaoWLanIps.put(naoNo, "");
      }
    }
    return naoIpCount > 0;
  }

  private void copyFiles2Nao()
  {
    clearLog();
    setFormEnabled(false);        
    NaoScpConfig cfg = new NaoScpConfig(config);
    cfg.addresses.clear();
    cfg.copyConfig = cbCopyConfig.isSelected();
    cfg.copyLib = cbCopyLib.isSelected();
    cfg.copyExe = cbCopyExe.isSelected();
    cfg.copyLogs = cbCopyLogs.isSelected();
    cfg.restartNaoqi = cbRestartNaoqi.isSelected();
    cfg.restartNaoth = cbRestartNaoth.isSelected();
    cfg.noBackup = cbNoBackup.isSelected();
    cfg.forceBackup = cbForceBackup.isSelected();
    if(cfg.backupIsSelected)
    {
      cfg.boxSelected = jBackupBox.getSelectedItem().toString();
      cfg.selectedBackup = cfg.backups.get(jBackupBox.getSelectedItem()).toString();
    }
    else
    {
      cfg.boxSelected = "";
      cfg.selectedBackup = "";
    }

    if(cfg.copyConfig || cfg.copyExe || cfg.copyLib || cfg.copyLogs || cfg.forceBackup || cfg.noBackup)
    {
      showCopyDoneMsg = true;
    }

    if(cfg.restartNaoth || cfg.restartNaoqi)
    {
      if(showCopyDoneMsg)
      {
        showCopyDoneMsg = false;
        showScriptDoneMsg = false;
        showDoneMsg = true;
      }
      else
      {
        showScriptDoneMsg = true;
        showDoneMsg = false;
      }
    }
    if(cfg.debugVersion && cfg.noBackup)
    {
      logTextPane.setBackground(Color.PINK);
    }
    else
    {
      logTextPane.setBackground(Color.WHITE);
    }

    if(!checkNaoIps(false))
    {
      actionInfo("[0;31mNo Nao has a valid ip address specified\n[0m");
      setFormEnabled(true);
      return;
    }
    
    cfg.backupTimestamp = String.valueOf(System.currentTimeMillis());
    prepareDeploy(cfg);
   
    for(Integer naoNo : copyDone.keySet())
    {
      if(naoNo > 0)
      {
        scriptDone.put(naoNo, false);
        copyDone.put(naoNo, false);
      }
      hadCopyErrors.put(naoNo, false);
      hadScriptErrors.put(naoNo, false);
      hadErrors.put(naoNo, false);
    }
    copyDone.put(0, true);
    scriptDone.put(0, true);

    for(Integer naoNo : sNaoLanIps.keySet())
    {
      if(!sNaoLanIps.get(naoNo).equals(""))
      {
        NaoScpConfig naoCfg = new NaoScpConfig(cfg);        
        naoCfg.addresses.add(sNaoLanIps.get(naoNo));
        naoCfg.addresses.add(sNaoWLanIps.get(naoNo));

        if(!showCopyDoneMsg && showScriptDoneMsg)
        {
          if (naoCfg.restartNaoqi)
          {
            remoteScriptRunner scriptRunner = new remoteScriptRunner(naoCfg, String.valueOf(naoNo), String.valueOf(iNaoBytes.get(naoNo)), "restartNaoqi", false);
            scriptRunner.execute();
          }
          else if(naoCfg.restartNaoth)
          {
            remoteScriptRunner scriptRunner = new remoteScriptRunner(naoCfg, String.valueOf(naoNo), String.valueOf(iNaoBytes.get(naoNo)), "restartNaoTH", false);
            scriptRunner.execute();
          }
          else
          {
            allIsDone(naoNo);
          }
        }
        else
        {
          remoteCopier copier = new remoteCopier(naoCfg, String.valueOf(naoNo), String.valueOf(iNaoBytes.get(naoNo)));
          copier.execute();
        }
      }
      else
      {
        allIsDone(naoNo);
      }
    }
  }

  private boolean checkDirPath(boolean verbose)
  {
    if(config == null || config.jDirPathLabel == null)
    {
      if(verbose)
      {
        JOptionPane.showMessageDialog(null, "Main Directory not set");
      }
      return false;
    }
    return true;
  }

    private void copyButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_copyButtonActionPerformed
      if(checkDirPath(true))
      {
        copyFiles2Nao();
        resetBackupList();
      }
    }//GEN-LAST:event_copyButtonActionPerformed

  public void haveError(String sNaoNo, String error)
  {
     haveError(Integer.parseInt(sNaoNo), error);
  }
  
  public void haveError(int naoNo, String error)
  {
    hadErrors.put(naoNo, true);
    actionInfo(error);
  }
  
  public void haveCopyError(String sNaoNo, String error)
  {
     haveCopyError(Integer.parseInt(sNaoNo), error);
  }
  
  public void haveCopyError(int naoNo, String error)
  {
    hadCopyErrors.put(naoNo, true);
    haveError(naoNo, "[041mcopy error: " + error + "\n[0m");
  }

  public void haveScriptError(String sNaoNo, String error)
  {
     haveScriptError(Integer.parseInt(sNaoNo), error);
  }
  
  public void haveScriptError(int naoNo, String error)
  {
    hadScriptErrors.put(naoNo, true);
    haveError(naoNo, "[041mscript error:[0m " + error + "\n[0m");
  }

  public void allIsDone(String sNaoNo)
  {
     allIsDone(Integer.parseInt(sNaoNo));
  }
  
  public void allIsDone(int naoNo)
  {
    copyDone.put(naoNo, true);
    scriptDone.put(naoNo, true);
    checkIfFinished();
  }
    
  public void scriptIsDone(String sNaoNo)
  {
     scriptIsDone(Integer.parseInt(sNaoNo));
  }
  
  public void scriptIsDone(int naoNo)
  {
    scriptDone.put(naoNo, true);
    checkIfFinished();
  }  
  
  public void copyIsDone(String sNaoNo)
  {
     copyIsDone(Integer.parseInt(sNaoNo));
  }
  
  public void copyIsDone(int naoNo)
  {
    copyDone.put(naoNo, true);
    checkIfFinished();
  }

  public void checkIfFinished()
  {
    boolean done = true;
    boolean hadError = false;
    boolean hadCopyError = false;
    boolean hadScriptError = false;
    
    String add = "";
    
    for(Integer naoNo_ : this.iNaoBytes.keySet())
    {
      hadError = hadError || hadErrors.get(naoNo_);
      hadCopyError = hadCopyError || hadCopyErrors.get(naoNo_);
      hadScriptError = hadScriptError || hadScriptErrors.get(naoNo_);
      if(showDoneMsg)
      {
        done = done && copyDone.get(naoNo_) && scriptDone.get(naoNo_);
      }
      else
      {
        if(showCopyDoneMsg)
        {
          done = done && copyDone.get(naoNo_);
        }
        if(showScriptDoneMsg)
        {
          done = done && scriptDone.get(naoNo_);
        }
      }
    }
    if(done)
    {    
      if(showDoneMsg)
      {
        if(hadError)
        {
          add = " - error with Nao ";
          for(Integer naoNo_ : this.hadErrors.keySet())
          {
            if(hadErrors.get(naoNo_))
            {
              add += " " + naoNo_ + " ";
            }
          }
          add += " - please check Logs.";
        }
        actionInfo("[1;32mCopy + Scripts done\n[0m");
        JOptionPane.showMessageDialog(this, "Copy + Scripts done" + add);
      }
      else
      {
        if(showCopyDoneMsg)
        {
          if(hadCopyError)
          {
            add = " - copy error with Nao ";
            for(Integer naoNo_ : this.hadCopyErrors.keySet())
            {
              if(hadCopyErrors.get(naoNo_))
              {
                add += " " + naoNo_ + " ";
              }
            }
            add += " - please check Logs.";
          }
          actionInfo("[1;32mCopy done\n[0m");
          JOptionPane.showMessageDialog(this, "Copy done" + add);
        }
        if(showScriptDoneMsg)
        {
          if(hadCopyError)
          {
            add = " - script error with Nao ";
            for(Integer naoNo_ : this.hadScriptErrors.keySet())
            {
              if(hadScriptErrors.get(naoNo_))
              {
                add += " " + naoNo_ + " ";
              }
            }
            add += " - please check Logs.";
          }
          actionInfo("[1;32mScripts done\n[0m");
          JOptionPane.showMessageDialog(this, "Scripts done" + add);
        }
      }
      resetBackupList();    
      setFormEnabled(true);
    }
  }  
  
  private void setFormEnabled(boolean enable)
  {
    copyButton.setEnabled(enable);
    cbCopyConfig.setEnabled(enable);
    cbCopyLib.setEnabled(enable);
    cbCopyExe.setEnabled(enable);
    cbCopyLogs.setEnabled(enable);

    cbRestartNaoth.setEnabled(enable);
    cbRestartNaoqi.setEnabled(enable);
    cbRebootSystem.setEnabled(enable);
    if(config.debugVersion)
    {
      cbNoBackup.setEnabled(enable);
    }
    cbForceBackup.setEnabled(enable);

    if(jBackupBox.getItemCount() > 1 || !enable)
    {
      jBackupBox.setEnabled(enable);
    }

    jColorBox.setEnabled(enable);
    jCommentTextArea.setEnabled(enable);
    jDirChooser.setEnabled(enable);
    jSchemeBox.setEnabled(enable);

    lblTeamCommLAN.setEnabled(enable);
    lblTeamCommWLAN.setEnabled(enable);
    jTeamCommPort.setEnabled(enable);
    jTeamNumber.setEnabled(enable);

    naoByte1.setEnabled(enable);
    naoByte2.setEnabled(enable);
    naoByte3.setEnabled(enable);
    naoByte4.setEnabled(enable);

    sshUser.setEnabled(enable);
    sshRootUser.setEnabled(enable);
    sshPassword.setEnabled(enable);
    sshRootPassword.setEnabled(enable);

    subnetFieldLAN.setEnabled(enable);
    netmaskFieldLAN.setEnabled(enable);
    broadcastFieldLAN.setEnabled(enable);

    subnetFieldWLAN.setEnabled(enable);
    netmaskFieldWLAN.setEnabled(enable);
    broadcastFieldWLAN.setEnabled(enable);

    radioWPA.setEnabled(enable);
    radioWEP.setEnabled(enable);
    wlanSSID.setEnabled(enable);
    wlanKey.setEnabled(enable);

    lstNaos.setEnabled(enable);
            
    jButtonRefreshData.setEnabled(enable);
    jButtonSaveNetworkConfig.setEnabled(enable);
    jButtonInitRobotSystem.setEnabled(enable);
    jButtonSetRobotNetwork.setEnabled(enable);
    jButtonRemoteKernelVideoReload.setEnabled(enable);
  }


  private boolean prepareDeploy(NaoScpConfig cfg)
  {
    return prepareDeploy(cfg, false);
  }  
  
  /**
   * Staging - prepares "deploy out dir" with the files to be copied to the robots
   * copies files, creates scheme.cfg, modifies other cfgs according to UI settings
   * @return
   */
  private boolean prepareDeploy(NaoScpConfig cfg, boolean init)
  {
    actionInfo("preparing deploy dir");

    File deployOutDir = new File(cfg.localDeployRootPath());

    if( ! deployOutDir.isDirectory())
    {
      if( ! deployOutDir.mkdirs())
      {
        actionInfo("Error: Could not create deploy out directory");
        return false;
      }
    }

    if( ! deployOutDir.canWrite())
    {
      actionInfo("Error: Cant write to deploy out dir");
      return false;
    }

    checkNaoIps(init);
    
    for(Integer naoNo : iNaoBytes.keySet())
    {
      if((!init && naoNo > 0) || (init && naoNo == 0))
      {
        int iNaoByte = iNaoBytes.get(naoNo);
        String sNaoByte = String.valueOf(iNaoByte);
        if(iNaoByte < 256 && iNaoByte > -1)
        {
          String sNaoNo = String.valueOf(naoNo);
          String currentDeployPath = cfg.localDeployOutPath(sNaoNo);
          File currentDeployDir = new File(currentDeployPath);

          if(currentDeployDir.isDirectory())
          {
            deleteDir(currentDeployDir);
          }

          currentDeployDir.mkdirs();
          File localLibPath = new File(cfg.localDeployOutPath(sNaoNo) + cfg.libnaoPath());
          localLibPath.mkdirs();
          File localBinPath = new File(cfg.localDeployOutPath(sNaoNo) + cfg.binPath());
          localBinPath.mkdirs();

          if(cfg.copyLib)
          {
            File localLib = new File(
            cfg.localDeployOutPath(sNaoNo) + cfg.libnaoPath() + "/libnaosmal.so");
            if(localLib.exists())
            {
              localLib.delete();
            }
            copyFiles(new File(cfg.localLibnaosmalPath() + "/libnaosmal.so"), localLib);
          }

          if(cfg.copyExe)
          {
            File localExe = new File(
            cfg.localDeployOutPath(sNaoNo) + cfg.binPath() + "/naoth");
            if(localExe.exists())
            {
              localExe.delete();
            }
            try
            {
              FileOutputStream fos = new FileOutputStream(cfg.localDeployOutPath(sNaoNo) + cfg.libnaoPath() + "/comment.cfg");
              DataOutputStream out = new DataOutputStream(fos);
              out.writeBytes(jCommentTextArea.getText());
              fos.close();
            }
            catch(IOException ioe)
            {
              actionInfo("I/O Error in prepareDeploy- " + ioe.toString());
            }

            copyFiles(new File(cfg.localLibnaosmalPath() + "/naoth"), localExe);
          }

          String myConfigPathIn = cfg.localDeployInPath(sNaoNo, sNaoByte) + "/Config";
          File myConfigDirIn = new File(myConfigPathIn);
          myConfigDirIn.mkdirs();
          
          String myConfigPath = cfg.localDeployOutPath(sNaoNo) + cfg.configPath();
          File myGenericConfigDir = new File(myConfigPath + "/general");
          myGenericConfigDir.mkdirs();

          if(cfg.copyConfig)
          {
            File myConfigDir = new File(myConfigPath);
            copyFiles(new File(cfg.localConfigPath()), myConfigDir);
            writeTeamcommCfg(new File(myConfigPath + "/general/teamcomm.cfg"));
            writeScheme(new File(myConfigPath + "/scheme.cfg"));
          }
          writePlayerCfg(new File(myConfigPath + "/general/player.cfg"), sNaoNo);
        }
      }
    }

    actionInfo("finished preparing deploy dir");

    return true;
  }//end prepareDeploy

/**
   * Staging - prepares "deploy out dir for setting up nao system" with the files to be copied to the robots
   * copies files, modifies nao system cfgs according to UI settings
   * @return
   */
  private boolean prepareSetupDeploy(NaoScpConfig cfg, String sNaoByte)
  {
    actionInfo("preparing deploy dir");

    File deployOutDir = new File(cfg.localDeployRootPath());

    if( ! deployOutDir.isDirectory())
    {
      if( ! deployOutDir.mkdirs())
      {
        actionInfo("Error: Could not create deploy out directory");
        return false;
      }
    }

    if( ! deployOutDir.canWrite())
    {
      actionInfo("Error: Cant write to deploy out dir");
      return false;
    }

    checkNaoIps(false);
    int naoNo = 0;
    int iNaoByte = Integer.parseInt(sNaoByte);
    
    if(iNaoByte < 256 && iNaoByte > -1)
    {
      String sNaoNo = String.valueOf(naoNo);

      String mySetupScriptPath = cfg.localDeployOutPath(sNaoNo) + cfg.setupScriptPath();
      File mySetupScriptDir = new File(mySetupScriptPath);
      if(mySetupScriptDir.isDirectory())
      {
        deleteDir(mySetupScriptDir);
      }
      mySetupScriptDir.mkdirs();

      File mySetupScriptCheckRC = new File(mySetupScriptPath +"/checkRC.sh");        
      copyFiles(new File(cfg.localSetupScriptPath() + "/checkRC.sh"), mySetupScriptCheckRC);
      File mySetupScriptInitEnv = new File(mySetupScriptPath +"/init_env.sh");        
      copyFiles(new File(cfg.localSetupScriptPath() + "/init_env.sh"), mySetupScriptInitEnv);
      File mySetupScriptInitNet = new File(mySetupScriptPath +"/init_net.sh");        
      copyFiles(new File(cfg.localSetupScriptPath() + "/init_net.sh"), mySetupScriptInitNet);
      File myAutoloadIni = new File(mySetupScriptPath +"/autoload.ini");        
      copyFiles(new File(cfg.localSetupScriptPath() + "/autoload.ini"), myAutoloadIni);
      File myNaothScript = new File(mySetupScriptPath +"/naoth");        
      copyFiles(new File(cfg.localSetupScriptPath() + "/naoth"), myNaothScript);
            
      File libRT = new File(cfg.stagingLibDir +"/usr/lib/librt.so");
      if(libRT.exists())
      {
        libRT.delete();
      }

      String mySetupPath = mySetupScriptPath + "/etc";
      File mySetupDir = new File(mySetupPath);
      mySetupDir.mkdirs();

      copyFiles(new File(cfg.localSetupScriptPath() + "/etc"), mySetupDir);
      
      setConfdNet(cfg, sNaoByte);
      setHostname(cfg, sNaoByte);
      saveWpaSupplicant(cfg, cfg.localDeployOutPath("0") + cfg.setupScriptPath());
    }
        
    actionInfo("finished preparing deploy dir");

    return true;
  }//end prepareSetupDeploy

  private void setConfdNet(NaoScpConfig cfg, String sNaoByte)
  {
      try
      {
        BufferedReader br = new BufferedReader(new FileReader(cfg.localSetupScriptPath() + "/etc/conf.d/net"));

        String line = "";
        String fileContent = "";
        while(line != null)
        {
          line = br.readLine();
          if(line != null)
          {
            fileContent += line + "\n";
          }
        }
        br.close();
        if(subnetFieldLAN.getText().endsWith("."))
        {
          fileContent = fileContent.replace("ETH_ADDR", subnetFieldLAN.getText() + sNaoByte);
        }
        else
        {
          fileContent = fileContent.replace("ETH_ADDR", subnetFieldLAN.getText() + "."+ sNaoByte);
        }
        if(subnetFieldWLAN.getText().endsWith("."))
        {
          fileContent = fileContent.replace("WLAN_ADDR", subnetFieldWLAN.getText() + sNaoByte);
        }
        else
        {
          fileContent = fileContent.replace("WLAN_ADDR", subnetFieldWLAN.getText() + "."+ sNaoByte);
        }
        fileContent = fileContent.replace("ETH_NETMASK", netmaskFieldLAN.getText());
        fileContent = fileContent.replace("WLAN_NETMASK", netmaskFieldWLAN.getText());
        fileContent = fileContent.replace("ETH_BRD", broadcastFieldLAN.getText());
        fileContent = fileContent.replace("WLAN_BRD", broadcastFieldWLAN.getText());
        fileContent.trim();

        BufferedWriter bw = new BufferedWriter(new FileWriter(cfg.localDeployOutPath("0") + cfg.setupScriptPath() + "/etc/conf.d/net"));
        bw.write(fileContent);
        bw.close();
      }
      catch(Exception e)
      {
        actionInfo(e.toString());
      }
  }//end setConfdNet

  private void loadWpaSupplicant()
  {
    try
    {
      BufferedReader br = new BufferedReader(new FileReader(config.localSetupScriptPath() + "/etc/wpa_supplicant/wpa_supplicant.conf"));
      String line = "";
      String fileContent = "";
      while(line != null)
      {
        line = br.readLine();
        if(line != null)
        {
          if(line.contains("ssid=\""))
          {
            String ssid = line.trim().replace("ssid=", "").replace("\"", "");
            wlanSSID.setText(ssid);
          }
          if(line.contains("wep_key0="))
          {
            String ssid = line.trim().replace("wep_key0=", "");
            wlanKey.setText(ssid);
          }
          if(line.contains("psk="))
          {
            String ssid = line.trim().replace("psk=", "").replace("\"", "");
            wlanKey.setText(ssid);
          }
          fileContent += line + "\n";
        }
      }
      br.close();
      if(fileContent.contains("WPA-PSK"))
      {
        radioWPA.setSelected(true);
        radioWEP.setSelected(false);
      }
      else
      {
        radioWPA.setSelected(false);
        radioWEP.setSelected(true);
      }
    }
    catch(Exception e)
    {}

  }//end loadWpaSupplicant

  private void saveWpaSupplicant(NaoScpConfig cfg, String dstDir)
  {
    try
    {
      String wpaConfigFileName = cfg.localSetupScriptPath() + "/wpa_supplicant.wpa";
      if(radioWEP.isSelected())
      {
        wpaConfigFileName = cfg.localSetupScriptPath() + "/wpa_supplicant.wep";
      }
      BufferedReader br = new BufferedReader(new FileReader(wpaConfigFileName));

      String line = "";
      String fileContent = "";
      while(line != null)
      {
        line = br.readLine();
        if(line != null)
        {
          fileContent += line + "\n";
        }
      }
      br.close();
      fileContent = fileContent.replace("WLAN_SSID", wlanSSID.getText());
      fileContent = fileContent.replace("WLAN_KEY", wlanKey.getText());
      fileContent.trim();

      File wpaConfig = new File(dstDir + "/etc/wpa_supplicant/wpa_supplicant.conf");
      if(wpaConfig.exists())
      {
        wpaConfig.delete();
      }
      BufferedWriter bw = new BufferedWriter(new FileWriter(dstDir + "/etc/wpa_supplicant/wpa_supplicant.conf"));
      bw.write(fileContent);
      bw.close();
    }
    catch(Exception e)
    {
      actionInfo(e.toString());
    }
  }//end setWpaSupplicant
  
  private void setHostname(NaoScpConfig cfg, String sNaoByte)
  {
    try
    {
      BufferedReader br = new BufferedReader(new FileReader(cfg.localSetupScriptPath() + "/etc/hostname"));

      String line = "";
      String fileContent = "";
      while(line != null)
      {
        line = br.readLine();
        if(line != null)
        {
          fileContent += line + "\n";
        }
      }
      br.close();
      fileContent = fileContent.replace("NAONR", sNaoByte);

      BufferedWriter bw = new BufferedWriter(new FileWriter(cfg.localDeployOutPath("0") + cfg.setupScriptPath() + "/etc/hostname"));
      bw.write(fileContent);
      bw.close();


      br = new BufferedReader(new FileReader(cfg.localSetupScriptPath() + "/etc/conf.d/hostname"));

      line = "";
      fileContent = "";
      while(line != null)
      {
        line = br.readLine();
        if(line != null)
        {
          fileContent += line + "\n";
        }
      }
      br.close();

      fileContent = fileContent.replace("NAONR", sNaoByte);
      bw = new BufferedWriter(new FileWriter(cfg.localDeployOutPath("0") + cfg.setupScriptPath() + "/etc/conf.d/hostname"));
      bw.write(fileContent);
      bw.close();
    }
    catch(Exception e)
    {
      actionInfo(e.toString());
    }
  }//end setHostname
  
  
  private String readFile(String fileName)
  {
    StringBuilder content = new StringBuilder();

    try 
    {
      //use buffering, reading one line at a time
      //FileReader always assumes default encoding is OK!
      BufferedReader input =  new BufferedReader(new FileReader(fileName));
      try {
        String line; //not declared within while loop
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
      actionInfo("I/O Error in readFile- " + fileName + "\n" + ex.toString());
    }

    return content.toString();
  }//end readFile

  private void setDirectory(String directoryName)
  {
    jDirPathLabel.setText(directoryName);
    config.jDirPathLabel = directoryName;
    setSchemes();
    copyButton.setEnabled(true);
    jButtonSaveNetworkConfig.setEnabled(true);
    jButtonSetRobotNetwork.setEnabled(true);
    jButtonInitRobotSystem.setEnabled(true);
    setFormData();
  }

  private void setCommentText()
  {
    String comment = "";
    if(jBackupBox.getItemCount() > 1)
    {
      File file = new File(config.localDeployRootPath() + "/in/" + config.backups.get(jBackupBox.getSelectedItem()) + "/comment.cfg");
      if(file.exists() && file.isFile())
      {
        comment = readFile(file.getPath());
      }
    }
    jCommentTextArea.setText(comment);
  }

  private void resetBackupList()
  {
    String deployInPath = config.localDeployRootPath() + "/in";
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
        String entry = backupFiles[i].getName();
        Boolean entryAvaliable = false;
        String entryTag = "";
        Boolean isMinimal = false;

        File minConfigDir = new File(config.localDeployRootPath() + "/in/" + entry + "/MinimalConfig/");
        if(minConfigDir.exists() && minConfigDir.isDirectory())
        {
          entryAvaliable = true;
          isMinimal = true;
        }

        File configDir = new File(config.localDeployRootPath() + "/in/" + entry + "/Config/");
        if(configDir.exists() && configDir.isDirectory())
        {
          entryAvaliable = true;
          entryTag += "C";
        }

        File libFile = new File(config.localDeployRootPath() + "/in/" + entry + "/libnaosmal.so");
        if(libFile.exists() && libFile.isFile())
        {
          entryAvaliable = true;
          entryTag += "L";
        }

        File exeFile = new File(config.localDeployRootPath() + "/in/" + entry + "/naoth");
        if(exeFile.exists() && exeFile.isFile())
        {
          entryAvaliable = true;
          entryTag += "E";
        }

        if(isMinimal)
        {
          if(entryTag.equals(""))
          {
            entryTag = "M" + entryTag;
          }
          else
          {
            entryTag = "M [" + entryTag + "]";
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
            config.backups.put(backupItems[i], entry);
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
    this.initNetworkConfig();
    resetBackupList();
    
    File f = new File(config.localConfigPath() + "/general/player.cfg");
    if(!f.isFile())
    {
      writePlayerCfg(f, "1");
    }
    else
    {
      readPlayerCfg(f);
    }

    loadWpaSupplicant();

    f = new File(config.localConfigPath() + "/general/teamcomm.cfg");
    if(!f.isFile())
    {
      writeTeamcommCfg(f);
    }
    else
    {
      readTeamCfg(f);
    }
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
    File configPath = new File(config.localConfigPath());
    if( ! configPath.isDirectory())
    {
      return false;
    }

    File schemePath = new File(config.localConfigPath() + "/scheme");
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
        if(files[i].getName().equalsIgnoreCase("nao"))
        {
          jSchemeBox.setSelectedIndex(jSchemeBox.getItemCount() - 1);
        }
      }
    }
    return true;
  }//end setSchemes
    
  private void logBashColored(String logtext)
  {
    ArrayList<String> unformatedParts = new ArrayList<String>();
    ArrayList<String> partFormats = new ArrayList<String>();
    partFormats.add(lastBashColorOption);
    
    String bashFormatIdentifier = "["; 
    int posLeft = 0;     
    
    while (posLeft < logtext.length())
    {
      int pos = logtext.indexOf(bashFormatIdentifier, posLeft);
      if(pos != -1)
      {
        int posFormatEnd = logtext.indexOf("m", pos + 1);
        if(posFormatEnd != -1)
        {
          String format = logtext.substring(pos + 1, posFormatEnd);
          String toBeFormated = logtext.substring(posLeft, pos);
          partFormats.add("bash_" + format.replace(";", "_"));
          lastBashColorOption = "bash_" + format.replace(";", "_");
          unformatedParts.add(toBeFormated);
          pos = posFormatEnd;
        }
        posLeft = pos;
      }     
      posLeft++;
    }
    unformatedParts.add("");
    StyledDocument doc = logTextPane.getStyledDocument();

    try 
    {
      if(unformatedParts.size() > 1)
      {
        String unformated = "";
        for (int i = 0; i < unformatedParts.size(); i++) 
        {
          unformated += unformatedParts.get(i);
          if(logActionInfo)
          {
            Style s = doc.getStyle(partFormats.get(i));
            doc.insertString(doc.getLength(), unformatedParts.get(i), s);
          }
        }
        jCopyStatus.setText(unformated);
        System.out.println(unformated);
      }
      else
      {
        jCopyStatus.setText(logtext);
        System.out.println(logtext + "\n");
        if(logActionInfo)
        {
          doc.insertString(doc.getLength(), logtext + "\n", doc.getStyle(lastBashColorOption));
        }
      }
    } 
    catch (BadLocationException ble) 
    {
//        System.err.println("Couldn't insert initial text into text pane.");
    }
  }  
  
  
  protected final void actionInfo(String logtext)
  {
    logBashColored(logtext);
  }//end log

  private void clearLog()
  {
    logTextPane.setText("");
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
    if( ! src.exists() ||  ! src.canRead() || src.getName().equals(".svn") || src.getName().equals(".bzr") || src.getName().equals(".hg") || src.getName().equals(".git"))
    {
      return;
    }
    if(src.isDirectory())
    {
      if( ! dest.exists())
      {
        if( ! dest.mkdirs())
        {
          actionInfo("copyFiles: Could not create direcotry: " + dest.getAbsolutePath() + ".");
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
        actionInfo("copyFiles: Unable to copy file: " + src.getAbsolutePath() + " to " + dest.
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
        BufferedWriter writer = new BufferedWriter(new FileWriter(schemeCfg));
        writer.write(outString);
        writer.close();
      }
      catch(IOException ioe)
      {
        actionInfo("I/O Error in writeScheme- " + ioe.toString());
      }
    }
  }
  
  private void readPlayerCfg(File configFile)
  {
    try
    {
      BufferedReader reader = new BufferedReader(new FileReader(configFile));
      String line = reader.readLine();
      while(line != null && !line.contains("[player]"))
      {
        line = reader.readLine();
      }
      line = reader.readLine();
      while(line != null)
      {
        String value;
        if(line.contains("TeamColor="))
        {
          value = line.replace("TeamColor=", "").trim();
          for(int idx = 0; idx < jColorBox.getItemCount(); idx++)
          {
            if(value.equalsIgnoreCase(jColorBox.getItemAt(idx).toString()) )
            {
              jColorBox.setSelectedIndex(idx);
              break;
            }
          }
        }
        else if(line.contains("TeamNumber="))
        {
          value = line.replace("TeamNumber=", "").trim();          
          jTeamNumber.setText(value);
        }        
        line = reader.readLine();
      }
    }
    catch(Exception e)
    {
      actionInfo(e.toString());
    }
  }
  
  private void writePlayerCfg(File configFile, String playerNumber)
  {
    if(playerNumber.equals("0"))
    {
      playerNumber = setupPlayerNo;
    }
    
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
    
    c.append("NumOfPlayer=");
    c.append(String.valueOf(iNaoBytes.size() - 1));
    c.append("\n");
    
    try
    {
      BufferedWriter writer = new BufferedWriter(new FileWriter(configFile));
      writer.write(c.toString());
      writer.close();
    }
    catch(IOException ex)
    {
      actionInfo("I/O Error in writePlayerCfg- " + ex.getMessage());
    }
  }
  
  private void readTeamCfg(File configFile)
  {
    try
    {
      BufferedReader reader = new BufferedReader(new FileReader(configFile));
      String line = reader.readLine();
      while(line != null && !line.contains("[teamcomm]"))
      {
        line = reader.readLine();
      }
      line = reader.readLine();
      while(line != null)
      {
        String value;
        if(line.contains("port="))
        {
          value = line.replace("port=", "").trim();          
          jTeamCommPort.setText(value);
        }        
        line = reader.readLine();
      }
    }
    catch(Exception e)
    {
      actionInfo(e.toString());
    }
  }
 
  private void writeTeamcommCfg(File configFile)
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
      BufferedWriter writer = new BufferedWriter(new FileWriter(configFile));
      writer.write(c.toString());
      writer.close();
    }
    catch(IOException ex)
    {
      actionInfo("I/O Error in writeTeamcommCfg- " + ex.getMessage());
    }
  }
  
  public boolean isIPV6Address(String hostAddress)
  {
    if(hostAddress != null && hostAddress.contains(":"))
    {
      return true;
    }
    return false;
  }

  public boolean isIPV4Address(String hostAddress)
  {
    if(hostAddress.contains(":") || !hostAddress.contains(".") || hostAddress.length() < 8)
    {
      return false;
    }
    String[] IPv4Parts = hostAddress.trim().split("\\.");
    if(IPv4Parts.length != 4)
    {
      return false;
    }
    for(int p = 0; p < 4; p++)
    {
      int part;
      try
      {
        part = Integer.parseInt(IPv4Parts[p]);
      }
      catch(NumberFormatException ex)
      {
        return false;
      }
      if(part < 0 || part > 255)
      {
        return false;
      }
    }    
    return true;
  }

  
  private class robotConfigPreparator
  {
    private ArrayList<String> addresses;
    String address;
    robotConfigPreparator(NaoScpConfig naoScpConfig)
    {
      synchronized (naoModel)
      {
        clearLog();
        naoScpConfig.copyConfig = true;
        naoScpConfig.copyLib = true;
        naoScpConfig.copyExe = true;
        naoScpConfig.copyLogs = false;
        naoScpConfig.restartNaoth = false;
        naoScpConfig.noBackup = true;

        setFormEnabled(false);

        showCopyDoneMsg = false;
        showScriptDoneMsg = false;
        showDoneMsg = true;

        for(Integer naoNo : copyDone.keySet())
        {
          if(naoNo > 0)
          {
            copyDone.put(naoNo, true);
            scriptDone.put(naoNo, true);
          }
          hadCopyErrors.put(naoNo, false);
          hadScriptErrors.put(naoNo, false);
          hadErrors.put(naoNo, false);
        }
        copyDone.put(0, false);
        scriptDone.put(0, false);
        naoScpConfig.backupTimestamp = String.valueOf(System.currentTimeMillis());

        addresses = new ArrayList<String>();      
        NaoSshWrapper w;
        int i = lstNaos.getSelectedIndex();
        lstNaos.clearSelection();
        if (i >= 0)
        {
          w = (NaoSshWrapper) naoModel.get(i);

          if(w.getAddresses() != null && w.getAddresses().length > 0)
          {
            for(InetAddress addr : w.getAddresses())
            {
              if(addr instanceof Inet4Address)
              {
                addresses.add(addr.getHostAddress());
              }
            }
            address = w.getAddresses()[0].getHostAddress();
          }
          else
          {
            address = w.getDefaultHostAddress();
          }

          if(isIPV6Address(address))
          {
            if(address == null)
            {
              address = JOptionPane.showInputDialog("IPv6 addresses are not supported. Enter appropriate IPv4 address", "");             
              while(address != null && !isIPV4Address(address))
              {
                address = JOptionPane.showInputDialog("Not a valid IPv4 address. Enter a valid one", "");             
              }
            }
          }
        }
        else
        {        
          address = JOptionPane.showInputDialog("No IP selected. Enter an IPv4 address", "");
          while(address != null && !isIPV4Address(address))
          {
            if(isIPV6Address(address))
            {
              address = JOptionPane.showInputDialog("IPv6 addresses are not supported. Enter appropriate IPv4 address", ""); 
            }
            else
            {
              address = JOptionPane.showInputDialog("Not a valid IPv4 address. Enter a valid one", "");             
            }
          }
          addresses.clear();
          addresses.add(address);
        }
      }
    }
    
    public String getDefaultAddress()
    {
      sNaoLanIps.put(0, address);
      sNaoWLanIps.put(0, address);
      return address;
    }
    
    public String askForNaoNumber()
    {
      String sNaoByte = null;
      int iNaoByte = -1;
      while(iNaoByte > 255 || iNaoByte < 0)
      {
        sNaoByte = JOptionPane.showInputDialog("Enter Nao Number (0-255)", "");
        if(sNaoByte == null)
        {
          break;
        }
        else
        {
          try
          {
            iNaoByte = Integer.parseInt(sNaoByte);
          }
          catch(Exception e)
          {
            iNaoByte = -1;
          }
        }
      }
      iNaoBytes.put(0, iNaoByte);          
      return sNaoByte;
    }
    
    public String askForPlayerNumber()
    {
      String sPlayerNo = null;
      int iPlayerNo = 0;
      while(iPlayerNo >= iNaoBytes.size() || iPlayerNo < 1)
      {
        sPlayerNo = JOptionPane.showInputDialog("Enter Player Number (1 - " + (iNaoBytes.size() - 1) + ")", "");
        if(sPlayerNo == null)
        {
          break;
        }
        else
        {
          iPlayerNo = Integer.parseInt(sPlayerNo);
        }
      }
      return sPlayerNo;
    }
    
    public ArrayList<String> getAdressList()
    {
      return this.addresses;
    }
  }

  private void remoteReloadKernelVideoModule()
  {
    clearLog();
    NaoScpConfig cfg = new NaoScpConfig(config);
    robotConfigPreparator preparator = new robotConfigPreparator(cfg);
    String address = preparator.getDefaultAddress();
    cfg.addresses = preparator.getAdressList();

    //show only Script done Message @ the end
    showCopyDoneMsg = false;
    showScriptDoneMsg = true;
    showDoneMsg = false;
    
    //nothing to copy so its done @ begin
    copyDone.put(0, true);

    if(address == null)
    {
      actionInfo("reloading kernel video module aborted");
      setFormEnabled(true);
      return;
    }
    remoteScriptRunner scriptRunner = new remoteScriptRunner(cfg, "0", "0", "reloadKernelVideoModule", false);
    scriptRunner.execute();
  }
  
  private void updateConfig()
  {
    config.sshUser = this.sshUser.getText();
    config.sshPassword = this.sshPassword.getText();
    config.sshRootPassword = this.sshRootPassword.getText();
    config.progressBar = this.progressBar;
  }
  
  private void initializeRobot()
  {
    updateConfig();
    
    clearLog();
    NaoScpConfig cfg = new NaoScpConfig(config);
    cfg.stagingLibDir = null;
    File stdCtcDir = new File("/opt/aldebaran/info/crosscompile/staging");

    if(stdCtcDir.isDirectory())
    {
      stdCtcDir = new File("/opt/aldebaran/info/crosscompile/staging");
    }
    else
    {
      stdCtcDir = new File("./../../");
    }      

    JFileChooser chooser = new JFileChooser();
    chooser.setCurrentDirectory(stdCtcDir);
    chooser.setDialogTitle("Select toolchain \"extern\" Directory");
    chooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
    chooser.setAcceptAllFileFilterUsed(false);
    int ret = chooser.showOpenDialog(this);

    while(ret != JFileChooser.CANCEL_OPTION && cfg.stagingLibDir == null)
    {
      if(ret == JFileChooser.APPROVE_OPTION)
      {
        cfg.stagingLibDir = chooser.getSelectedFile().getPath();
      }

      if(cfg.stagingLibDir != null)
      {
        cfg.stagingLibDir += "/lib";
        File gioFile = new File(cfg.stagingLibDir + "/libgio-2.0.so");
        File glibDir = new File(cfg.stagingLibDir + "/glib-2.0");
        if(!gioFile.isFile() || !glibDir.isDirectory())
        {
          cfg.stagingLibDir = null;
          chooser.setDialogTitle("toolchain \"extern\" Directory seems to be wrong. Try again");
          ret = chooser.showOpenDialog(this);
        }
      }
    }     
    if(cfg.stagingLibDir == null)
    {
      actionInfo("no valid toolchain \"extern\" Directory selected");
      setFormEnabled(true);
      return;
    }
    
    robotConfigPreparator preparator = new robotConfigPreparator(cfg);
    cfg.copySysLibs = true;
    String address = preparator.getDefaultAddress();
    cfg.addresses = preparator.getAdressList();
    String sNaoByte = null;
    setupPlayerNo = null;
    if(address != null )
    {
      sNaoByte = preparator.askForNaoNumber();
      if(sNaoByte != null )
      {
        setupPlayerNo = preparator.askForPlayerNumber();
      }
    }    
    iNaoBytes = iNaoBytes;
    if(address == null || sNaoByte == null || setupPlayerNo == null || !prepareDeploy(cfg, true) || !prepareSetupDeploy(cfg, sNaoByte))
    {
      actionInfo("robot initialization aborted");
      setFormEnabled(true);
      return;
    }
    remoteSetupCopier setupCopier = new remoteSetupCopier(cfg, sNaoByte, "full");
    setupCopier.execute();
  }

  
  private void setRobotNetwork()
  {
    clearLog();
    NaoScpConfig cfg = new NaoScpConfig(config);
    robotConfigPreparator preparator = new robotConfigPreparator(cfg);
    cfg.copySysLibs = false;
    final String address = preparator.getDefaultAddress();
    cfg.addresses = preparator.getAdressList();
      
    String sNaoByte = null;
    if(address != null )
    {
      sNaoByte = preparator.askForNaoNumber();
    }
    if(address == null || sNaoByte == null || !prepareSetupDeploy(cfg, sNaoByte))
    {
      actionInfo("robot network configuration aborted");
      setFormEnabled(true);
      return;
    }
    remoteSetupCopier setupCopier = new remoteSetupCopier(cfg, sNaoByte, "network");
    setupCopier.execute();
  }
    
  private void setActionBtnLabel()
  {
    boolean copyActionSelected = cbCopyConfig.isSelected()
                                  || cbCopyExe.isSelected()
                                  || cbCopyLib.isSelected()
                                  || cbCopyLogs.isSelected();
    boolean scriptActionSelected = cbRestartNaoth.isSelected()
                                  || cbRestartNaoqi.isSelected();

    copyButton.setEnabled(checkDirPath(false));

    if(copyActionSelected && scriptActionSelected)
    {
      copyButton.setText("Copy files and run script");
    }
    else if(copyActionSelected)
    {
      copyButton.setText("Copy files");
    }
    else if(scriptActionSelected)
    {
      if(cbForceBackup.isSelected())
      {
        copyButton.setText("Full backup and run Script");
      }
      else if(cbNoBackup.isSelected())
      {
        copyButton.setText("Minimal backup and run Script");
      }
      else
      {
        copyButton.setText("Run Script");
      }
    }
    else if(cbForceBackup.isSelected())
    {
      copyButton.setText("Full backup");
    }
    else if(cbNoBackup.isSelected())
    {
      copyButton.setText("Minimal backup");
    }
    else
    {
      copyButton.setText("Nothing to do");
      copyButton.setEnabled(false);
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

  // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
  private void initComponents()
  {

    jDialog1 = new javax.swing.JDialog();
    wlanBtnGroup = new javax.swing.ButtonGroup();
    jLabel12 = new javax.swing.JLabel();
    jSplitPane1 = new javax.swing.JSplitPane();
    jTabbedPane1 = new javax.swing.JTabbedPane();
    jPanel1 = new javax.swing.JPanel();
    jActionsPanel = new javax.swing.JPanel();
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
    jLabel15 = new javax.swing.JLabel();
    jBackupBox = new javax.swing.JComboBox();
    naoByte4 = new javax.swing.JTextField();
    jLabel17 = new javax.swing.JLabel();
    cbCopyLib = new javax.swing.JCheckBox();
    cbRestartNaoth = new javax.swing.JCheckBox();
    cbCopyConfig = new javax.swing.JCheckBox();
    jSettingsPanel = new javax.swing.JPanel();
    jLabel3 = new javax.swing.JLabel();
    jColorBox = new javax.swing.JComboBox();
    jLabel2 = new javax.swing.JLabel();
    jTeamNumber = new javax.swing.JTextField();
    jLabel4 = new javax.swing.JLabel();
    jSchemeBox = new javax.swing.JComboBox();
    jButtonRefreshData = new javax.swing.JButton();
    cbNoBackup = new javax.swing.JCheckBox();
    jScrollPane2 = new javax.swing.JScrollPane();
    jCommentTextArea = new javax.swing.JTextArea();
    cbCopyLogs = new javax.swing.JCheckBox();
    jLabel16 = new javax.swing.JLabel();
    cbCopyExe = new javax.swing.JCheckBox();
    cbRestartNaoqi = new javax.swing.JCheckBox();
    cbForceBackup = new javax.swing.JCheckBox();
    jPanel2 = new javax.swing.JPanel();
    jSettingsPanel1 = new javax.swing.JPanel();
    jLabel13 = new javax.swing.JLabel();
    jLabel1 = new javax.swing.JLabel();
    jLabel20 = new javax.swing.JLabel();
    jLabel22 = new javax.swing.JLabel();
    subnetFieldLAN = new javax.swing.JTextField();
    netmaskFieldLAN = new javax.swing.JTextField();
    broadcastFieldLAN = new javax.swing.JTextField();
    jLabel14 = new javax.swing.JLabel();
    jLabel19 = new javax.swing.JLabel();
    jLabel21 = new javax.swing.JLabel();
    jLabel23 = new javax.swing.JLabel();
    subnetFieldWLAN = new javax.swing.JTextField();
    netmaskFieldWLAN = new javax.swing.JTextField();
    broadcastFieldWLAN = new javax.swing.JTextField();
    radioWPA = new javax.swing.JRadioButton();
    radioWEP = new javax.swing.JRadioButton();
    jLabel5 = new javax.swing.JLabel();
    wlanSSID = new javax.swing.JTextField();
    jLabel9 = new javax.swing.JLabel();
    wlanKey = new javax.swing.JPasswordField();
    jSettingsPanel2 = new javax.swing.JPanel();
    jLabel24 = new javax.swing.JLabel();
    lblTeamCommWLAN = new javax.swing.JTextField();
    jLabel25 = new javax.swing.JLabel();
    lblTeamCommLAN = new javax.swing.JTextField();
    jLabel26 = new javax.swing.JLabel();
    jTeamCommPort = new javax.swing.JTextField();
    jLabel27 = new javax.swing.JLabel();
    sshUser = new javax.swing.JTextField();
    jLabel28 = new javax.swing.JLabel();
    sshPassword = new javax.swing.JPasswordField();
    jLabel29 = new javax.swing.JLabel();
    sshRootUser = new javax.swing.JTextField();
    jLabel30 = new javax.swing.JLabel();
    sshRootPassword = new javax.swing.JPasswordField();
    jButtonSetRobotNetwork = new javax.swing.JButton();
    jButtonInitRobotSystem = new javax.swing.JButton();
    jButtonSaveNetworkConfig = new javax.swing.JButton();
    jScrollPane3 = new javax.swing.JScrollPane();
    lstNaos = new javax.swing.JList();
    cbRebootSystem = new javax.swing.JCheckBox();
    jButtonRemoteKernelVideoReload = new javax.swing.JButton();
    jPanel3 = new javax.swing.JPanel();
    jScrollPane5 = new javax.swing.JScrollPane();
    logTextPane = new javax.swing.JTextPane();
    jCopyStatus = new javax.swing.JLabel();
    progressBar = new javax.swing.JProgressBar();

    org.jdesktop.layout.GroupLayout jDialog1Layout = new org.jdesktop.layout.GroupLayout(jDialog1.getContentPane());
    jDialog1.getContentPane().setLayout(jDialog1Layout);
    jDialog1Layout.setHorizontalGroup(
      jDialog1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
      .add(0, 400, Short.MAX_VALUE)
    );
    jDialog1Layout.setVerticalGroup(
      jDialog1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
      .add(0, 300, Short.MAX_VALUE)
    );

    setDefaultCloseOperation(javax.swing.WindowConstants.EXIT_ON_CLOSE);
    setTitle("NaoSCP");
    addWindowListener(new java.awt.event.WindowAdapter()
    {
      public void windowClosing(java.awt.event.WindowEvent evt)
      {
        formWindowClosing(evt);
      }
    });

    jLabel12.setFont(new java.awt.Font("Lucida Grande", 0, 8)); // NOI18N
    jLabel12.setText("v0.4");

    jLabel6.setText("Nao 1:");

    naoByte1.setText("-1");
    naoByte1.setMaximumSize(new java.awt.Dimension(10, 31));
    naoByte1.setName("naoByte1"); // NOI18N
    naoByte1.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        naoByte1ActionPerformed(evt);
      }
    });

    jLabel7.setText("Nao 2:");

    naoByte2.setText("-1");
    naoByte2.setMaximumSize(new java.awt.Dimension(10, 31));
    naoByte2.setName("naoByte2"); // NOI18N
    naoByte2.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        naoByte2ActionPerformed(evt);
      }
    });

    naoByte3.setText("-1");
    naoByte3.setMaximumSize(new java.awt.Dimension(10, 31));
    naoByte3.setName("naoByte3"); // NOI18N
    naoByte3.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        naoByte3ActionPerformed(evt);
      }
    });

    jLabel8.setText("Nao 3:");

    copyButton.setEnabled(false);
    copyButton.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        copyButtonActionPerformed(evt);
      }
    });

    jDirChooser.setText("...");
    jDirChooser.setActionCommand("jDirChoose");
    jDirChooser.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        jDirChooserPerformed(evt);
      }
    });

    jDirPathLabel.setHorizontalAlignment(javax.swing.SwingConstants.LEFT);
    jDirPathLabel.setText("Set directory to {RepDir}/NaoTHSoccer");
    jDirPathLabel.setToolTipText("NaoController project directory (e.g., \"D:\\u005cu005cNaoTH-2009\\u005cu005cProjects\\u005cu005cNaoController\")");

    jLabel15.setText("Version to Upload:");

    jBackupBox.setEnabled(false);
    jBackupBox.addItemListener(new java.awt.event.ItemListener()
    {
      public void itemStateChanged(java.awt.event.ItemEvent evt)
      {
        jBackupBoxItemStateChanged(evt);
      }
    });

    naoByte4.setText("-1");
    naoByte4.setMaximumSize(new java.awt.Dimension(10, 31));
    naoByte4.setName("naoByte4"); // NOI18N
    naoByte4.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        naoByte4ActionPerformed(evt);
      }
    });

    jLabel17.setText("Nao 4:");

    org.jdesktop.layout.GroupLayout jActionsPanelLayout = new org.jdesktop.layout.GroupLayout(jActionsPanel);
    jActionsPanel.setLayout(jActionsPanelLayout);
    jActionsPanelLayout.setHorizontalGroup(
      jActionsPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
      .add(jActionsPanelLayout.createSequentialGroup()
        .add(jActionsPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
          .add(jActionsPanelLayout.createSequentialGroup()
            .add(jDirChooser)
            .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
            .add(jDirPathLabel, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
          .add(jActionsPanelLayout.createSequentialGroup()
            .addContainerGap()
            .add(jActionsPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.TRAILING)
              .add(org.jdesktop.layout.GroupLayout.LEADING, jSeparator1)
              .add(org.jdesktop.layout.GroupLayout.LEADING, jActionsPanelLayout.createSequentialGroup()
                .add(jLabel15)
                .add(18, 18, 18)
                .add(jBackupBox, 0, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
              .add(org.jdesktop.layout.GroupLayout.LEADING, jActionsPanelLayout.createSequentialGroup()
                .add(jLabel6)
                .add(18, 18, 18)
                .add(naoByte1, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 38, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
                .add(6, 6, 6)
                .add(jLabel7)
                .add(18, 18, 18)
                .add(naoByte2, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 38, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
                .add(6, 6, 6)
                .add(jLabel8)
                .add(18, 18, 18)
                .add(naoByte3, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 38, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
                .add(6, 6, 6)
                .add(jLabel17)
                .add(18, 18, 18)
                .add(naoByte4, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 35, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
                .add(18, 18, 18)
                .add(copyButton, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)))))
        .addContainerGap())
    );
    jActionsPanelLayout.setVerticalGroup(
      jActionsPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
      .add(jActionsPanelLayout.createSequentialGroup()
        .addContainerGap()
        .add(jActionsPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
          .add(jLabel7)
          .add(jLabel8)
          .add(jLabel6)
          .add(jLabel17)
          .add(naoByte2, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
          .add(naoByte1, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
          .add(naoByte3, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
          .add(naoByte4, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
          .add(copyButton, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        .add(18, 18, 18)
        .add(jActionsPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
          .add(jLabel15)
          .add(jBackupBox, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
        .addPreferredGap(org.jdesktop.layout.LayoutStyle.UNRELATED)
        .add(jSeparator1, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 21, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
        .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
        .add(jActionsPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
          .add(jDirChooser, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 18, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
          .add(jDirPathLabel, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 18, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
        .addContainerGap())
    );

    cbCopyLib.setText("copyLibNaoSMAL");
    cbCopyLib.addItemListener(new java.awt.event.ItemListener()
    {
      public void itemStateChanged(java.awt.event.ItemEvent evt)
      {
        cbCopyLibItemStateChanged(evt);
      }
    });

    cbRestartNaoth.setSelected(true);
    cbRestartNaoth.setText("restartNaoTH");
    cbRestartNaoth.addItemListener(new java.awt.event.ItemListener()
    {
      public void itemStateChanged(java.awt.event.ItemEvent evt)
      {
        cbRestartNaothItemStateChanged(evt);
      }
    });
    cbRestartNaoth.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        cbRestartNaothActionPerformed(evt);
      }
    });

    cbCopyConfig.setText("copyConfig");
    cbCopyConfig.addItemListener(new java.awt.event.ItemListener()
    {
      public void itemStateChanged(java.awt.event.ItemEvent evt)
      {
        cbCopyConfigItemStateChanged(evt);
      }
    });

    jSettingsPanel.setBackground(new java.awt.Color(204, 204, 255));

    jLabel3.setText("Color:");

    jColorBox.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "red", "blue" }));
    jColorBox.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        jColorBoxActionPerformed(evt);
      }
    });

    jLabel2.setText("TeamNr:");

    jTeamNumber.setText("4");

    jLabel4.setText("Scheme:");

    jSchemeBox.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "n/a" }));
    jSchemeBox.addItemListener(new java.awt.event.ItemListener()
    {
      public void itemStateChanged(java.awt.event.ItemEvent evt)
      {
        jSchemeBoxItemStateChanged(evt);
      }
    });

    jButtonRefreshData.setText("Refresh");
    jButtonRefreshData.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        jButtonRefreshDataActionPerformed(evt);
      }
    });

    org.jdesktop.layout.GroupLayout jSettingsPanelLayout = new org.jdesktop.layout.GroupLayout(jSettingsPanel);
    jSettingsPanel.setLayout(jSettingsPanelLayout);
    jSettingsPanelLayout.setHorizontalGroup(
      jSettingsPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
      .add(jSettingsPanelLayout.createSequentialGroup()
        .addContainerGap()
        .add(jSettingsPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
          .add(jLabel4)
          .add(jLabel3))
        .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
        .add(jSettingsPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
          .add(jSettingsPanelLayout.createSequentialGroup()
            .add(jColorBox, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 89, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
            .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
            .add(jLabel2)
            .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
            .add(jTeamNumber, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 36, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
            .add(18, 18, 18)
            .add(jButtonRefreshData, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 197, Short.MAX_VALUE))
          .add(jSchemeBox, 0, 423, Short.MAX_VALUE))
        .addContainerGap())
    );
    jSettingsPanelLayout.setVerticalGroup(
      jSettingsPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
      .add(jSettingsPanelLayout.createSequentialGroup()
        .addContainerGap()
        .add(jSettingsPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
          .add(jSchemeBox, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
          .add(jLabel4, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 17, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
        .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
        .add(jSettingsPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
          .add(jColorBox)
          .add(jTeamNumber)
          .add(jLabel2, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 17, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
          .add(jButtonRefreshData)
          .add(jLabel3, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 59, Short.MAX_VALUE)))
    );

    cbNoBackup.setText("ONLY MINIMAL BACKUP");
    cbNoBackup.addItemListener(new java.awt.event.ItemListener()
    {
      public void itemStateChanged(java.awt.event.ItemEvent evt)
      {
        cbNoBackupItemStateChanged(evt);
      }
    });
    cbNoBackup.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        cbNoBackupActionPerformed(evt);
      }
    });

    jCommentTextArea.setColumns(15);
    jCommentTextArea.setRows(4);
    jScrollPane2.setViewportView(jCommentTextArea);

    cbCopyLogs.setText("copyLogs");
    cbCopyLogs.addItemListener(new java.awt.event.ItemListener()
    {
      public void itemStateChanged(java.awt.event.ItemEvent evt)
      {
        cbCopyLogsItemStateChanged(evt);
      }
    });
    cbCopyLogs.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        cbCopyLogsActionPerformed(evt);
      }
    });

    jLabel16.setText("Comment:");

    cbCopyExe.setSelected(true);
    cbCopyExe.setText("copyNaoTH(exe)");
    cbCopyExe.addItemListener(new java.awt.event.ItemListener()
    {
      public void itemStateChanged(java.awt.event.ItemEvent evt)
      {
        cbCopyExeItemStateChanged(evt);
      }
    });

    cbRestartNaoqi.setText("restartNaoqi");
    cbRestartNaoqi.addItemListener(new java.awt.event.ItemListener()
    {
      public void itemStateChanged(java.awt.event.ItemEvent evt)
      {
        cbRestartNaoqiItemStateChanged(evt);
      }
    });
    cbRestartNaoqi.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        cbRestartNaoqiActionPerformed(evt);
      }
    });

    cbForceBackup.setText("force full Backup");
    cbForceBackup.addItemListener(new java.awt.event.ItemListener()
    {
      public void itemStateChanged(java.awt.event.ItemEvent evt)
      {
        cbForceBackupItemStateChanged(evt);
      }
    });
    cbForceBackup.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        cbForceBackupActionPerformed(evt);
      }
    });

    org.jdesktop.layout.GroupLayout jPanel1Layout = new org.jdesktop.layout.GroupLayout(jPanel1);
    jPanel1.setLayout(jPanel1Layout);
    jPanel1Layout.setHorizontalGroup(
      jPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
      .add(jPanel1Layout.createSequentialGroup()
        .addContainerGap()
        .add(jPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
          .add(jPanel1Layout.createSequentialGroup()
            .add(jPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
              .add(jSettingsPanel, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
              .add(jPanel1Layout.createSequentialGroup()
                .add(jPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
                  .add(cbCopyExe)
                  .add(cbCopyLib))
                .addPreferredGap(org.jdesktop.layout.LayoutStyle.UNRELATED)
                .add(jPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
                  .add(cbCopyConfig)
                  .add(cbCopyLogs))
                .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                .add(jPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
                  .add(jPanel1Layout.createSequentialGroup()
                    .add(cbRestartNaoth)
                    .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                    .add(cbForceBackup))
                  .add(jPanel1Layout.createSequentialGroup()
                    .add(cbRestartNaoqi)
                    .addPreferredGap(org.jdesktop.layout.LayoutStyle.UNRELATED)
                    .add(cbNoBackup)))))
            .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
            .add(jPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
              .add(jPanel1Layout.createSequentialGroup()
                .add(jLabel16)
                .add(0, 132, Short.MAX_VALUE))
              .add(jScrollPane2))
            .addContainerGap())
          .add(jActionsPanel, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)))
    );
    jPanel1Layout.setVerticalGroup(
      jPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
      .add(jPanel1Layout.createSequentialGroup()
        .addContainerGap()
        .add(jActionsPanel, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
        .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
        .add(jPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
          .add(jPanel1Layout.createSequentialGroup()
            .add(jSettingsPanel, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
            .add(11, 11, 11))
          .add(jPanel1Layout.createSequentialGroup()
            .add(jLabel16)
            .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
            .add(jScrollPane2, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 93, Short.MAX_VALUE)
            .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)))
        .add(jPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
          .add(jPanel1Layout.createSequentialGroup()
            .add(jPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
              .add(cbCopyConfig)
              .add(cbRestartNaoth)
              .add(cbForceBackup, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 23, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
            .add(8, 8, 8)
            .add(jPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
              .add(cbCopyLogs)
              .add(cbRestartNaoqi)
              .add(cbNoBackup, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 23, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)))
          .add(jPanel1Layout.createSequentialGroup()
            .add(cbCopyExe)
            .add(8, 8, 8)
            .add(cbCopyLib)))
        .addContainerGap(19, Short.MAX_VALUE))
    );

    jTabbedPane1.addTab("Copy & Run", jPanel1);

    jPanel2.setPreferredSize(new java.awt.Dimension(456, 462));
    jPanel2.setVerifyInputWhenFocusTarget(false);

    jSettingsPanel1.setBackground(new java.awt.Color(204, 204, 255));

    jLabel13.setText("LAN:");

    jLabel1.setText("SubNet");

    jLabel20.setText("Netmask");

    jLabel22.setText("Broadcast");

    subnetFieldLAN.setText("10.0.0");
    subnetFieldLAN.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        subnetFieldLANActionPerformed(evt);
      }
    });

    netmaskFieldLAN.setText("255.255.255.0");
    netmaskFieldLAN.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        netmaskFieldLANActionPerformed(evt);
      }
    });

    broadcastFieldLAN.setText("10.0.0.255");
    broadcastFieldLAN.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        broadcastFieldLANActionPerformed(evt);
      }
    });
    broadcastFieldLAN.addInputMethodListener(new java.awt.event.InputMethodListener()
    {
      public void inputMethodTextChanged(java.awt.event.InputMethodEvent evt)
      {
        broadcastFieldLANInputMethodTextChanged(evt);
      }
      public void caretPositionChanged(java.awt.event.InputMethodEvent evt)
      {
      }
    });
    broadcastFieldLAN.addKeyListener(new java.awt.event.KeyAdapter()
    {
      public void keyPressed(java.awt.event.KeyEvent evt)
      {
        broadcastFieldLANKeyPressed(evt);
      }
    });

    jLabel14.setText("WLAN:");

    jLabel19.setText("SubNet");

    jLabel21.setText("Netmask");

    jLabel23.setText("Broadcast");

    subnetFieldWLAN.setText("192.168.4");
    subnetFieldWLAN.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        subnetFieldWLANActionPerformed(evt);
      }
    });

    netmaskFieldWLAN.setText("255.255.255.0");
    netmaskFieldWLAN.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        netmaskFieldWLANActionPerformed(evt);
      }
    });

    broadcastFieldWLAN.setText("192.168.4.255");
    broadcastFieldWLAN.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        broadcastFieldWLANActionPerformed(evt);
      }
    });
    broadcastFieldWLAN.addInputMethodListener(new java.awt.event.InputMethodListener()
    {
      public void inputMethodTextChanged(java.awt.event.InputMethodEvent evt)
      {
        broadcastFieldWLANInputMethodTextChanged(evt);
      }
      public void caretPositionChanged(java.awt.event.InputMethodEvent evt)
      {
        broadcastFieldWLANCaretPositionChanged(evt);
      }
    });
    broadcastFieldWLAN.addPropertyChangeListener(new java.beans.PropertyChangeListener()
    {
      public void propertyChange(java.beans.PropertyChangeEvent evt)
      {
        broadcastFieldWLANPropertyChange(evt);
      }
    });
    broadcastFieldWLAN.addKeyListener(new java.awt.event.KeyAdapter()
    {
      public void keyTyped(java.awt.event.KeyEvent evt)
      {
        broadcastFieldWLANKeyTyped(evt);
      }
      public void keyPressed(java.awt.event.KeyEvent evt)
      {
        broadcastFieldWLANKeyPressed(evt);
      }
    });
    broadcastFieldWLAN.addVetoableChangeListener(new java.beans.VetoableChangeListener()
    {
      public void vetoableChange(java.beans.PropertyChangeEvent evt)throws java.beans.PropertyVetoException
      {
        broadcastFieldWLANVetoableChange(evt);
      }
    });

    radioWPA.setBackground(new java.awt.Color(204, 204, 255));
    radioWPA.setSelected(true);
    radioWPA.setText("WPA PSK");

    radioWEP.setBackground(new java.awt.Color(204, 204, 255));
    radioWEP.setText("WEP");

    jLabel5.setText("SSID");

    wlanSSID.setText("SPL_Field_B");
    wlanSSID.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        wlanSSIDActionPerformed(evt);
      }
    });

    jLabel9.setText("WLAN Key");

    wlanKey.setText("SPLRC2012");
    wlanKey.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        wlanKeyActionPerformed(evt);
      }
    });

    org.jdesktop.layout.GroupLayout jSettingsPanel1Layout = new org.jdesktop.layout.GroupLayout(jSettingsPanel1);
    jSettingsPanel1.setLayout(jSettingsPanel1Layout);
    jSettingsPanel1Layout.setHorizontalGroup(
      jSettingsPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
      .add(jSettingsPanel1Layout.createSequentialGroup()
        .add(jSettingsPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
          .add(org.jdesktop.layout.GroupLayout.TRAILING, jSettingsPanel1Layout.createSequentialGroup()
            .add(12, 12, 12)
            .add(jSettingsPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
              .add(jSettingsPanel1Layout.createSequentialGroup()
                .add(jLabel13)
                .add(217, 217, 217))
              .add(jSettingsPanel1Layout.createSequentialGroup()
                .add(jSettingsPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
                  .add(jLabel20, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 66, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
                  .add(jLabel1)
                  .add(jLabel22))
                .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                .add(jSettingsPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.TRAILING)
                  .add(org.jdesktop.layout.GroupLayout.LEADING, subnetFieldLAN, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 173, Short.MAX_VALUE)
                  .add(org.jdesktop.layout.GroupLayout.LEADING, netmaskFieldLAN, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 173, Short.MAX_VALUE)
                  .add(org.jdesktop.layout.GroupLayout.LEADING, broadcastFieldLAN, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 173, Short.MAX_VALUE))
                .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                .add(jSettingsPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
                  .add(jLabel14)
                  .add(jSettingsPanel1Layout.createSequentialGroup()
                    .add(jSettingsPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.TRAILING)
                      .add(jSettingsPanel1Layout.createSequentialGroup()
                        .add(jSettingsPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
                          .add(jLabel21)
                          .add(jLabel19))
                        .add(28, 28, 28))
                      .add(jSettingsPanel1Layout.createSequentialGroup()
                        .add(jLabel23)
                        .add(18, 18, 18)))
                    .add(jSettingsPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
                      .add(broadcastFieldWLAN, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 139, Short.MAX_VALUE)
                      .add(subnetFieldWLAN, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 139, Short.MAX_VALUE)
                      .add(netmaskFieldWLAN, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 139, Short.MAX_VALUE)
                      .add(radioWPA, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 100, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)))))))
          .add(jSettingsPanel1Layout.createSequentialGroup()
            .addContainerGap()
            .add(jSettingsPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
              .add(org.jdesktop.layout.GroupLayout.TRAILING, jSettingsPanel1Layout.createSequentialGroup()
                .add(84, 84, 84)
                .add(jSettingsPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
                  .add(wlanKey, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 397, Short.MAX_VALUE)
                  .add(org.jdesktop.layout.GroupLayout.TRAILING, jSettingsPanel1Layout.createSequentialGroup()
                    .add(wlanSSID, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 169, Short.MAX_VALUE)
                    .addPreferredGap(org.jdesktop.layout.LayoutStyle.UNRELATED)
                    .add(radioWEP)
                    .add(168, 168, 168))))
              .add(jLabel5)
              .add(jLabel9))))
        .add(13, 13, 13))
    );
    jSettingsPanel1Layout.setVerticalGroup(
      jSettingsPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
      .add(jSettingsPanel1Layout.createSequentialGroup()
        .addContainerGap()
        .add(jSettingsPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.TRAILING)
          .add(jSettingsPanel1Layout.createSequentialGroup()
            .add(jLabel14)
            .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
            .add(jSettingsPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
              .add(jLabel19)
              .add(subnetFieldWLAN, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
            .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
            .add(jSettingsPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
              .add(jLabel21)
              .add(netmaskFieldWLAN, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
            .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
            .add(broadcastFieldWLAN, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
          .add(jSettingsPanel1Layout.createSequentialGroup()
            .add(jLabel13)
            .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
            .add(jSettingsPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
              .add(subnetFieldLAN, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
              .add(jLabel1))
            .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
            .add(jSettingsPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
              .add(jLabel20)
              .add(netmaskFieldLAN, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
            .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
            .add(jSettingsPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
              .add(jLabel22)
              .add(broadcastFieldLAN, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
              .add(jLabel23))))
        .add(18, 18, 18)
        .add(jSettingsPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
          .add(jLabel5)
          .add(wlanSSID, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
          .add(radioWEP)
          .add(radioWPA))
        .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
        .add(jSettingsPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
          .add(jLabel9)
          .add(wlanKey, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
        .addContainerGap(15, Short.MAX_VALUE))
    );

    jSettingsPanel2.setBackground(new java.awt.Color(204, 204, 255));

    jLabel24.setText("TeamComm:");

    lblTeamCommWLAN.setEditable(false);
    lblTeamCommWLAN.setText("192.168.4.255");
    lblTeamCommWLAN.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        lblTeamCommWLANActionPerformed(evt);
      }
    });

    jLabel25.setText("/");

    lblTeamCommLAN.setEditable(false);
    lblTeamCommLAN.setText("10.0.0.255");
    lblTeamCommLAN.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        lblTeamCommLANActionPerformed(evt);
      }
    });

    jLabel26.setText(":");

    jTeamCommPort.setText("10400");

    jLabel27.setText("ssh:");

    sshUser.setText("nao");
    sshUser.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        sshUserActionPerformed(evt);
      }
    });

    jLabel28.setText(":");

    sshPassword.setText("nao");

    jLabel29.setText("ssh:");

    sshRootUser.setEditable(false);
    sshRootUser.setText("root");
    sshRootUser.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        sshRootUserActionPerformed(evt);
      }
    });

    jLabel30.setText(":");

    sshRootPassword.setText("root");

    org.jdesktop.layout.GroupLayout jSettingsPanel2Layout = new org.jdesktop.layout.GroupLayout(jSettingsPanel2);
    jSettingsPanel2.setLayout(jSettingsPanel2Layout);
    jSettingsPanel2Layout.setHorizontalGroup(
      jSettingsPanel2Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
      .add(jSettingsPanel2Layout.createSequentialGroup()
        .addContainerGap()
        .add(jSettingsPanel2Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
          .add(jSettingsPanel2Layout.createSequentialGroup()
            .add(jLabel24)
            .add(18, 18, 18)
            .add(lblTeamCommWLAN, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 131, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
            .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
            .add(jLabel25)
            .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
            .add(lblTeamCommLAN, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 111, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
            .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
            .add(jLabel26)
            .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
            .add(jTeamCommPort, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 124, Short.MAX_VALUE))
          .add(org.jdesktop.layout.GroupLayout.TRAILING, jSettingsPanel2Layout.createSequentialGroup()
            .add(jSettingsPanel2Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.TRAILING, false)
              .add(org.jdesktop.layout.GroupLayout.LEADING, jSettingsPanel2Layout.createSequentialGroup()
                .add(jLabel29)
                .add(18, 18, 18)
                .add(sshRootUser, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 74, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                .add(jLabel30, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
              .add(jSettingsPanel2Layout.createSequentialGroup()
                .add(jLabel27)
                .add(18, 18, 18)
                .add(sshUser, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 74, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                .add(jLabel28)))
            .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
            .add(jSettingsPanel2Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
              .add(sshPassword, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 339, Short.MAX_VALUE)
              .add(sshRootPassword, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 339, Short.MAX_VALUE))))
        .addContainerGap())
    );
    jSettingsPanel2Layout.setVerticalGroup(
      jSettingsPanel2Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
      .add(jSettingsPanel2Layout.createSequentialGroup()
        .addContainerGap()
        .add(jSettingsPanel2Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
          .add(jLabel24, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 20, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
          .add(lblTeamCommWLAN, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
          .add(jLabel25, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 26, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
          .add(lblTeamCommLAN, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
          .add(jLabel26, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 26, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
          .add(jTeamCommPort, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
        .addPreferredGap(org.jdesktop.layout.LayoutStyle.UNRELATED)
        .add(jSettingsPanel2Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
          .add(jLabel27)
          .add(sshUser, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
          .add(jLabel28, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 26, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
          .add(sshPassword, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
        .addPreferredGap(org.jdesktop.layout.LayoutStyle.UNRELATED)
        .add(jSettingsPanel2Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
          .add(jLabel30, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 26, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
          .add(sshRootUser, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
          .add(jLabel29)
          .add(sshRootPassword, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
        .addContainerGap(org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
    );

    jButtonSetRobotNetwork.setText("Set Network to Robot");
    jButtonSetRobotNetwork.setEnabled(false);
    jButtonSetRobotNetwork.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        jButtonSetRobotNetworkActionPerformed(evt);
      }
    });

    jButtonInitRobotSystem.setText("Initialize Robot System");
    jButtonInitRobotSystem.setEnabled(false);
    jButtonInitRobotSystem.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        jButtonInitRobotSystemActionPerformed(evt);
      }
    });

    jButtonSaveNetworkConfig.setText("Save As Default Config");
    jButtonSaveNetworkConfig.setEnabled(false);
    jButtonSaveNetworkConfig.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        jButtonSaveNetworkConfigActionPerformed(evt);
      }
    });

    lstNaos.setSelectionMode(javax.swing.ListSelectionModel.SINGLE_SELECTION);
    lstNaos.addMouseListener(new java.awt.event.MouseAdapter()
    {
      public void mouseClicked(java.awt.event.MouseEvent evt)
      {
        lstNaosMouseClicked(evt);
      }
    });
    jScrollPane3.setViewportView(lstNaos);

    cbRebootSystem.setText("reboot OS");

    jButtonRemoteKernelVideoReload.setText("Reload kernel video module");
    jButtonRemoteKernelVideoReload.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        jButtonRemoteKernelVideoReloadActionPerformed(evt);
      }
    });

    org.jdesktop.layout.GroupLayout jPanel2Layout = new org.jdesktop.layout.GroupLayout(jPanel2);
    jPanel2.setLayout(jPanel2Layout);
    jPanel2Layout.setHorizontalGroup(
      jPanel2Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
      .add(jPanel2Layout.createSequentialGroup()
        .addContainerGap()
        .add(jPanel2Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
          .add(jSettingsPanel1, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
          .add(jSettingsPanel2, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        .addPreferredGap(org.jdesktop.layout.LayoutStyle.UNRELATED)
        .add(jPanel2Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
          .add(cbRebootSystem, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
          .add(jButtonSetRobotNetwork, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
          .add(org.jdesktop.layout.GroupLayout.TRAILING, jButtonInitRobotSystem, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 205, Short.MAX_VALUE)
          .add(jButtonSaveNetworkConfig, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 199, Short.MAX_VALUE)
          .add(jScrollPane3, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 0, Short.MAX_VALUE)
          .add(jButtonRemoteKernelVideoReload, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 199, Short.MAX_VALUE))
        .addContainerGap())
    );
    jPanel2Layout.setVerticalGroup(
      jPanel2Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
      .add(org.jdesktop.layout.GroupLayout.TRAILING, jPanel2Layout.createSequentialGroup()
        .add(jPanel2Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.TRAILING)
          .add(jPanel2Layout.createSequentialGroup()
            .addContainerGap()
            .add(cbRebootSystem)
            .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
            .add(jScrollPane3))
          .add(jPanel2Layout.createSequentialGroup()
            .add(14, 14, 14)
            .add(jSettingsPanel1, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)))
        .addPreferredGap(org.jdesktop.layout.LayoutStyle.UNRELATED)
        .add(jPanel2Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
          .add(org.jdesktop.layout.GroupLayout.TRAILING, jSettingsPanel2, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
          .add(org.jdesktop.layout.GroupLayout.TRAILING, jPanel2Layout.createSequentialGroup()
            .add(jButtonRemoteKernelVideoReload)
            .addPreferredGap(org.jdesktop.layout.LayoutStyle.UNRELATED)
            .add(jButtonSaveNetworkConfig, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 27, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
            .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
            .add(jButtonSetRobotNetwork, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 27, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
            .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
            .add(jButtonInitRobotSystem, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 27, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)))
        .addContainerGap())
    );

    jTabbedPane1.addTab("Network Configuration / Utilities", jPanel2);

    jSplitPane1.setLeftComponent(jTabbedPane1);

    logTextPane.setEditable(false);
    jScrollPane5.setViewportView(logTextPane);

    jCopyStatus.setFont(new java.awt.Font("Lucida Grande", 0, 10)); // NOI18N
    jCopyStatus.setText("idle...");

    progressBar.setEnabled(false);

    org.jdesktop.layout.GroupLayout jPanel3Layout = new org.jdesktop.layout.GroupLayout(jPanel3);
    jPanel3.setLayout(jPanel3Layout);
    jPanel3Layout.setHorizontalGroup(
      jPanel3Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
      .add(jCopyStatus, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
      .add(progressBar, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 261, Short.MAX_VALUE)
      .add(jScrollPane5)
    );
    jPanel3Layout.setVerticalGroup(
      jPanel3Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
      .add(jPanel3Layout.createSequentialGroup()
        .add(jScrollPane5, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 331, Short.MAX_VALUE)
        .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
        .add(jCopyStatus, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 19, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
        .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
        .add(progressBar, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
    );

    jSplitPane1.setRightComponent(jPanel3);

    org.jdesktop.layout.GroupLayout layout = new org.jdesktop.layout.GroupLayout(getContentPane());
    getContentPane().setLayout(layout);
    layout.setHorizontalGroup(
      layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
      .add(layout.createSequentialGroup()
        .addContainerGap()
        .add(layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
          .add(layout.createSequentialGroup()
            .add(jLabel12)
            .add(0, 0, Short.MAX_VALUE))
          .add(jSplitPane1, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 1069, Short.MAX_VALUE))
        .addContainerGap())
    );
    layout.setVerticalGroup(
      layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
      .add(layout.createSequentialGroup()
        .add(jLabel12)
        .add(2, 2, 2)
        .add(jSplitPane1, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 378, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
        .addContainerGap(org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
    );

    pack();
  }// </editor-fold>//GEN-END:initComponents

    private void subnetFieldLANActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_subnetFieldLANActionPerformed
      // TODO add your handling code here:
    }//GEN-LAST:event_subnetFieldLANActionPerformed

    private void cbCopyLogsActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_cbCopyLogsActionPerformed
      // TODO add your handling code here:
}//GEN-LAST:event_cbCopyLogsActionPerformed

    private void cbNoBackupActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_cbNoBackupActionPerformed
    }//GEN-LAST:event_cbNoBackupActionPerformed

    private void jBackupBoxItemStateChanged(java.awt.event.ItemEvent evt) {//GEN-FIRST:event_jBackupBoxItemStateChanged
      
      String itemValue = evt.getItem().toString();

      if(jBackupBox.getItemCount() > 0 && jBackupBox.getSelectedItem().equals(itemValue) && !itemValue.equals(jBackupBox.getItemAt(0)))
      {
        config.backupIsSelected = true;
        config.boxSelected = jBackupBox.getSelectedItem().toString();
        config.selectedBackup = config.backups.get(jBackupBox.getSelectedItem()).toString();
      }
      else
      {
        config.boxSelected = "";
        config.selectedBackup = "";
        config.backupIsSelected = false;
      }
      setCommentText();
    }//GEN-LAST:event_jBackupBoxItemStateChanged

    private void cbCopyConfigItemStateChanged(java.awt.event.ItemEvent evt) {//GEN-FIRST:event_cbCopyConfigItemStateChanged
      setActionBtnLabel();
    }//GEN-LAST:event_cbCopyConfigItemStateChanged

    private void cbCopyLibItemStateChanged(java.awt.event.ItemEvent evt) {//GEN-FIRST:event_cbCopyLibItemStateChanged
      setActionBtnLabel();
    }//GEN-LAST:event_cbCopyLibItemStateChanged

    private void naoByte4ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_naoByte4ActionPerformed
      // TODO add your handling code here:
    }//GEN-LAST:event_naoByte4ActionPerformed

    private void jButtonRefreshDataActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButtonRefreshDataActionPerformed
      setFormData();
    }//GEN-LAST:event_jButtonRefreshDataActionPerformed

    private void cbRestartNaothItemStateChanged(java.awt.event.ItemEvent evt) {//GEN-FIRST:event_cbRestartNaothItemStateChanged
      setActionBtnLabel();
    }//GEN-LAST:event_cbRestartNaothItemStateChanged

    private void cbRestartNaothActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_cbRestartNaothActionPerformed
      
    }//GEN-LAST:event_cbRestartNaothActionPerformed

    private void cbCopyExeItemStateChanged(java.awt.event.ItemEvent evt) {//GEN-FIRST:event_cbCopyExeItemStateChanged
      setActionBtnLabel();
    }//GEN-LAST:event_cbCopyExeItemStateChanged

    private void subnetFieldWLANActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_subnetFieldWLANActionPerformed
      // TODO add your handling code here:
    }//GEN-LAST:event_subnetFieldWLANActionPerformed

    private void netmaskFieldLANActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_netmaskFieldLANActionPerformed
      // TODO add your handling code here:
    }//GEN-LAST:event_netmaskFieldLANActionPerformed

    private void netmaskFieldWLANActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_netmaskFieldWLANActionPerformed
      // TODO add your handling code here:
    }//GEN-LAST:event_netmaskFieldWLANActionPerformed

    private void broadcastFieldLANActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_broadcastFieldLANActionPerformed
      // TODO add your handling code here:
    }//GEN-LAST:event_broadcastFieldLANActionPerformed

    private void broadcastFieldWLANActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_broadcastFieldWLANActionPerformed
      lblTeamCommWLAN.setText(broadcastFieldWLAN.getText());
    }//GEN-LAST:event_broadcastFieldWLANActionPerformed

    private void lblTeamCommWLANActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_lblTeamCommWLANActionPerformed
      // TODO add your handling code here:
    }//GEN-LAST:event_lblTeamCommWLANActionPerformed

    private void lblTeamCommLANActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_lblTeamCommLANActionPerformed
      // TODO add your handling code here:
    }//GEN-LAST:event_lblTeamCommLANActionPerformed

    private void sshUserActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_sshUserActionPerformed
      // TODO add your handling code here:
    }//GEN-LAST:event_sshUserActionPerformed

    private void sshRootUserActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_sshRootUserActionPerformed
      // TODO add your handling code here:
    }//GEN-LAST:event_sshRootUserActionPerformed

    private void jButtonSetRobotNetworkActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButtonSetRobotNetworkActionPerformed
      if(checkDirPath(true))
      {
        setRobotNetwork();
      }
    }//GEN-LAST:event_jButtonSetRobotNetworkActionPerformed

    private void jButtonSaveNetworkConfigActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButtonSaveNetworkConfigActionPerformed
      try
      {
        actionInfo("saving network configuration");
        writeNetworkConfig();
        saveWpaSupplicant(config, config.localSetupScriptPath());
        writePlayerCfg(new File(config.localConfigPath() + "/general/player.cfg"), "1");
        writeTeamcommCfg(new File(config.localConfigPath() + "/general/teamcomm.cfg"));
        writeScheme(new File(config.localConfigPath() + "/scheme.cfg"));
      }
      catch(IOException ex)
      {
        actionInfo("Could'nt write network config file\n"  + ex.getMessage());
      }
    }//GEN-LAST:event_jButtonSaveNetworkConfigActionPerformed

    private void lstNaosMouseClicked(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_lstNaosMouseClicked
      
      if (evt.getClickCount() == 2 && checkDirPath(true))
      {
        Object[] options={ "initialize Robot", "set network config" };
        int pressedBtnId = JOptionPane.showOptionDialog
                            (
                              null, "Choose or loose!",
                              "Demand", JOptionPane.DEFAULT_OPTION, 
                              JOptionPane.INFORMATION_MESSAGE,
                              null, options, options[0]
                            );
        if(pressedBtnId == 0)
        {
          initializeRobot();
        }
        else if(pressedBtnId == 1)
        {
          setRobotNetwork();
        }
      }
    }//GEN-LAST:event_lstNaosMouseClicked
       
    private void formWindowClosing(java.awt.event.WindowEvent evt) {//GEN-FIRST:event_formWindowClosing
//      for(int i = 0; i < jmdnsList.size() && i < jmdnsServiceListenerList.size(); i++)
//      {
//        JmDNS jmdns = jmdnsList.get(i);
//        ServiceListener svcListener = jmdnsServiceListenerList.get(i);
//        try
//        {
//          jmdns.removeServiceListener("_nao._tcp.local.", svcListener);
//          jmdns.close();
//        }
//        catch(Exception e){}
//      }
    }//GEN-LAST:event_formWindowClosing

    private void jButtonInitRobotSystemActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButtonInitRobotSystemActionPerformed
      if(checkDirPath(true))
      {
        initializeRobot();
      }
    }//GEN-LAST:event_jButtonInitRobotSystemActionPerformed

    private void jSchemeBoxItemStateChanged(java.awt.event.ItemEvent evt) {//GEN-FIRST:event_jSchemeBoxItemStateChanged
    }//GEN-LAST:event_jSchemeBoxItemStateChanged

    private void broadcastFieldLANInputMethodTextChanged(java.awt.event.InputMethodEvent evt) {//GEN-FIRST:event_broadcastFieldLANInputMethodTextChanged
    }//GEN-LAST:event_broadcastFieldLANInputMethodTextChanged

    private void broadcastFieldWLANInputMethodTextChanged(java.awt.event.InputMethodEvent evt) {//GEN-FIRST:event_broadcastFieldWLANInputMethodTextChanged
    }//GEN-LAST:event_broadcastFieldWLANInputMethodTextChanged

    private void broadcastFieldWLANPropertyChange(java.beans.PropertyChangeEvent evt) {//GEN-FIRST:event_broadcastFieldWLANPropertyChange
    }//GEN-LAST:event_broadcastFieldWLANPropertyChange

    private void broadcastFieldWLANVetoableChange(java.beans.PropertyChangeEvent evt)throws java.beans.PropertyVetoException {//GEN-FIRST:event_broadcastFieldWLANVetoableChange
    }//GEN-LAST:event_broadcastFieldWLANVetoableChange

    private void broadcastFieldWLANCaretPositionChanged(java.awt.event.InputMethodEvent evt) {//GEN-FIRST:event_broadcastFieldWLANCaretPositionChanged
    }//GEN-LAST:event_broadcastFieldWLANCaretPositionChanged

    private void broadcastFieldWLANKeyTyped(java.awt.event.KeyEvent evt) {//GEN-FIRST:event_broadcastFieldWLANKeyTyped
    }//GEN-LAST:event_broadcastFieldWLANKeyTyped

    private void broadcastFieldWLANKeyPressed(java.awt.event.KeyEvent evt) {//GEN-FIRST:event_broadcastFieldWLANKeyPressed
      lblTeamCommWLAN.setText(broadcastFieldWLAN.getText());
    }//GEN-LAST:event_broadcastFieldWLANKeyPressed

    private void broadcastFieldLANKeyPressed(java.awt.event.KeyEvent evt) {//GEN-FIRST:event_broadcastFieldLANKeyPressed
      lblTeamCommLAN.setText(broadcastFieldLAN.getText());
    }//GEN-LAST:event_broadcastFieldLANKeyPressed

  private void cbNoBackupItemStateChanged(java.awt.event.ItemEvent evt) {//GEN-FIRST:event_cbNoBackupItemStateChanged
    setActionBtnLabel();
  }//GEN-LAST:event_cbNoBackupItemStateChanged

  private void wlanSSIDActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_wlanSSIDActionPerformed
    // TODO add your handling code here:
  }//GEN-LAST:event_wlanSSIDActionPerformed

  private void cbRestartNaoqiItemStateChanged(java.awt.event.ItemEvent evt) {//GEN-FIRST:event_cbRestartNaoqiItemStateChanged
    setActionBtnLabel();
  }//GEN-LAST:event_cbRestartNaoqiItemStateChanged

  private void cbRestartNaoqiActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_cbRestartNaoqiActionPerformed
    // TODO add your handling code here:
  }//GEN-LAST:event_cbRestartNaoqiActionPerformed

  private void cbForceBackupItemStateChanged(java.awt.event.ItemEvent evt) {//GEN-FIRST:event_cbForceBackupItemStateChanged
    setActionBtnLabel();
  }//GEN-LAST:event_cbForceBackupItemStateChanged

  private void cbForceBackupActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_cbForceBackupActionPerformed
    // TODO add your handling code here:
  }//GEN-LAST:event_cbForceBackupActionPerformed

  private void cbCopyLogsItemStateChanged(java.awt.event.ItemEvent evt) {//GEN-FIRST:event_cbCopyLogsItemStateChanged
    setActionBtnLabel();
  }//GEN-LAST:event_cbCopyLogsItemStateChanged

  private void jButtonRemoteKernelVideoReloadActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButtonRemoteKernelVideoReloadActionPerformed
    remoteReloadKernelVideoModule();
  }//GEN-LAST:event_jButtonRemoteKernelVideoReloadActionPerformed

  private void wlanKeyActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_wlanKeyActionPerformed
    // TODO add your handling code here:
  }//GEN-LAST:event_wlanKeyActionPerformed

  // Variables declaration - do not modify//GEN-BEGIN:variables
  private javax.swing.JTextField broadcastFieldLAN;
  private javax.swing.JTextField broadcastFieldWLAN;
  private javax.swing.JCheckBox cbCopyConfig;
  private javax.swing.JCheckBox cbCopyExe;
  private javax.swing.JCheckBox cbCopyLib;
  private javax.swing.JCheckBox cbCopyLogs;
  private javax.swing.JCheckBox cbForceBackup;
  private javax.swing.JCheckBox cbNoBackup;
  private javax.swing.JCheckBox cbRebootSystem;
  private javax.swing.JCheckBox cbRestartNaoqi;
  private javax.swing.JCheckBox cbRestartNaoth;
  private javax.swing.JButton copyButton;
  private javax.swing.JPanel jActionsPanel;
  private javax.swing.JComboBox jBackupBox;
  private javax.swing.JButton jButtonInitRobotSystem;
  private javax.swing.JButton jButtonRefreshData;
  private javax.swing.JButton jButtonRemoteKernelVideoReload;
  private javax.swing.JButton jButtonSaveNetworkConfig;
  private javax.swing.JButton jButtonSetRobotNetwork;
  private javax.swing.JComboBox jColorBox;
  private javax.swing.JTextArea jCommentTextArea;
  private javax.swing.JLabel jCopyStatus;
  private javax.swing.JDialog jDialog1;
  private javax.swing.JButton jDirChooser;
  private javax.swing.JLabel jDirPathLabel;
  private javax.swing.JLabel jLabel1;
  private javax.swing.JLabel jLabel12;
  private javax.swing.JLabel jLabel13;
  private javax.swing.JLabel jLabel14;
  private javax.swing.JLabel jLabel15;
  private javax.swing.JLabel jLabel16;
  private javax.swing.JLabel jLabel17;
  private javax.swing.JLabel jLabel19;
  private javax.swing.JLabel jLabel2;
  private javax.swing.JLabel jLabel20;
  private javax.swing.JLabel jLabel21;
  private javax.swing.JLabel jLabel22;
  private javax.swing.JLabel jLabel23;
  private javax.swing.JLabel jLabel24;
  private javax.swing.JLabel jLabel25;
  private javax.swing.JLabel jLabel26;
  private javax.swing.JLabel jLabel27;
  private javax.swing.JLabel jLabel28;
  private javax.swing.JLabel jLabel29;
  private javax.swing.JLabel jLabel3;
  private javax.swing.JLabel jLabel30;
  private javax.swing.JLabel jLabel4;
  private javax.swing.JLabel jLabel5;
  private javax.swing.JLabel jLabel6;
  private javax.swing.JLabel jLabel7;
  private javax.swing.JLabel jLabel8;
  private javax.swing.JLabel jLabel9;
  private javax.swing.JPanel jPanel1;
  private javax.swing.JPanel jPanel2;
  private javax.swing.JPanel jPanel3;
  private javax.swing.JComboBox jSchemeBox;
  private javax.swing.JScrollPane jScrollPane2;
  private javax.swing.JScrollPane jScrollPane3;
  private javax.swing.JScrollPane jScrollPane5;
  private javax.swing.JSeparator jSeparator1;
  private javax.swing.JPanel jSettingsPanel;
  private javax.swing.JPanel jSettingsPanel1;
  private javax.swing.JPanel jSettingsPanel2;
  private javax.swing.JSplitPane jSplitPane1;
  private javax.swing.JTabbedPane jTabbedPane1;
  private javax.swing.JTextField jTeamCommPort;
  private javax.swing.JTextField jTeamNumber;
  private javax.swing.JTextField lblTeamCommLAN;
  private javax.swing.JTextField lblTeamCommWLAN;
  private javax.swing.JTextPane logTextPane;
  private javax.swing.JList lstNaos;
  private javax.swing.JTextField naoByte1;
  private javax.swing.JTextField naoByte2;
  private javax.swing.JTextField naoByte3;
  private javax.swing.JTextField naoByte4;
  private javax.swing.JTextField netmaskFieldLAN;
  private javax.swing.JTextField netmaskFieldWLAN;
  private javax.swing.JProgressBar progressBar;
  private javax.swing.JRadioButton radioWEP;
  private javax.swing.JRadioButton radioWPA;
  private javax.swing.JPasswordField sshPassword;
  private javax.swing.JPasswordField sshRootPassword;
  private javax.swing.JTextField sshRootUser;
  private javax.swing.JTextField sshUser;
  private javax.swing.JTextField subnetFieldLAN;
  private javax.swing.JTextField subnetFieldWLAN;
  private javax.swing.ButtonGroup wlanBtnGroup;
  private javax.swing.JPasswordField wlanKey;
  private javax.swing.JTextField wlanSSID;
  // End of variables declaration//GEN-END:variables
};


