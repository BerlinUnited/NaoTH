/*
 * LogfileRecorder.java
 *
 * Created on 28.05.2009, 18:14:57
 */
package de.naoth.rc.dialogs;

import com.jcraft.jsch.JSchException;
import com.jcraft.jsch.SftpException;
import com.jcraft.jsch.SftpProgressMonitor;
import de.naoth.rc.Helper;
import de.naoth.rc.RobotControl;
import de.naoth.rc.components.ExtendedFileChooser;
import de.naoth.rc.core.dialog.AbstractDialog;
import de.naoth.rc.core.manager.ObjectListener;
import de.naoth.rc.core.manager.SwingCommandExecutor;
import de.naoth.rc.scp.Scp;
import de.naoth.rc.server.Command;
import java.io.FileOutputStream;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;
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
@PluginImplementation
public class LogfileRecorder extends AbstractDialog
{

  @InjectPlugin
  public RobotControl parent;
  @InjectPlugin
  public static SwingCommandExecutor commandExecutor;
  
  //private MessageServer server;
  private LoggerItem selectedLog;
  
  Map<String, List<String>> selectionLists = new TreeMap<String, List<String>>();
        
 
  /** Creates new form LogfileRecorder */
  public LogfileRecorder()
  {
    initComponents();

    selectedLog = (LoggerItem) cbLogName.getSelectedItem();
    
    
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
    
    // todo: load from config
    DefaultComboBoxModel m = new DefaultComboBoxModel();
    m.addElement("none");
    m.addElement("Basic Perception");
    m.addElement("Last Record");
    cbSelectionScheme.setModel(m);
   
    
    DefaultComboBoxModel loggerListModel = new DefaultComboBoxModel();
    loggerListModel.addElement(new LoggerItem("CognitionLog", "Cognition:CognitionLog"));
    loggerListModel.addElement(new LoggerItem("MotionLog", "Motion:MotionLog"));
    cbLogName.setModel(loggerListModel);
  }
  
  class LoggerItem
  {
      private final String name;
      private final String cmd;
      
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
        // remember selected stuff
        ArrayList<String> selectedOptions = 
                new ArrayList<String>(stringSelectionPanel.getSelection());
        stringSelectionPanel.clear();
        String[] strings = (new String(result)).split(" ");
        stringSelectionPanel.addOptions(strings);
        // restore previous selection
        stringSelectionPanel.select(selectedOptions);
        stringSelectionPanel.setEnabled(true);
        cbSelectionScheme.setEnabled(true);
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
    
    btSave.setEnabled(true);
    btNew.setEnabled(true);
    txtTempFile.setEnabled(true);
    cbLogName.setEnabled(true);

    Command cmdOff = selectedLog.getCommand()
      .addArg("off");

    //server.executeSingleCommand(this, cmdOff);
    commandExecutor.executeCommand(new DefaultHandler(), cmdOff);
    
    // close file on robot
    Command cmdClose = selectedLog.getCommand()
      .addArg("close");
    //server.executeSingleCommand(this, cmdClose);
    commandExecutor.executeCommand(new DefaultHandler(), cmdClose);
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
        cbSelectionScheme = new javax.swing.JComboBox();
        jLabel3 = new javax.swing.JLabel();

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

        cbSelectionScheme.setEnabled(false);
        cbSelectionScheme.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                cbSelectionSchemeActionPerformed(evt);
            }
        });

        jLabel3.setText("Selection Scheme:");

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
                        .addGap(0, 399, Short.MAX_VALUE))
                    .addComponent(txtTempFile, javax.swing.GroupLayout.DEFAULT_SIZE, 507, Short.MAX_VALUE)
                    .addGroup(layout.createSequentialGroup()
                        .addComponent(jLabel2)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 45, Short.MAX_VALUE)
                        .addComponent(jLabel3)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(cbSelectionScheme, javax.swing.GroupLayout.PREFERRED_SIZE, 135, javax.swing.GroupLayout.PREFERRED_SIZE))
                    .addComponent(jProgressBar, javax.swing.GroupLayout.DEFAULT_SIZE, 507, Short.MAX_VALUE)
                    .addComponent(stringSelectionPanel, javax.swing.GroupLayout.DEFAULT_SIZE, 507, Short.MAX_VALUE))
                .addContainerGap())
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addComponent(tbLog, javax.swing.GroupLayout.PREFERRED_SIZE, 32, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(18, 18, 18)
                .addComponent(jLabel1)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(txtTempFile, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jLabel2)
                    .addComponent(cbSelectionScheme, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(jLabel3))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(stringSelectionPanel, javax.swing.GroupLayout.PREFERRED_SIZE, 183, Short.MAX_VALUE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jProgressBar, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap())
        );
    }// </editor-fold>//GEN-END:initComponents

    private void btNewActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_btNewActionPerformed
    {//GEN-HEADEREND:event_btNewActionPerformed

      if(parent.checkConnected())
      {
        Command openCMD = selectedLog.getCommand()
          .addArg("open", txtTempFile.getText());
        
        //server.executeSingleCommand(this, openCMD);
        commandExecutor.executeCommand(new UpdateLogListHandler(), openCMD);
        
        btNew.setEnabled(false);
        btRecord.setEnabled(true);
        btSave.setEnabled(false);
        btClose.setEnabled(true);
        txtTempFile.setEnabled(false);
        cbLogName.setEnabled(false);
        stringSelectionPanel.setEnabled(true);
        cbSelectionScheme.setEnabled(true);
      }
    }//GEN-LAST:event_btNewActionPerformed

    private void btRecordActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_btRecordActionPerformed
    {//GEN-HEADEREND:event_btRecordActionPerformed

      if(parent.checkConnected())
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
              //server.executeSingleCommand(this, cmdActivate);
              commandExecutor.executeCommand(new DefaultHandler(), cmdActivate);
          }

          // active all selected items
          Collection<String> selected = stringSelectionPanel.getSelection();
          for(String item: selected)
          {
              Command cmdActivate = selectedLog.getCommand().addArg("activate", item);
              //server.executeSingleCommand(this, cmdActivate);
              commandExecutor.executeCommand(new DefaultHandler(), cmdActivate);
          }
          
          // remember selected stuff
          ArrayList<String> selectedOptions = 
            new ArrayList<String>(stringSelectionPanel.getSelection());
          selectionLists.put("Last Record", selectedOptions);
        }
        
        // activate permantent logging
        Command cmdOnOff = selectedLog.getCommand()
          .addArg((btRecord.isSelected() ? "on" : "off"));
        //server.executeSingleCommand(this, cmdOnOff);
        commandExecutor.executeCommand(new DefaultHandler(), cmdOnOff);
        
        // disable the controls once the recording started
        if(btRecord.isSelected())
        {
            stringSelectionPanel.setEnabled(!btRecord.isSelected());
            cbSelectionScheme.setEnabled(!btRecord.isSelected());
            btNew.setEnabled(!btRecord.isSelected());
        }
      }//end if connected

    }//GEN-LAST:event_btRecordActionPerformed

    private void btSaveActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_btSaveActionPerformed
    {//GEN-HEADEREND:event_btSaveActionPerformed

      if(parent.checkConnected())
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

      if(parent.checkConnected())
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
        List<String> selection = selectionLists.get((String)cbSelectionScheme.getSelectedItem());
        stringSelectionPanel.select(selection);
    }//GEN-LAST:event_cbSelectionSchemeActionPerformed

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JButton btClose;
    private javax.swing.JButton btNew;
    private javax.swing.JToggleButton btRecord;
    private javax.swing.JButton btSave;
    private javax.swing.JComboBox cbLogName;
    private javax.swing.JComboBox cbSelectionScheme;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JLabel jLabel2;
    private javax.swing.JLabel jLabel3;
    private javax.swing.JProgressBar jProgressBar;
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

          scp = new Scp(parent.getMessageServer().getAddress().getAddress().getHostAddress());
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
  

  @Override
  public void dispose()
  {
    // stop recording
    close();
    //System.out.println("Dispose is not implemented for: " + this.getClass().getName());
  }//end dispose

}//end class
