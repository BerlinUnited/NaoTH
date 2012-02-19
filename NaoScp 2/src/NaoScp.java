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



import javax.swing.*;
import java.io.*;
import java.util.*;
//import java.util.regex.*;
// import java.util.Arrays;
import java.net.*;
import java.text.*;
import java.awt.Component;
import javax.jmdns.ServiceEvent;
import javax.jmdns.ServiceInfo;
import javax.jmdns.ServiceListener;
import javax.jmdns.JmDNS;
//import javax.jmdns.JmmDNS;
import static net.jcores.jre.CoreKeeper.$;
import net.jcores.jre.interfaces.functions.F0R;
//import net.jcores.jre.utils.internal.wrapper.InputStreamWrapper;

public class NaoScp extends NaoScpMainFrame implements ServiceListener
{
  private boolean fhIsTesting = false;
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
  private HashMap<String, ArrayList<InetAddress>> hostAdresses = new HashMap<String, ArrayList<InetAddress>>();
  private List<NaoSshWrapper> services;
  private final DefaultListModel naoModel;
  
  private boolean showCopyDoneMsg = false;
  private boolean showScriptDoneMsg = false;
  private boolean showDoneMsg = true;
  
  private String setupPlayerNo;
  
  
  @SuppressWarnings("unchecked")
  public NaoScp()
  {
    initComponents();
    
    super.sshUser = this.sshUser;
    super.sshPassword = this.sshPassword;
    super.sshRootPassword = this.sshRootPassword;
    super.jLogWindow = this.jLogWindow;
    super.jBackupBox = this.jBackupBox;
    super.progressBar = this.progressBar;
    
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
    
    services = Collections.synchronizedList(new ArrayList<NaoSshWrapper>());
    try
    {      
      //get own interface ip addresses
      for (Enumeration<NetworkInterface> en = NetworkInterface.getNetworkInterfaces(); en.hasMoreElements();) 
      {
        NetworkInterface intf = en.nextElement();
        if(!intf.isLoopback() && intf.isUp())
        {
          ArrayList<InetAddress> interfaceAdresses = new ArrayList<InetAddress>();
          for (Enumeration<InetAddress> enumIpAddr = intf.getInetAddresses(); enumIpAddr.hasMoreElements(); ) 
          {
            InetAddress addr = enumIpAddr.nextElement();
            {
              interfaceAdresses.add(addr);
            }
          }
          hostAdresses.put(intf.getName(), interfaceAdresses);
        }
      }
     
      //hook jmdns service for each address
      for(String intf : hostAdresses.keySet())
      {
        if(hostAdresses.get(intf).size() > 0)
        {
          JmDNS j = JmDNS.create(hostAdresses.get(intf).get(0), intf);
          j.addServiceListener("_nao._tcp.local.", (ServiceListener) this);
//          j.addServiceListener("_ssh._tcp.local.", (ServiceListener) this);
          jmdnsList.add(j);
        }
      }
     
      $.async(new F0R<String>()
      {

        @Override
        public String f()
        {
//          for (ServiceInfo info : jmdns.list("_nao._tcp.local.", 1000))
//          {
//            services.add(new NaoSshWrapper(info, "nao"));
//          }
//          for (ServiceInfo info : jmdns.list("_ssh._tcp.local.", 1000))
//          {
//            services.add(new NaoSshWrapper(info, "ssh_1"));            
//          }
          for(JmDNS j : jmdnsList)
          {
            for (ServiceInfo info : j.list("_nao._tcp.local.", 1000))
            {
              services.add(new NaoSshWrapper(info, j.getName() + "_nao" ));            
            }
//            for (ServiceInfo info : j.list("_ssh._tcp.local.", 100))
//            {
//              services.add(new NaoSshWrapper(info, j.getName() + "_ssh" ));            
//            }
          }
          updateList();
          return "";
        }
      });
    }
    catch (IOException ex)
    {
//      Logger.getLogger(NaoScp.class.getName()).log(Level.SEVERE, null, ex);
    }
    
    
    this.cbNoBackup.setVisible(debugVersion);
    this.cbNoBackup.setEnabled(debugVersion);
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
      try
      {
        String ResourceName="NaoScp.class";
        String programPath = URLDecoder.decode(this.getClass().getClassLoader().getResource(ResourceName).getPath(), "UTF-8");
        programPath = programPath.replace("file:", "");
        programPath = programPath.replace("/NaoScp 2/dist/NaoScp.jar!/NaoScp.class", "");
        programPath = programPath.replace("/NaoScp 2/build/classes/NaoScp.class", "") + "/NaoTHSoccer";
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

    if(fhIsTesting)
    {
      remotePrefix = "/Users/robotest/deploy";
      subnetFieldLAN.setText("127.0.0");
      sshUser.setText("robotest");
      sshPassword.setText("robotest");
    }
  }

  private void updateList()
  {
    synchronized (naoModel)
    {
      naoModel.clear();
      for (NaoSshWrapper nao : services)
      {
        if (nao.isValid())
        {
          naoModel.addElement(nao);
        }
      }

    }
  }

  @Override
  public void serviceAdded(ServiceEvent event)
  {
    services.add(new NaoSshWrapper(event.getInfo()));
    updateList();
  }

  @Override
  public void serviceRemoved(ServiceEvent event)
  {
    services.remove(new NaoSshWrapper(event.getInfo()));
    updateList();
  }

  @Override
  public void serviceResolved(ServiceEvent event)
  {
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

  public String remoteRootPath(String sNaoNo)
  {
    if(fhIsTesting)
    {
      return remotePrefix + "/" + sNaoNo;
    }
    return remotePrefix;
  }

  public String setupScriptPath()
  {
    return homePath() + "/naoqi/naothSetup";
  }

  public String configPath()
  {
    return homePath() + "/naoqi/Config";
  }

  public String setupPath()
  {
    return homePath() + "/naoqi/Config";
  }

  public String libnaoPath()
  {
    return homePath() + "/bin";
  }
  
  public String naoqiPrefPath()
  {
    return homePath() + "/naoqi/preferences";
  }
  
  public String binPath()
  {
    return homePath() + "/bin";
  }

  public String homePath()
  {
    return "/home/nao";
  }

  public String localDeployRootPath()
  {
    return jDirPathLabel.getText() + "/Deploy";
  }

  public String localDeployOutPath(String sNaoNo)
  {
    return localDeployRootPath() + "/out/" + sNaoNo;
  }

  public String localDeployInPath(String sNaoNo, String sNaoByte)
  {
    return localDeployRootPath() + "/in/" + backupTimestamp + "-" + sNaoNo + "-" + String.valueOf(sNaoByte);
  }

  public String localSetupScriptPath()
  {
    return jDirPathLabel.getText() + "/../Misc_Gentoo/NaoConfigFiles";
  }

  public String localConfigPath()
  {
    return jDirPathLabel.getText() + "/Config";
  }

  public String localLibnaothPath()
  {
    return jDirPathLabel.getText() + "/dist/Nao";
  }

  private void readNetworkConfig() throws FileNotFoundException, IOException
  {
    FileReader fReader = new FileReader(localSetupScriptPath() + "/network.conf");        
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
    File file = new File(localSetupScriptPath() + "/network.conf");        
    if(!file.isFile())
    {
      file.createNewFile();
    }

    FileWriter fWriter = new FileWriter(localSetupScriptPath() + "/network.conf");

    actionInfo("writing NaoConfigFiles/network.conf");
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
  
  private void checkNaoIps()
  {
    for(Integer naoNo : this.iNaoBytes.keySet())
    {
      if(naoNo > 0)
      {
        iNaoBytes.put(naoNo, Integer.parseInt(naoNumberFields.get(naoNo).getText()));
      }
    }

    for(Integer naoNo : this.sNaoLanIps.keySet())
    {
      if(naoNo > 0)
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
        }
        else
        {
          sNaoLanIps.put(naoNo, "");
          sNaoWLanIps.put(naoNo, "");
        }
      }
    }

  }
    
    private void copyButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_copyButtonActionPerformed

      setFormEnabled(false);
      
      copyConfig = cbCopyConfig.isSelected();
      copyLib = cbCopyLib.isSelected();
      copyExe = cbCopyExe.isSelected();
      copyLogs = cbCopyLogs.isSelected();
      restartNaoth = cbRestartNaoth.isSelected();
      noBackup = cbNoBackup.isSelected();
      
      
      if(copyConfig || copyExe || copyLib || copyLogs)
      {
        showCopyDoneMsg = true;        
      }
      
      if(restartNaoth)
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
      
      jLogWindow.setText("");

      backupTimestamp = String.valueOf(System.currentTimeMillis());
      prepareDeploy();

      checkNaoIps();
      
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
        //if(iNaoBytes.get(naoNo) < 256 && iNaoBytes.get(naoNo) > -1)
        boolean connected = false;
        if(!sNaoLanIps.get(naoNo).equals(""))
        {
          sshCopier cLan = new sshCopier(this, sNaoLanIps.get(naoNo), naoNo, iNaoBytes.get(naoNo));
          if(cLan.testConnection())
          {
            cLan.execute();            
            synchronized(cLan)
            {
              while(!cLan.isDone())
              {
                this.validateTree();
                jLogWindow.updateUI();
              }
            }
            if(hadErrors.get(naoNo))
            {
              allIsDone(naoNo);
              return;
            }
            if (restartNaoth)
            {
              sshScriptRunner sLan = new sshScriptRunner(this, sNaoLanIps.get(naoNo), naoNo, iNaoBytes.get(naoNo), new String[] {"restartNaoTH"});
              sLan.execute();
              synchronized(sLan)
              {
                while(!sLan.isDone())
                {
                  this.validateTree();
                  jLogWindow.updateUI();
                }
              }
            }
            connected = true;
          }
          else
          {
            sshCopier cWlan = new sshCopier(this, sNaoWLanIps.get(naoNo), naoNo, iNaoBytes.get(naoNo));
            if(cWlan.testConnection())
            {
              cWlan.execute();
              synchronized(cWlan)
              {
                while(!cWlan.isDone())
                {
                  this.validateTree();
                  jLogWindow.updateUI();
                }
              }
              if(hadErrors.get(naoNo))
              {
                allIsDone(naoNo);
                return;
              }
              if (restartNaoth)
              {
                sshScriptRunner sWlan = new sshScriptRunner(this, sNaoWLanIps.get(naoNo), naoNo, iNaoBytes.get(naoNo), new String[] {"restartNaoTH"});
                sWlan.execute();
                synchronized(sWlan)
                {
                  while(!sWlan.isDone())
                  {
                    this.validateTree();
                    jLogWindow.updateUI();
                  }
                }
              }
              connected = true;
            }
          }
        }
        
        if(!connected)
        {
          haveError(naoNo, "Nao " + naoNo + " on LAN(" + sNaoLanIps.get(naoNo) + ") & WLAN(" + sNaoWLanIps.get(naoNo) + ") not reachable");          
        }
        allIsDone(naoNo);
      }
    }//GEN-LAST:event_copyButtonActionPerformed

  public void haveError(String sNaoNo, String error)
  {
     haveCopyError(Integer.parseInt(sNaoNo), error);
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
    haveError(naoNo, "copy error: " + error);
  }

  public void haveScriptError(String sNaoNo, String error)
  {
     haveScriptError(Integer.parseInt(sNaoNo), error);
  }
  
  public void haveScriptError(int naoNo, String error)
  {
    hadScriptErrors.put(naoNo, true);
    haveError(naoNo, "script error: " + error);
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
        actionInfo("Copy + Scripts done");
        Thread.yield();
        JOptionPane.showMessageDialog(null, "Copy + Scripts done" + add);
        setFormEnabled(true);
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
          actionInfo("Copy done");
          Thread.yield();
          JOptionPane.showMessageDialog(null, "Copy done" + add);
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
          actionInfo("Scripts done");
          Thread.yield();
          JOptionPane.showMessageDialog(null, "Scripts done" + add);
        }
      }
      resetBackupList();    
      setFormEnabled(true);
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
//      jSchemeBox.setEnabled(enable);
//      lblTeamCommWLAN.setEnabled(enable);
//      jTeamCommPort.setEnabled(enable);
//      jTeamNumber.setEnabled(enable);
      naoByte1.setEnabled(enable);
      naoByte2.setEnabled(enable);
      naoByte3.setEnabled(enable);
      naoByte4.setEnabled(enable);
//      sshPassword.setEnabled(enable);
//      sshUser.setEnabled(enable);
      jSettingsPanel.setEnabled(enable);
      jSettingsPanel1.setEnabled(enable);
      jSettingsPanel2.setEnabled(enable);
//      subnetFieldLAN.setEnabled(enable);
//      subnetFieldWLAN.setEnabled(enable);
  }


  private boolean prepareDeploy()
  {
    return prepareDeploy(false);
  }  
  
  /**
   * Staging - prepares "deploy out dir" with the files to be copied to the robots
   * copies files, creates scheme.cfg, modifies other cfgs according to UI settings
   * @return
   */
  private boolean prepareDeploy(boolean init)
  {
    actionInfo("preparing deploy dir");

    File deployOutDir = new File(localDeployRootPath());

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

    checkNaoIps();
    
    for(Integer naoNo : iNaoBytes.keySet())
    {
      if((!init && naoNo > 0) || (init && naoNo == 0))
      {
        int iNaoByte = iNaoBytes.get(naoNo);
        String sNaoByte = String.valueOf(iNaoByte);
        if(iNaoByte < 256 && iNaoByte > -1)
        {
          String sNaoNo = String.valueOf(naoNo);
          String currentDeployPath = localDeployOutPath(sNaoNo);
          File currentDeployDir = new File(currentDeployPath);

          if(currentDeployDir.isDirectory())
          {
            deleteDir(currentDeployDir);
          }

          currentDeployDir.mkdirs();
          File localLibPath = new File(localDeployOutPath(sNaoNo) + libnaoPath());
          localLibPath.mkdirs();
          File localBinPath = new File(localDeployOutPath(sNaoNo) + binPath());
          localBinPath.mkdirs();

          if(copyLib)
          {
            File localLib = new File(
            localDeployOutPath(sNaoNo) + libnaoPath() + "/libnaoth.so");
            if(localLib.exists())
            {
              localLib.delete();
            }
            try
            {
              FileOutputStream fos = new FileOutputStream(localDeployOutPath(sNaoNo) + libnaoPath() + "/comment.cfg");
              DataOutputStream out = new DataOutputStream(fos);
              out.writeBytes(jCommentTextArea.getText());
              fos.close();
            }
            catch(IOException ioe)
            {
              actionInfo("I/O Error in prepareDeploy- " + ioe.toString());
            }          

            copyFiles(new File(localLibnaothPath() + "/libnaoth.so"), localLib);
          }

          if(copyExe)
          {
            File localExe = new File(
            localDeployOutPath(sNaoNo) + binPath() + "/naoth");
            if(localExe.exists())
            {
              localExe.delete();
            }

            copyFiles(new File(localLibnaothPath() + "/naoth"), localExe);
          }

          if(copyConfig)
          {
            String myConfigPath = localDeployOutPath(sNaoNo) + configPath();
            File myConfigDir = new File(myConfigPath);
            myConfigDir.mkdirs();

            String myConfigPathIn = localDeployInPath(sNaoNo, sNaoByte) + "/Config";
            File myConfigDirIn = new File(myConfigPathIn);
            myConfigDirIn.mkdirs();

            copyFiles(new File(localConfigPath()), myConfigDir);
            writePlayerCfg(new File(myConfigPath + "/private/player.cfg"), sNaoNo);
            writeTeamcommCfg(new File(myConfigPath + "/private/teamcomm.cfg"));
            writeScheme(new File(myConfigPath + "/scheme.cfg"));
            writeNaoInfo(new File(myConfigPath + "/nao.info"));
          }
        }
      }
    }

    actionInfo("finished preparing deploy dir");

    return true;
  }//end prepareDeploy

/**
   * Staging - prepares "deploy out dir for setting up nao system" with the files to be copied to the robots
   * copies files, creates scheme.cfg, modifies other cfgs according to UI settings
   * @return
   */
  private boolean prepareSetupDeploy(String sNaoByte)
  {
    actionInfo("preparing deploy dir");

    File deployOutDir = new File(localDeployRootPath());

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

    checkNaoIps();
    int naoNo = 0;
    int iNaoByte = Integer.parseInt(sNaoByte);
    
    if(iNaoByte < 256 && iNaoByte > -1)
    {
      String sNaoNo = String.valueOf(naoNo);

      String mySetupScriptPath = localDeployOutPath(sNaoNo) + setupScriptPath();
      File mySetupScriptDir = new File(mySetupScriptPath);
      if(mySetupScriptDir.isDirectory())
      {
        deleteDir(mySetupScriptDir);
      }
      mySetupScriptDir.mkdirs();

      File mySetupScriptCheckRC = new File(mySetupScriptPath +"/checkRC.sh");        
      copyFiles(new File(localSetupScriptPath() + "/checkRC.sh"), mySetupScriptCheckRC);
      File mySetupScriptInitEnv = new File(mySetupScriptPath +"/init_env.sh");        
      copyFiles(new File(localSetupScriptPath() + "/init_env.sh"), mySetupScriptInitEnv);
      File mySetupScriptInitNet = new File(mySetupScriptPath +"/init_net.sh");        
      copyFiles(new File(localSetupScriptPath() + "/init_net.sh"), mySetupScriptInitNet);
      File myAutoloadIni = new File(mySetupScriptPath +"/autoload.ini");        
      copyFiles(new File(localSetupScriptPath() + "/autoload.ini"), myAutoloadIni);
      File myNaothScript = new File(mySetupScriptPath +"/naoth");        
      copyFiles(new File(localSetupScriptPath() + "/naoth"), myNaothScript);
            
      File libRT = new File(stagingLibDir +"/usr/lib/librt.so");
      if(libRT.exists())
      {
        libRT.delete();
      }

      String mySetupPath = mySetupScriptPath + "/etc";
      File mySetupDir = new File(mySetupPath);
      mySetupDir.mkdirs();

      copyFiles(new File(localSetupScriptPath() + "/etc"), mySetupDir);
      
    }
    setConfdNet(sNaoByte);
    setHostname(sNaoByte);
        
    actionInfo("finished preparing deploy dir");

    return true;
  }//end prepareSetupDeploy

  private void setConfdNet(String sNaoByte)
  {
      try
      {
        BufferedReader br = new BufferedReader(new FileReader(localSetupScriptPath() + "/etc/conf.d/net"));
        
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
                        
        BufferedWriter bw = new BufferedWriter(new FileWriter(localDeployOutPath("0") + setupScriptPath() + "/etc/conf.d/net"));
        bw.write(fileContent);
        bw.close();
      }
      catch(Exception e)
      {
        actionInfo(e.toString());
      }
  }
  
  private void setHostname(String sNaoByte)
  {
      try
      {
        BufferedReader br = new BufferedReader(new FileReader(localSetupScriptPath() + "/etc/hostname"));
        
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
                        
        BufferedWriter bw = new BufferedWriter(new FileWriter(localDeployOutPath("0") + setupScriptPath() + "/etc/hostname"));
        bw.write(fileContent);
        bw.close();

        
        br = new BufferedReader(new FileReader(localSetupScriptPath() + "/etc/conf.d/hostname"));
        
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
        bw = new BufferedWriter(new FileWriter(localDeployOutPath("0") + setupScriptPath() + "/etc/conf.d/hostname"));
        bw.write(fileContent);
        bw.close();
      }
      catch(Exception e)
      {
        actionInfo(e.toString());
      }
  }
  
  
  private String readFile(String fileName)
  {
    StringBuilder content = new StringBuilder();

    try 
    {
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
      actionInfo("I/O Error in readFile- " + fileName + "\n" + ex.toString());
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
      File file = new File(localDeployRootPath() + "/in/" + backups.get(jBackupBox.getSelectedItem()) + "/comment.cfg");
      if(file.exists() && file.isFile())
      {
        comment = readFile(file.getPath());
      }
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

        File minConfigDir = new File(localDeployRootPath() + "/in/" + entry + "/MinimalConfig/");
        if(minConfigDir.exists() && minConfigDir.isDirectory())
        {
          entryAvaliable = true;
          entryTag += "M";
        }

        if(copyConfig)
        {
            File configDir = new File(localDeployRootPath() + "/in/" + entry + "/Config/");
            if(configDir.exists() && configDir.isDirectory())
            {
              entryAvaliable = true;
              entryTag += "C";
            }
        }

        if(copyLib)
        {
            File libFile = new File(localDeployRootPath() + "/in/" + entry + "/libnaoth.so");
            if(libFile.exists() && libFile.isFile())
            {
              entryAvaliable = true;
              entryTag += "L";
            }
        }

        if(copyExe)
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
    this.initNetworkConfig();
    resetBackupList();
    
    File f = new File(localConfigPath() + "/general/player.cfg");    
    if(!f.isFile())
    {
      writePlayerCfg(f, "1");
    }
    else
    {
      readPlayerCfg(f);
    }
    
    f = new File(localConfigPath() + "/general/teamcomm.cfg");
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
        if(files[i].getName().equalsIgnoreCase("nao"))
        {
          jSchemeBox.setSelectedIndex(jSchemeBox.getItemCount() - 1);
        }
      }
    }
    return true;
  }//end setSchemes

  protected void actionInfo(String debugtext)
  {
    jCopyStatus.setText(debugtext);
    if(logActionInfo)
    {
      log(debugtext);
    }
  }//end actionInfo

  private void log(final String logtext)
  {
    jLogWindow.append(logtext + "\n");
    jLogWindow.invalidate();
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
  private void writeNaoInfo(File infoFile)
  {    
    try
    {
      BufferedWriter writer = new BufferedWriter(new FileWriter(infoFile));
      writer.write("");
      writer.close();
    }
    catch(IOException ioe)
    {
      actionInfo("I/O Error in writeNaoInfo- " + ioe.toString());
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
        String value = null;
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
        String value = null;
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
      int part = 0;
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
    robotConfigPreparator()
    {
      synchronized (naoModel)
      {
        copyConfig = true;
        copyLib = true;
        copyExe = true;
        copyLogs = false;
        restartNaoth = false;
        noBackup = true;

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
        backupTimestamp = String.valueOf(System.currentTimeMillis());

        addresses = new ArrayList<String>();      
        NaoSshWrapper w = null;
        int i = lstNaos.getSelectedIndex();
        lstNaos.clearSelection();
        if (i >= 0)
        {
          w = (NaoSshWrapper) naoModel.get(i);

          if(w.getAddresses().length > 0)
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
          iNaoBytes.put(0, iNaoByte);          
        }
      }
      return sNaoByte;
    }
    
    public String askForPlayerNumber()
    {
      String sPlayerNo = null;
      int iPlayerNo = 0;
      while(iPlayerNo >= iNaoBytes.size() || iPlayerNo < 1)
      {
        sPlayerNo = JOptionPane.showInputDialog("Enter Nao Number (1 - " + (iNaoBytes.size() - 1) + ")", "");
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
    
  private void initializeRobot()
  {
    if(stagingLibDir == null)
    {
      haveError("0", "no valid CTC Staging Directory selected");
      allIsDone("0");
      return;
    }
    
    robotConfigPreparator preparator = new robotConfigPreparator();
    copySysLibs = true;
    String address = preparator.getDefaultAddress();
    ArrayList<String> addresses = preparator.getAdressList();
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
    
    if(address == null || sNaoByte == null || setupPlayerNo == null || !prepareDeploy(true) || !prepareSetupDeploy(sNaoByte))
    {
      haveError("0","robot initialization aborted");
      allIsDone("0");
      return;
    }

    boolean testOk = false;
    sshSetupCopier cLan = new sshSetupCopier(this, address, sNaoByte, "full");
    testOk = cLan.testConnection();

    if(!testOk && addresses.size() > 0 && !addresses.contains(address))
    {
      int idx = 0;
      while(!testOk && idx < addresses.size())
      {    
        address = addresses.get(idx);
        cLan = new sshSetupCopier(this, address, sNaoByte, "full");
        testOk = cLan.testConnection();
        idx++;
      }
    }
    
    if(testOk)
    {
      cLan.execute();
      synchronized(cLan)
      {
        while(!cLan.isDone())
        {
          this.validateTree();
          jLogWindow.updateUI();
        }
      }
      if(hadErrors.get(0))
      {
        allIsDone("0");
        return;
      }
      sshScriptRunner sLan = new sshScriptRunner(this, address, "0", sNaoByte, new String[] {"initializeRobot"}, cbRebootSystem.isSelected());
      if(sLan.testConnection())
      {
        sLan.execute();
        synchronized(sLan)
        {
          while(!sLan.isDone())
          {
            this.validateTree();
            jLogWindow.updateUI();
          }
        }
      }
      else
      {
        haveError("0", "Nao (" + address + ") not reachable, no script did run");
      }
    }
    else
    {
      haveError("0", "Nao (" + address + ") not reachable, no setting up possible");
    }
    allIsDone("0");
  }  
  
  private void setRobotNetwork()
  {
    robotConfigPreparator preparator = new robotConfigPreparator();
    copySysLibs = false;
    String address = preparator.getDefaultAddress();
    ArrayList<String> addresses = preparator.getAdressList();
      
    String sNaoByte = null;
    if(address != null )
    {
      sNaoByte = preparator.askForNaoNumber();
    }
    if(address == null || sNaoByte == null || !prepareSetupDeploy(sNaoByte))
    {
      haveError("0","robot network configuration aborted");
      allIsDone("0");
      return;
    }

    boolean testOk = false;
    sshSetupCopier cLan = new sshSetupCopier(this, address, sNaoByte, "network");    
    testOk = cLan.testConnection();

    if(!testOk && addresses.size() > 0 && !addresses.contains(address))
    {
      int idx = 0;
      while(!testOk && idx < addresses.size())
      {    
        address = addresses.get(idx);
        cLan = new sshSetupCopier(this, address, sNaoByte, "network");
        testOk = cLan.testConnection();
        idx++;
      }
    }

    if(testOk)
    {
      cLan.execute();
      synchronized(cLan)
      {
        while(!cLan.isDone())
        {
          this.validateTree();
          jLogWindow.updateUI();
        }
      }
      if(hadCopyErrors.get(0))
      {
        allIsDone("0");
        return;
      }
      sshScriptRunner sLan = new sshScriptRunner(this, address, "0", sNaoByte, new String[] {"setRobotNetworkConfig"}, cbRebootSystem.isSelected());
      if(sLan.testConnection())
      {
        sLan.execute();
        synchronized(sLan)
        {
          while(!sLan.isDone())
          {
            this.validateTree();
            jLogWindow.updateUI();
          }
        }
      }
      else
      {
        haveError("0", "Nao (" + address + ") not reachable, no script did run");
      }
    }
    else
    {
      haveError("0", "Nao (" + address + ") not reachable, no setting up possible");
    }
    allIsDone("0");
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
  private void initComponents() {

    jDialog1 = new javax.swing.JDialog();
    jLabel12 = new javax.swing.JLabel();
    jScrollPane1 = new javax.swing.JScrollPane();
    jLogWindow = new javax.swing.JTextArea();
    jCopyStatus = new javax.swing.JLabel();
    progressBar = new javax.swing.JProgressBar();
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
    cbCopyExe = new javax.swing.JCheckBox();
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
    addWindowListener(new java.awt.event.WindowAdapter() {
      public void windowClosing(java.awt.event.WindowEvent evt) {
        formWindowClosing(evt);
      }
    });

    jLabel12.setFont(new java.awt.Font("Lucida Grande", 0, 8));
    jLabel12.setText("v0.4");

    jLogWindow.setColumns(20);
    jLogWindow.setEditable(false);
    jLogWindow.setFont(new java.awt.Font("Lucida Grande", 0, 10));
    jLogWindow.setRows(5);
    jScrollPane1.setViewportView(jLogWindow);

    jCopyStatus.setFont(new java.awt.Font("Lucida Grande", 0, 10));
    jCopyStatus.setText("idle...");

    progressBar.setEnabled(false);

    jLabel6.setText("Nao 1:");

    naoByte1.setText("-1");
    naoByte1.setMaximumSize(new java.awt.Dimension(10, 31));
    naoByte1.setName("naoByte1"); // NOI18N
    naoByte1.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        naoByte1ActionPerformed(evt);
      }
    });

    jLabel7.setText("Nao 2:");

    naoByte2.setText("-1");
    naoByte2.setMaximumSize(new java.awt.Dimension(10, 31));
    naoByte2.setName("naoByte2"); // NOI18N
    naoByte2.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        naoByte2ActionPerformed(evt);
      }
    });

    naoByte3.setText("-1");
    naoByte3.setMaximumSize(new java.awt.Dimension(10, 31));
    naoByte3.setName("naoByte3"); // NOI18N
    naoByte3.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        naoByte3ActionPerformed(evt);
      }
    });

    jLabel8.setText("Nao 3:");

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
    jDirPathLabel.setText("SET DIRECTORY :)");
    jDirPathLabel.setToolTipText("NaoController project directory (e.g., \"D:\\NaoTH-2009\\Projects\\NaoController\")");

    jLabel15.setText("Version to Upload:");

    jBackupBox.setEnabled(false);
    jBackupBox.addItemListener(new java.awt.event.ItemListener() {
      public void itemStateChanged(java.awt.event.ItemEvent evt) {
        jBackupBoxItemStateChanged(evt);
      }
    });

    naoByte4.setText("-1");
    naoByte4.setMaximumSize(new java.awt.Dimension(10, 31));
    naoByte4.setName("naoByte4"); // NOI18N
    naoByte4.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
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
            .add(jDirPathLabel, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 654, Short.MAX_VALUE))
          .add(jActionsPanelLayout.createSequentialGroup()
            .addContainerGap()
            .add(jActionsPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.TRAILING)
              .add(org.jdesktop.layout.GroupLayout.LEADING, jSeparator1, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 678, Short.MAX_VALUE)
              .add(org.jdesktop.layout.GroupLayout.LEADING, jActionsPanelLayout.createSequentialGroup()
                .add(jLabel15)
                .add(18, 18, 18)
                .add(jBackupBox, 0, 540, Short.MAX_VALUE))
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
                .add(copyButton, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 249, Short.MAX_VALUE)))))
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

    cbCopyLib.setText("copyLibNaoTH");
    cbCopyLib.addItemListener(new java.awt.event.ItemListener() {
      public void itemStateChanged(java.awt.event.ItemEvent evt) {
        cbCopyLibItemStateChanged(evt);
      }
    });

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

    cbCopyConfig.setSelected(true);
    cbCopyConfig.setText("copyConfig");
    cbCopyConfig.addItemListener(new java.awt.event.ItemListener() {
      public void itemStateChanged(java.awt.event.ItemEvent evt) {
        cbCopyConfigItemStateChanged(evt);
      }
    });

    cbCopyExe.setSelected(true);
    cbCopyExe.setText("copyNaoTH(exe)");
    cbCopyExe.addItemListener(new java.awt.event.ItemListener() {
      public void itemStateChanged(java.awt.event.ItemEvent evt) {
        cbCopyExeItemStateChanged(evt);
      }
    });

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

    jLabel4.setText("Scheme:");

    jSchemeBox.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "n/a" }));
    jSchemeBox.addItemListener(new java.awt.event.ItemListener() {
      public void itemStateChanged(java.awt.event.ItemEvent evt) {
        jSchemeBoxItemStateChanged(evt);
      }
    });

    jButtonRefreshData.setText("Refresh");
    jButtonRefreshData.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
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
            .add(jButtonRefreshData, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 129, Short.MAX_VALUE))
          .add(jSchemeBox, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 352, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
        .add(73, 73, 73))
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
          .add(jLabel3, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 59, Short.MAX_VALUE))
        .addContainerGap())
    );

    cbNoBackup.setText("NO BACKUP");
    cbNoBackup.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        cbNoBackupActionPerformed(evt);
      }
    });

    jCommentTextArea.setColumns(15);
    jCommentTextArea.setRows(4);
    jScrollPane2.setViewportView(jCommentTextArea);

    cbCopyLogs.setText("copyLogs");
    cbCopyLogs.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        cbCopyLogsActionPerformed(evt);
      }
    });

    jLabel16.setText("Comment:");

    org.jdesktop.layout.GroupLayout jPanel1Layout = new org.jdesktop.layout.GroupLayout(jPanel1);
    jPanel1.setLayout(jPanel1Layout);
    jPanel1Layout.setHorizontalGroup(
      jPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
      .add(jPanel1Layout.createSequentialGroup()
        .addContainerGap()
        .add(jPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
          .add(jPanel1Layout.createSequentialGroup()
            .add(cbCopyConfig)
            .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
            .add(cbCopyLib)
            .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
            .add(cbCopyExe)
            .add(18, 18, 18)
            .add(cbRestartNaoth)
            .addPreferredGap(org.jdesktop.layout.LayoutStyle.UNRELATED)
            .add(cbCopyLogs)
            .addPreferredGap(org.jdesktop.layout.LayoutStyle.UNRELATED)
            .add(cbNoBackup))
          .add(jPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.TRAILING, false)
            .add(org.jdesktop.layout.GroupLayout.LEADING, jActionsPanel, 0, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
            .add(org.jdesktop.layout.GroupLayout.LEADING, jPanel1Layout.createSequentialGroup()
              .add(jSettingsPanel, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
              .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
              .add(jPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
                .add(jScrollPane2, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 186, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
                .add(jLabel16)))))
        .addContainerGap())
    );
    jPanel1Layout.setVerticalGroup(
      jPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
      .add(jPanel1Layout.createSequentialGroup()
        .addContainerGap()
        .add(jActionsPanel, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
        .add(15, 15, 15)
        .add(jPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
          .add(jPanel1Layout.createSequentialGroup()
            .add(jLabel16)
            .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
            .add(jScrollPane2, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 93, Short.MAX_VALUE))
          .add(jSettingsPanel, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
        .addPreferredGap(org.jdesktop.layout.LayoutStyle.UNRELATED)
        .add(jPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
          .add(cbCopyConfig)
          .add(cbCopyLib)
          .add(cbCopyExe)
          .add(cbRestartNaoth)
          .add(cbCopyLogs)
          .add(cbNoBackup, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 23, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
        .add(48, 48, 48))
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
    subnetFieldLAN.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        subnetFieldLANActionPerformed(evt);
      }
    });

    netmaskFieldLAN.setText("255.255.255.0");
    netmaskFieldLAN.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        netmaskFieldLANActionPerformed(evt);
      }
    });

    broadcastFieldLAN.setText("10.0.0.255");
    broadcastFieldLAN.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        broadcastFieldLANActionPerformed(evt);
      }
    });
    broadcastFieldLAN.addInputMethodListener(new java.awt.event.InputMethodListener() {
      public void inputMethodTextChanged(java.awt.event.InputMethodEvent evt) {
        broadcastFieldLANInputMethodTextChanged(evt);
      }
      public void caretPositionChanged(java.awt.event.InputMethodEvent evt) {
      }
    });
    broadcastFieldLAN.addKeyListener(new java.awt.event.KeyAdapter() {
      public void keyPressed(java.awt.event.KeyEvent evt) {
        broadcastFieldLANKeyPressed(evt);
      }
    });

    jLabel14.setText("WLAN:");

    jLabel19.setText("SubNet");

    jLabel21.setText("Netmask");

    jLabel23.setText("Broadcast");

    subnetFieldWLAN.setText("192.168.13");
    subnetFieldWLAN.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        subnetFieldWLANActionPerformed(evt);
      }
    });

    netmaskFieldWLAN.setText("255.255.255.0");
    netmaskFieldWLAN.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        netmaskFieldWLANActionPerformed(evt);
      }
    });

    broadcastFieldWLAN.setText("192.168.13.255");
    broadcastFieldWLAN.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        broadcastFieldWLANActionPerformed(evt);
      }
    });
    broadcastFieldWLAN.addInputMethodListener(new java.awt.event.InputMethodListener() {
      public void inputMethodTextChanged(java.awt.event.InputMethodEvent evt) {
        broadcastFieldWLANInputMethodTextChanged(evt);
      }
      public void caretPositionChanged(java.awt.event.InputMethodEvent evt) {
        broadcastFieldWLANCaretPositionChanged(evt);
      }
    });
    broadcastFieldWLAN.addPropertyChangeListener(new java.beans.PropertyChangeListener() {
      public void propertyChange(java.beans.PropertyChangeEvent evt) {
        broadcastFieldWLANPropertyChange(evt);
      }
    });
    broadcastFieldWLAN.addKeyListener(new java.awt.event.KeyAdapter() {
      public void keyTyped(java.awt.event.KeyEvent evt) {
        broadcastFieldWLANKeyTyped(evt);
      }
      public void keyPressed(java.awt.event.KeyEvent evt) {
        broadcastFieldWLANKeyPressed(evt);
      }
    });
    broadcastFieldWLAN.addVetoableChangeListener(new java.beans.VetoableChangeListener() {
      public void vetoableChange(java.beans.PropertyChangeEvent evt)throws java.beans.PropertyVetoException {
        broadcastFieldWLANVetoableChange(evt);
      }
    });

    org.jdesktop.layout.GroupLayout jSettingsPanel1Layout = new org.jdesktop.layout.GroupLayout(jSettingsPanel1);
    jSettingsPanel1.setLayout(jSettingsPanel1Layout);
    jSettingsPanel1Layout.setHorizontalGroup(
      jSettingsPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
      .add(org.jdesktop.layout.GroupLayout.TRAILING, jSettingsPanel1Layout.createSequentialGroup()
        .add(15, 15, 15)
        .add(jSettingsPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
          .add(jSettingsPanel1Layout.createSequentialGroup()
            .add(jLabel13)
            .add(217, 217, 217))
          .add(jSettingsPanel1Layout.createSequentialGroup()
            .add(jSettingsPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.TRAILING)
              .add(org.jdesktop.layout.GroupLayout.LEADING, jSettingsPanel1Layout.createSequentialGroup()
                .add(jSettingsPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
                  .add(jLabel20, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 66, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
                  .add(jLabel1))
                .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                .add(jSettingsPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
                  .add(subnetFieldLAN, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 154, Short.MAX_VALUE)
                  .add(netmaskFieldLAN, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 154, Short.MAX_VALUE)))
              .add(org.jdesktop.layout.GroupLayout.LEADING, jSettingsPanel1Layout.createSequentialGroup()
                .add(jLabel22)
                .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                .add(broadcastFieldLAN, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 155, Short.MAX_VALUE)))
            .add(31, 31, 31)))
        .add(jSettingsPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
          .add(jLabel14)
          .add(jSettingsPanel1Layout.createSequentialGroup()
            .add(jSettingsPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
              .add(jLabel21)
              .add(jLabel19)
              .add(jLabel23))
            .add(18, 18, 18)
            .add(jSettingsPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
              .add(broadcastFieldWLAN, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 157, Short.MAX_VALUE)
              .add(subnetFieldWLAN, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 157, Short.MAX_VALUE)
              .add(netmaskFieldWLAN, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 157, Short.MAX_VALUE))))
        .addContainerGap())
    );
    jSettingsPanel1Layout.setVerticalGroup(
      jSettingsPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
      .add(jSettingsPanel1Layout.createSequentialGroup()
        .add(jSettingsPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
          .add(jLabel13)
          .add(jLabel14))
        .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
        .add(jSettingsPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
          .add(subnetFieldLAN, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
          .add(jLabel1)
          .add(jLabel19)
          .add(subnetFieldWLAN, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
        .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
        .add(jSettingsPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
          .add(jLabel20)
          .add(netmaskFieldLAN, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
          .add(jLabel21)
          .add(netmaskFieldWLAN, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
        .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
        .add(jSettingsPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
          .add(jLabel22)
          .add(broadcastFieldLAN, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
          .add(jLabel23)
          .add(broadcastFieldWLAN, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
        .addContainerGap(org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
    );

    jSettingsPanel2.setBackground(new java.awt.Color(204, 204, 255));

    jLabel24.setText("TeamComm:");

    lblTeamCommWLAN.setEditable(false);
    lblTeamCommWLAN.setText("192.168.13.255");
    lblTeamCommWLAN.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        lblTeamCommWLANActionPerformed(evt);
      }
    });

    jLabel25.setText("/");

    lblTeamCommLAN.setEditable(false);
    lblTeamCommLAN.setText("10.0.0.255");
    lblTeamCommLAN.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        lblTeamCommLANActionPerformed(evt);
      }
    });

    jLabel26.setText(":");

    jTeamCommPort.setText("10700");

    jLabel27.setText("ssh:");

    sshUser.setText("nao");
    sshUser.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        sshUserActionPerformed(evt);
      }
    });

    jLabel28.setText(":");

    sshPassword.setText("nao");

    jLabel29.setText("ssh:");

    sshRootUser.setEditable(false);
    sshRootUser.setText("root");
    sshRootUser.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
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
            .add(jTeamCommPort, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 110, Short.MAX_VALUE))
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
              .add(sshPassword, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 360, Short.MAX_VALUE)
              .add(sshRootPassword, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 360, Short.MAX_VALUE))))
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
    jButtonSetRobotNetwork.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        jButtonSetRobotNetworkActionPerformed(evt);
      }
    });

    jButtonInitRobotSystem.setText("Initialize Robot System");
    jButtonInitRobotSystem.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        jButtonInitRobotSystemActionPerformed(evt);
      }
    });

    jButtonSaveNetworkConfig.setText("Save As Default Config");
    jButtonSaveNetworkConfig.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        jButtonSaveNetworkConfigActionPerformed(evt);
      }
    });

    lstNaos.setSelectionMode(javax.swing.ListSelectionModel.SINGLE_SELECTION);
    lstNaos.addMouseListener(new java.awt.event.MouseAdapter() {
      public void mouseClicked(java.awt.event.MouseEvent evt) {
        lstNaosMouseClicked(evt);
      }
    });
    jScrollPane3.setViewportView(lstNaos);

    cbRebootSystem.setText("reboot OS");

    org.jdesktop.layout.GroupLayout jPanel2Layout = new org.jdesktop.layout.GroupLayout(jPanel2);
    jPanel2.setLayout(jPanel2Layout);
    jPanel2Layout.setHorizontalGroup(
      jPanel2Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
      .add(jPanel2Layout.createSequentialGroup()
        .addContainerGap()
        .add(jPanel2Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.TRAILING)
          .add(jSettingsPanel2, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
          .add(jSettingsPanel1, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
        .add(jPanel2Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
          .add(jScrollPane3, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 163, Short.MAX_VALUE)
          .add(cbRebootSystem, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 163, Short.MAX_VALUE)
          .add(jButtonSetRobotNetwork, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 163, Short.MAX_VALUE)
          .add(org.jdesktop.layout.GroupLayout.TRAILING, jButtonInitRobotSystem, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 163, Short.MAX_VALUE)
          .add(jButtonSaveNetworkConfig, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        .add(19, 19, 19))
    );
    jPanel2Layout.setVerticalGroup(
      jPanel2Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
      .add(org.jdesktop.layout.GroupLayout.TRAILING, jPanel2Layout.createSequentialGroup()
        .addContainerGap()
        .add(jPanel2Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.TRAILING, false)
          .add(org.jdesktop.layout.GroupLayout.LEADING, jPanel2Layout.createSequentialGroup()
            .add(jSettingsPanel1, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
            .add(18, 18, 18)
            .add(jSettingsPanel2, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
          .add(jPanel2Layout.createSequentialGroup()
            .add(cbRebootSystem)
            .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
            .add(jScrollPane3)
            .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
            .add(jButtonSaveNetworkConfig, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 27, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
            .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
            .add(jButtonSetRobotNetwork, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 27, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
            .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
            .add(jButtonInitRobotSystem, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 27, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)))
        .addContainerGap())
    );

    jTabbedPane1.addTab("Network Configuration", jPanel2);

    org.jdesktop.layout.GroupLayout layout = new org.jdesktop.layout.GroupLayout(getContentPane());
    getContentPane().setLayout(layout);
    layout.setHorizontalGroup(
      layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
      .add(layout.createSequentialGroup()
        .addContainerGap()
        .add(layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
          .add(layout.createSequentialGroup()
            .add(jTabbedPane1, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 742, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
            .add(6, 6, 6)
            .add(layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
              .add(progressBar, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 202, Short.MAX_VALUE)
              .add(jScrollPane1)
              .add(jCopyStatus, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 202, Short.MAX_VALUE)))
          .add(jLabel12))
        .add(21, 21, 21))
    );
    layout.setVerticalGroup(
      layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
      .add(layout.createSequentialGroup()
        .add(jLabel12)
        .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
        .add(layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
          .add(layout.createSequentialGroup()
            .add(jScrollPane1, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 333, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
            .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
            .add(jCopyStatus, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 19, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
            .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
            .add(progressBar, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
          .add(jTabbedPane1, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 393, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
        .addContainerGap(20, Short.MAX_VALUE))
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
        setCommentText();
      }
    }//GEN-LAST:event_jBackupBoxItemStateChanged

    private void cbCopyConfigItemStateChanged(java.awt.event.ItemEvent evt) {//GEN-FIRST:event_cbCopyConfigItemStateChanged
      resetBackupList();
    }//GEN-LAST:event_cbCopyConfigItemStateChanged

    private void cbCopyLibItemStateChanged(java.awt.event.ItemEvent evt) {//GEN-FIRST:event_cbCopyLibItemStateChanged
      resetBackupList();
    }//GEN-LAST:event_cbCopyLibItemStateChanged

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

    private void cbCopyExeItemStateChanged(java.awt.event.ItemEvent evt) {//GEN-FIRST:event_cbCopyExeItemStateChanged
      // TODO add your handling code here:
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
      setRobotNetwork();
    }//GEN-LAST:event_jButtonSetRobotNetworkActionPerformed

    private void jButtonSaveNetworkConfigActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButtonSaveNetworkConfigActionPerformed
      try
      {
        writeNetworkConfig();
        writePlayerCfg(new File(localConfigPath() + "/general/player.cfg"), "1");
        writeTeamcommCfg(new File(localConfigPath() + "/general/teamcomm.cfg"));
        writeScheme(new File(localConfigPath() + "/scheme.cfg"));        
      }
      catch(IOException ex)
      {
        actionInfo("Could'nt write network config file\n"  + ex.getMessage());
      }
    }//GEN-LAST:event_jButtonSaveNetworkConfigActionPerformed

    private void lstNaosMouseClicked(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_lstNaosMouseClicked
      
      if (evt.getClickCount() == 2) {
        setRobotNetwork();
      }
    }//GEN-LAST:event_lstNaosMouseClicked

    private void formWindowClosing(java.awt.event.WindowEvent evt) {//GEN-FIRST:event_formWindowClosing
      for(JmDNS jmdns : jmdnsList)
      {
        try
        {
          jmdns.close();
        }
        catch(IOException ex)
        {
          
        }
      }
    }//GEN-LAST:event_formWindowClosing

    private void jButtonInitRobotSystemActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButtonInitRobotSystemActionPerformed
      
      stagingLibDir = null;
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
      chooser.setDialogTitle("Select CTC Staging Directory");
      chooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
      chooser.setAcceptAllFileFilterUsed(false);
      int ret = chooser.showOpenDialog(this);
      
      while(ret != JFileChooser.CANCEL_OPTION && stagingLibDir == null)
      {
        if(ret == JFileChooser.APPROVE_OPTION)
        {
          stagingLibDir = chooser.getSelectedFile().getPath();
        }

        if(stagingLibDir != null)
        {
          stagingLibDir += "/usr/lib";
          File gioDir = new File(stagingLibDir + "/gio");
          File glibDir = new File(stagingLibDir + "/glib-2.0");
          if(!gioDir.isDirectory() || !glibDir.isDirectory())
          {
            stagingLibDir = null;
            chooser.setDialogTitle("CTC Staging Directory seems to be wrong. Try again");
            ret = chooser.showOpenDialog(this);
          }
        }
      }            
      initializeRobot();
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

  // Variables declaration - do not modify//GEN-BEGIN:variables
  private javax.swing.JTextField broadcastFieldLAN;
  private javax.swing.JTextField broadcastFieldWLAN;
  private javax.swing.JCheckBox cbCopyConfig;
  private javax.swing.JCheckBox cbCopyExe;
  private javax.swing.JCheckBox cbCopyLib;
  private javax.swing.JCheckBox cbCopyLogs;
  private javax.swing.JCheckBox cbNoBackup;
  private javax.swing.JCheckBox cbRebootSystem;
  private javax.swing.JCheckBox cbRestartNaoth;
  private javax.swing.JButton copyButton;
  private javax.swing.JPanel jActionsPanel;
  private javax.swing.JComboBox jBackupBox;
  private javax.swing.JButton jButtonInitRobotSystem;
  private javax.swing.JButton jButtonRefreshData;
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
  private javax.swing.JLabel jLabel6;
  private javax.swing.JLabel jLabel7;
  private javax.swing.JLabel jLabel8;
  private javax.swing.JTextArea jLogWindow;
  private javax.swing.JPanel jPanel1;
  private javax.swing.JPanel jPanel2;
  private javax.swing.JComboBox jSchemeBox;
  private javax.swing.JScrollPane jScrollPane1;
  private javax.swing.JScrollPane jScrollPane2;
  private javax.swing.JScrollPane jScrollPane3;
  private javax.swing.JSeparator jSeparator1;
  private javax.swing.JPanel jSettingsPanel;
  private javax.swing.JPanel jSettingsPanel1;
  private javax.swing.JPanel jSettingsPanel2;
  private javax.swing.JTabbedPane jTabbedPane1;
  private javax.swing.JTextField jTeamCommPort;
  private javax.swing.JTextField jTeamNumber;
  private javax.swing.JTextField lblTeamCommLAN;
  private javax.swing.JTextField lblTeamCommWLAN;
  private javax.swing.JList lstNaos;
  private javax.swing.JTextField naoByte1;
  private javax.swing.JTextField naoByte2;
  private javax.swing.JTextField naoByte3;
  private javax.swing.JTextField naoByte4;
  private javax.swing.JTextField netmaskFieldLAN;
  private javax.swing.JTextField netmaskFieldWLAN;
  private javax.swing.JProgressBar progressBar;
  private javax.swing.JPasswordField sshPassword;
  private javax.swing.JPasswordField sshRootPassword;
  private javax.swing.JTextField sshRootUser;
  private javax.swing.JTextField sshUser;
  private javax.swing.JTextField subnetFieldLAN;
  private javax.swing.JTextField subnetFieldWLAN;
  // End of variables declaration//GEN-END:variables
}


