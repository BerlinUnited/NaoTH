/*
 * LogfileRecorder.java
 *
 * Created on 28.05.2009, 18:14:57
 */
package de.naoth.rc.dialogs;

import com.google.gson.Gson;
import com.jcraft.jsch.JSchException;
import com.jcraft.jsch.SftpException;
import com.jcraft.jsch.SftpProgressMonitor;
import de.naoth.rc.Helper;
import de.naoth.rc.RobotControl;
import de.naoth.rc.components.ExtendedFileChooser;
import de.naoth.rc.core.dialog.AbstractDialog;
import de.naoth.rc.core.dialog.DialogPlugin;
import de.naoth.rc.core.dialog.RCDialog;
import de.naoth.rc.core.manager.ObjectListener;
import de.naoth.rc.core.manager.SwingCommandExecutor;
import de.naoth.rc.dataformats.ModuleConfiguration;
import de.naoth.rc.manager.GenericManager;
import de.naoth.rc.manager.GenericManagerFactory;
import de.naoth.rc.manager.ModuleConfigurationManager;
import de.naoth.rc.scp.Scp;
import de.naoth.rc.core.server.Command;
import java.io.FileOutputStream;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;
import java.util.stream.Collectors;
import javax.swing.DefaultComboBoxModel;
import javax.swing.JFileChooser;
import javax.swing.JOptionPane;
import javax.swing.filechooser.FileNameExtensionFilter;
import javax.swing.SwingWorker;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;


/**
 *
 * @author thomas
 */
public class LogfileRecorder extends AbstractDialog implements ObjectListener<ModuleConfiguration>
{
  @RCDialog(category = RCDialog.Category.Log, name = "Recorder")
  @PluginImplementation
  public static class Plugin extends DialogPlugin<LogfileRecorder> {
    @InjectPlugin
    static public RobotControl parent;
    @InjectPlugin
    static public SwingCommandExecutor commandExecutor;
    @InjectPlugin
    public static GenericManagerFactory genericManagerFactory;
    @InjectPlugin
    public static ModuleConfigurationManager moduleConfigurationManager;
  }
  
  private LoggerItem selectedLog;
  
  Map<String, Collection<String>> selectionLists = new TreeMap<String, Collection<String>>();
  // todo: load from config
  List<String> defaultSelectionSchemes = Arrays.asList("none","Basic Perception", "Last Record", "-----------");
  List<String> ignoreModulesRepresentations = Arrays.asList();
  
  StatusApdateHandler statusUpdateHandler = new StatusApdateHandler();
  
  /** Creates new form LogfileRecorder */
  public LogfileRecorder()
  {
    initComponents();

    // TODO: make it config
    selectionLists.put("Basic Perception",     
        Arrays.asList( 
                "Image", 
                "ImageTop",
                "FrameInfo", 
                "CameraMatrix",
                "CameraMatrixTop",
                "SensorJointData", 
                "FSRData",
                "GyrometerData",
                "AccelerometerData",
                "InertialSensorData",
                "InertialModel")
    );
    
    defaultSelectionSchemes.forEach((item) -> { cbSelectionScheme.addItem(item); });
    
    DefaultComboBoxModel loggerListModel = new DefaultComboBoxModel();
    loggerListModel.addElement(new LoggerItem("CognitionLog", "Cognition:CognitionLog"));
    loggerListModel.addElement(new LoggerItem("MotionLog", "Motion:MotionLog"));
    cbLogName.setModel(loggerListModel);
    
    selectedLog = (LoggerItem) cbLogName.getSelectedItem();
    
    // add 'last record' selection entries
    String lastRecordString = Plugin.parent.getConfig().getProperty(this.getClass().getName()+".last_record", "[]");
    List lastRecord = (new Gson()).fromJson(lastRecordString, List.class);
    selectionLists.put("Last Record", lastRecord);
  }
  
  class LoggerItem
  {
      private final String name;
      private final String cmd;
      
      private GenericManager statusUpdateManager = null;
      
      public LoggerItem(String name, String cmd) {
          this.name = name;
          this.cmd = cmd;
      }
      
      @Override
      public String toString() {
          return name;
      }
      
      public Command getCommand() {
          return new Command(cmd);
      }
      
      public void setStatusListener(StatusApdateHandler listener) {
          
        if(statusUpdateManager == null) {
            Command cmdStatus = selectedLog.getCommand().addArg("status");
            statusUpdateManager = Plugin.genericManagerFactory.getManager(cmdStatus);
        }

        statusUpdateManager.addListener(listener);
      }
      
      public void removeStatusListener(StatusApdateHandler listener) {
        if (statusUpdateManager != null) {
            statusUpdateManager.removeListener(listener);
        }
      }
  }

  private String getLogType() {
      switch(cbLogName.getSelectedItem().toString()) {
          case "CognitionLog": return "Cognition";
          case "MotionLog": return "Motion";
      }
      return null;
  }

  class DefaultHandler implements ObjectListener<byte[]>
  {
    @Override
    public void newObjectReceived(final byte[] result)
    {
        
    }

    @Override
    public void errorOccured(String cause) {
        btRecord.setEnabled(false);
        btSave.setEnabled(false);
        btClose.setEnabled(false);
        txtTempFile.setEnabled(true);
    
        JOptionPane.showMessageDialog(null, "command failed: " + cause,
            "ERROR", JOptionPane.ERROR_MESSAGE);
    }
  }
  
  class UpdateLogListHandler extends DefaultHandler
  {
    @Override
    public void newObjectReceived(final byte[] result)
    {
        // remember selected stuff by copying it (no reference!!!)
        Collection<String> selectedOptions = new ArrayList<>(stringSelectionPanel.getSelection());
        stringSelectionPanel.clear();
        String[] strings = (new String(result)).split(" ");
        stringSelectionPanel.addOptions(strings);
        // restore previous selection
        stringSelectionPanel.select(selectedOptions);
        stringSelectionPanel.setEnabled(true);
        cbSelectionScheme.setEnabled(true);
        btnClearSelection.setEnabled(true);
    }
  }


  private void close()
  {
    // stop recording
    btRecord.setSelected(false);
    btRecord.setEnabled(false);
    btClose.setEnabled(false);
    //stringSelectionPanel.clear();
    stringSelectionPanel.setEnabled(false);
    cbSelectionScheme.setEnabled(false);
    btnClearSelection.setEnabled(false);
    
    btSave.setEnabled(true);
    btNew.setEnabled(true);
    txtTempFile.setEnabled(true);
    cbLogName.setEnabled(true);

    if (Plugin.parent.getMessageServer().isConnected()) {
        Command cmdOff = selectedLog.getCommand().addArg("off");
        Plugin.commandExecutor.executeCommand(new DefaultHandler(), cmdOff);

        // close file on robot
        Command cmdClose = selectedLog.getCommand().addArg("close");
        Plugin.commandExecutor.executeCommand(new DefaultHandler(), cmdClose);
    }
    
    selectedLog.removeStatusListener(statusUpdateHandler);
  }

  /** This method is called from within the constructor to
   * initialize the form.
   * WARNING: Do NOT modify this code. The content of this method is
   * always regenerated by the Form Editor.
   */
  @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        saveFileChooser = new ExtendedFileChooser();
        tbLog = new javax.swing.JToolBar();
        cbLogName = new javax.swing.JComboBox();
        btNew = new javax.swing.JButton();
        btRecord = new javax.swing.JToggleButton();
        btClose = new javax.swing.JButton();
        btSave = new javax.swing.JButton();
        jLabel1 = new javax.swing.JLabel();
        txtTempFile = new javax.swing.JTextField();
        jLabel2 = new javax.swing.JLabel();
        jProgressBar = new javax.swing.JProgressBar();
        stringSelectionPanel = new de.naoth.rc.components.StringSelectionPanel();
        jLabel3 = new javax.swing.JLabel();
        progressLabel = new javax.swing.JLabel();
        jPanel1 = new javax.swing.JPanel();
        cbSelectionScheme = new javax.swing.JComboBox();
        btnClearSelection = new javax.swing.JButton();

        saveFileChooser.setFileFilter(new FileNameExtensionFilter("Log files (*.log)", "log"));

        tbLog.setFloatable(false);
        tbLog.setRollover(true);

        cbLogName.setToolTipText("Choose the Logger Module");
        cbLogName.setBorder(javax.swing.BorderFactory.createEmptyBorder(1, 1, 1, 1));
        cbLogName.setMaximumSize(new java.awt.Dimension(124, 29));
        cbLogName.setMinimumSize(new java.awt.Dimension(124, 29));
        cbLogName.setPreferredSize(new java.awt.Dimension(124, 29));
        cbLogName.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                cbLogNameActionPerformed(evt);
            }
        });
        tbLog.add(cbLogName);

        btNew.setIcon(new javax.swing.ImageIcon(getClass().getResource("/toolbarButtonGraphics/general/New24.gif"))); // NOI18N
        btNew.setToolTipText("New");
        btNew.setFocusable(false);
        btNew.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btNew.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btNew.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btNewActionPerformed(evt);
            }
        });
        tbLog.add(btNew);

        btRecord.setIcon(new javax.swing.ImageIcon(getClass().getResource("/toolbarButtonGraphics/general/Stop24.gif"))); // NOI18N
        btRecord.setToolTipText("Record");
        btRecord.setEnabled(false);
        btRecord.setFocusable(false);
        btRecord.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btRecord.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btRecord.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btRecordActionPerformed(evt);
            }
        });
        tbLog.add(btRecord);

        btClose.setIcon(new javax.swing.ImageIcon(getClass().getResource("/toolbarButtonGraphics/media/Stop24.gif"))); // NOI18N
        btClose.setToolTipText("Close remote file");
        btClose.setEnabled(false);
        btClose.setFocusable(false);
        btClose.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btClose.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btClose.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btCloseActionPerformed(evt);
            }
        });
        tbLog.add(btClose);

        btSave.setIcon(new javax.swing.ImageIcon(getClass().getResource("/toolbarButtonGraphics/general/Save24.gif"))); // NOI18N
        btSave.setToolTipText("Download the logfile");
        btSave.setFocusable(false);
        btSave.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btSave.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btSave.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btSaveActionPerformed(evt);
            }
        });
        tbLog.add(btSave);

        jLabel1.setText("Temporary logfile path:");

        txtTempFile.setText("/tmp/cognition.log");
        txtTempFile.setToolTipText("Path to store the logfile on the robot.");

        jLabel2.setText("Record (use CTRL-Click to select more than one):");

        jProgressBar.setForeground(new java.awt.Color(255, 0, 51));

        stringSelectionPanel.setToolTipText("Move Items to the right to select them.");
        stringSelectionPanel.setEnabled(false);

        jLabel3.setText("Selection Scheme:");

        progressLabel.setHorizontalAlignment(javax.swing.SwingConstants.TRAILING);

        jPanel1.setLayout(new java.awt.BorderLayout());

        cbSelectionScheme.setEnabled(false);
        cbSelectionScheme.setPrototypeDisplayValue("BallDetectorIntegralImageTop");
        cbSelectionScheme.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                cbSelectionSchemeActionPerformed(evt);
            }
        });
        jPanel1.add(cbSelectionScheme, java.awt.BorderLayout.CENTER);

        btnClearSelection.setIcon(new javax.swing.ImageIcon(getClass().getResource("/data/bibliothek/gui/dock/core/small_delete.png"))); // NOI18N
        btnClearSelection.setToolTipText("Clear selection.");
        btnClearSelection.setEnabled(false);
        btnClearSelection.setIconTextGap(0);
        btnClearSelection.setMargin(new java.awt.Insets(2, 2, 2, 2));
        btnClearSelection.setMaximumSize(new java.awt.Dimension(20, 10));
        btnClearSelection.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnClearSelectionActionPerformed(evt);
            }
        });
        jPanel1.add(btnClearSelection, java.awt.BorderLayout.EAST);

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(tbLog, javax.swing.GroupLayout.DEFAULT_SIZE, 527, Short.MAX_VALUE)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(layout.createSequentialGroup()
                        .addComponent(jLabel1)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(progressLabel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
                    .addComponent(txtTempFile, javax.swing.GroupLayout.DEFAULT_SIZE, 507, Short.MAX_VALUE)
                    .addGroup(layout.createSequentialGroup()
                        .addComponent(jLabel2)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 128, Short.MAX_VALUE)
                        .addComponent(jLabel3)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(jPanel1, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                    .addComponent(jProgressBar, javax.swing.GroupLayout.DEFAULT_SIZE, 507, Short.MAX_VALUE)
                    .addComponent(stringSelectionPanel, javax.swing.GroupLayout.DEFAULT_SIZE, 575, Short.MAX_VALUE))
                .addContainerGap())
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addComponent(tbLog, javax.swing.GroupLayout.PREFERRED_SIZE, 32, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(18, 18, 18)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                    .addComponent(jLabel1, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addComponent(progressLabel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(txtTempFile, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                    .addComponent(jPanel1, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addComponent(jLabel3, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addComponent(jLabel2, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(stringSelectionPanel, javax.swing.GroupLayout.DEFAULT_SIZE, 224, Short.MAX_VALUE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jProgressBar, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap())
        );
    }// </editor-fold>//GEN-END:initComponents

    private void btNewActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_btNewActionPerformed
    {//GEN-HEADEREND:event_btNewActionPerformed

      if(Plugin.parent.checkConnected())
      {
        Command openCMD = selectedLog.getCommand().addArg("open", txtTempFile.getText());
        Plugin.commandExecutor.executeCommand(new UpdateLogListHandler(), openCMD);
        
        btNew.setEnabled(false);
        btRecord.setEnabled(true);
        btSave.setEnabled(false);
        btClose.setEnabled(true);
        txtTempFile.setEnabled(false);
        cbLogName.setEnabled(false);
        stringSelectionPanel.setEnabled(true);
        cbSelectionScheme.setEnabled(true);
        btnClearSelection.setEnabled(true);
        Plugin.moduleConfigurationManager.setModuleOwner(getLogType());
        Plugin.moduleConfigurationManager.addListener(this);
      }
    }//GEN-LAST:event_btNewActionPerformed

    private void btRecordActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_btRecordActionPerformed
    {//GEN-HEADEREND:event_btRecordActionPerformed

      if(Plugin.parent.checkConnected())
      {
        
        if(btRecord.isSelected() && stringSelectionPanel.getSelection().isEmpty())
        {
          btRecord.setSelected(false);
          JOptionPane.showMessageDialog(null, "Nothing selected for logging",
            "WARNING", JOptionPane.WARNING_MESSAGE);
        }
        else
        {
          // TODO: can we do it more efficient?
          // deactive all remaining items
          Collection<String> remaining = stringSelectionPanel.getOptions();
          for(String item: remaining)
          {
              Command cmdActivate = selectedLog.getCommand().addArg("deactive", item);
              Plugin.commandExecutor.executeCommand(new DefaultHandler(), cmdActivate);
          }

          // active all selected items
          Collection<String> selected = stringSelectionPanel.getSelection();
          for(String item: selected) {
              Command cmdActivate = selectedLog.getCommand().addArg("activate", item);
              Plugin.commandExecutor.executeCommand(new DefaultHandler(), cmdActivate);
          }
          
          // remember selected stuff
          selectionLists.put("Last Record", new ArrayList<>(stringSelectionPanel.getSelection()));
        }
        
        // activate permantent logging
        Command cmdOnOff = selectedLog.getCommand().addArg((btRecord.isSelected() ? "on" : "off"));
        //server.executeSingleCommand(this, cmdOnOff);
        Plugin.commandExecutor.executeCommand(new DefaultHandler(), cmdOnOff);
        
        // disable the controls once the recording started
        if(btRecord.isSelected())
        {
            stringSelectionPanel.setEnabled(!btRecord.isSelected());
            cbSelectionScheme.setEnabled(!btRecord.isSelected());
            btnClearSelection.setEnabled(!btRecord.isSelected());
            btNew.setEnabled(!btRecord.isSelected());
            
            selectedLog.setStatusListener(statusUpdateHandler);
        }
        else
        {
            selectedLog.removeStatusListener(statusUpdateHandler);
        }
      }//end if connected

    }//GEN-LAST:event_btRecordActionPerformed

    private void btSaveActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_btSaveActionPerformed
    {//GEN-HEADEREND:event_btSaveActionPerformed

      if(Plugin.parent.checkConnected())
      {
        // get the name of the file to download
        String fileName = new File(this.txtTempFile.getText()).getName();
        saveFileChooser.setSelectedFile(new File(fileName));
          
        // get file from robot
        if(saveFileChooser.showSaveDialog(this) == JFileChooser.APPROVE_OPTION)
        {
          File file = saveFileChooser.getSelectedFile();
          
          SwingWorker copy = new Copier(txtTempFile.getText(),file,jProgressBar);
          copy.execute();
        }
        txtTempFile.setEnabled(true);
      }
    }//GEN-LAST:event_btSaveActionPerformed

    private void btCloseActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_btCloseActionPerformed
    {//GEN-HEADEREND:event_btCloseActionPerformed

      if(Plugin.parent.checkConnected())
      {
        close();

        JOptionPane.showMessageDialog(null, "The file is saved on the robot as "
          + txtTempFile.getText() + " and can be copied by hand from there.");
        txtTempFile.setEnabled(true);
      }

    }//GEN-LAST:event_btCloseActionPerformed

    private void cbLogNameActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_cbLogNameActionPerformed
    {//GEN-HEADEREND:event_cbLogNameActionPerformed
      selectedLog = (LoggerItem) cbLogName.getSelectedItem();
      
      // auto change the filename
      String autofilename = selectedLog.toString().toLowerCase();
      autofilename = autofilename.substring(0, autofilename.length()-3) + ".log";
      String filename = txtTempFile.getText();
      int i = filename.lastIndexOf("/"); //File.separator
      if ( i >= 0 && i < filename.length()-1)
      {
        filename = filename.substring(0, i+1) + autofilename;
      }
      else
      {
        filename = autofilename;
      }
      txtTempFile.setText(filename);
    }//GEN-LAST:event_cbLogNameActionPerformed

    private void cbSelectionSchemeActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_cbSelectionSchemeActionPerformed
        stringSelectionPanel.clearSelection();
        if(cbSelectionScheme.getSelectedItem() != null) {
            Collection<String> selection = selectionLists.get((String)cbSelectionScheme.getSelectedItem());
            stringSelectionPanel.select(selection);
        }
    }//GEN-LAST:event_cbSelectionSchemeActionPerformed

    private void btnClearSelectionActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnClearSelectionActionPerformed
        cbSelectionScheme.setSelectedIndex(0);
    }//GEN-LAST:event_btnClearSelectionActionPerformed

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JButton btClose;
    private javax.swing.JButton btNew;
    private javax.swing.JToggleButton btRecord;
    private javax.swing.JButton btSave;
    private javax.swing.JButton btnClearSelection;
    private javax.swing.JComboBox cbLogName;
    private javax.swing.JComboBox cbSelectionScheme;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JLabel jLabel2;
    private javax.swing.JLabel jLabel3;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JProgressBar jProgressBar;
    private javax.swing.JLabel progressLabel;
    private javax.swing.JFileChooser saveFileChooser;
    private de.naoth.rc.components.StringSelectionPanel stringSelectionPanel;
    private javax.swing.JToolBar tbLog;
    private javax.swing.JTextField txtTempFile;
    // End of variables declaration//GEN-END:variables

    
    
  class Copier extends SwingWorker<Boolean, File> {
      String name;
      File store;
      Scp scp;
      javax.swing.JProgressBar progress;

      public Copier(String name, File store, javax.swing.JProgressBar progress) {
          this.name = name;
          this.store = store;
          progress.setValue(0);
          this.progress = progress;
      }

      @Override
      protected Boolean doInBackground() {
        try {
          FileOutputStream out = new FileOutputStream(this.store);
          SftpProgressMonitor monitor = new MyProgressMonitor();

          scp = new Scp(Plugin.parent.getMessageServer().getAddress().getAddress().getHostAddress());
          scp.c.get(name, out, monitor);

          out.close();
        }
        catch(JSchException | SftpException | IOException ex) {
          Helper.handleException("Could not get logfile from the robot", ex);
          return false;
        }
        return true; 
      }
      
      @Override
      public void done() {
         scp.shutdown();
         jProgressBar.setValue(0);
         JOptionPane.showMessageDialog(null, "File was transferred.");
      }

      class MyProgressMonitor implements SftpProgressMonitor
      {
        long count=0;
        long max=0;
        private long percent=-1;

        @Override
        public void init(int op, String src, String dest, long max)
        {
          this.max = max;
          jProgressBar.setIndeterminate(false);
          jProgressBar.setValue(0);
          jProgressBar.setMinimum(0);
          jProgressBar.setMaximum(100);
        }

        @Override
        public boolean count(long count)
        {
          this.count+=count;
          
          percent=this.count*100/max;
          jProgressBar.setValue((int)percent);
          return true;
        }

        @Override
        public void end(){
         
        }
      }//end class MyProgressMonitor
  }//end class Copier
  
  class StatusApdateHandler implements ObjectListener<byte[]> {

        @Override
        public void newObjectReceived(byte[] object) {
            double bytes = Double.parseDouble(new String(object));
            //System.out.println(String.format("%.2f MB", bytes / 1000.0/1000.0));
            progressLabel.setText(String.format("%.2f MB", bytes / 1000.0/1000.0));
        }

        @Override
        public void errorOccured(String cause) {
            JOptionPane.showMessageDialog(null, "command failed: " + cause,
                "ERROR", JOptionPane.ERROR_MESSAGE);
        }
    }

  @Override
  public void dispose()
  {
    // stop recording if necessary
    close();
    // remember 'last record'
    if(selectionLists.get("Last Record") != null) {
        Plugin.parent.getConfig().setProperty(this.getClass().getName()+".last_record", (new Gson()).toJson(selectionLists.get("Last Record")));
    }
  }

    @Override
    public void newObjectReceived(ModuleConfiguration moduleConfiguration) {
        // retrieve modules/representations only once
        Plugin.moduleConfigurationManager.removeListener(this);
        // mapping from modules to required representations
        HashMap<String, Set<String>> modules = new HashMap<>();
        // mapping from representations to providing modules
        HashMap<String, Set<String>> representations = new HashMap<>();
        // iterate through modules
        for (ModuleConfiguration.Node node : moduleConfiguration.getNodeList()) {
            // only modules
            if (node.getType() == ModuleConfiguration.NodeType.Module) {
                // generate the mapping from modules to required representations
                if(!node.require.isEmpty()) {
                    modules.put(node.getName(), new HashSet<>(node.require.stream().map((t) -> {
                        return t.getName();
                    }).collect(Collectors.toList())));
                }
                // generate the mapping from representations to providing modules
                if(!node.provide.isEmpty()) {
                    node.provide.stream().forEach((n) -> {
                        // create set, if not exists
                        if(!representations.containsKey(n.getName())) {
                            representations.put(n.getName(), new HashSet<>());
                        }
                        representations.get(n.getName()).add(node.getName());
                    });
                }
            }
        }
        
        ArrayList<String> newModules = new ArrayList<>();
        HashMap<String, Set<String>> dependend_representations = new HashMap<>();
        
        // for all modules collect all dependend modules
        for (String m : modules.keySet()) {
            dependend_representations.put(m, new HashSet<>());
            // the module depends on itself
            dependend_representations.get(m).add(m);
            // as long as new/dependend modules are added, continue adding modules
            while (true) {
                HashSet<String> subs = new HashSet<>();
                // check for each depended module, if it got another dependency
                for (String submodule : dependend_representations.get(m)) {
                    // do we know the dependent module
                    if(modules.containsKey(submodule)) {
                        // add all available & dependable representations
                        modules.get(submodule).stream().filter((r) -> {
                            return representations.containsKey(r);
                        }).forEach((r) -> {
                            // only select modules, which representations can not directly recorded
                            if(!stringSelectionPanel.getOptions().contains(r))
                                subs.addAll(representations.get(r));
                        });
                    }
                }
                // add dependend modules and check if something new was added
                int size = dependend_representations.get(m).size();
                dependend_representations.get(m).addAll(subs);
                if(dependend_representations.get(m).size() == size) {
                    // nothing new added, 'finish'
                    break;
                }
            }
            
            List<String> module_representations = new ArrayList<>();
            // only add modules which provide and where representation is available
            for (String string : dependend_representations.get(m)) {
                if(modules.containsKey(string)) {
                    module_representations.addAll(modules.get(string).stream().filter((t) -> {
                        return stringSelectionPanel.getOptions().contains(t) && !ignoreModulesRepresentations.contains(t);
                    }).collect(Collectors.toList()));
                }
            }
            
            // add module to selection list
            if(!module_representations.isEmpty()) {
                selectionLists.put(m, module_representations);
                newModules.add(m);
            }
        }
        // temporarily save selection by copying it (no reference!!!)
        ArrayList<String> tmp = new ArrayList<>(stringSelectionPanel.getSelection());
        // remove "old" modules from scheme selection list and re-add the default ones
        cbSelectionScheme.removeAllItems();
        defaultSelectionSchemes.forEach((item) -> { cbSelectionScheme.addItem(item); });
        newModules.stream().sorted().forEach((m) -> { cbSelectionScheme.addItem(m); });
        // re-add copied selection; this is necessary, 'cause of removing/adding items to the JCombobox!
        stringSelectionPanel.select(tmp);
    }

    @Override
    public void errorOccured(String cause) {
        Plugin.moduleConfigurationManager.removeListener(this);
    }
}//end class
