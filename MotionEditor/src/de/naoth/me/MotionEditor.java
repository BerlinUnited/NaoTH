/*
 * MainForm.java
 * renamed: MotionEditor.java (17.01.2011)
 *
 * Created on 12. März 2008, 18:40
 */
package de.naoth.me;

import de.naoth.me.core.JointDefaultConfiguration;
import de.naoth.me.core.JointPrototypeConfiguration;
import de.naoth.me.core.MotionNet;
import de.naoth.me.core.MotionNetLoader;
import de.naoth.rc.server.ConnectionDialog;
import de.naoth.rc.server.ConnectionStatusEvent;
import de.naoth.rc.server.ConnectionStatusListener;
import de.naoth.rc.server.MessageServer;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.io.CharArrayWriter;
import java.io.File;
import java.io.IOException;
import java.io.OutputStream;
import java.io.PrintStream;
import java.io.PrintWriter;
import java.io.UnsupportedEncodingException;
import java.io.Writer;
import java.net.URLDecoder;
import java.util.ArrayList;
import java.util.List;
import java.util.Properties;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.JFileChooser;
import javax.swing.JOptionPane;
import javax.swing.UIManager;
import javax.swing.filechooser.FileFilter;
import org.freehep.util.export.ExportDialog;

/**
 *
 * @author  Heinrich Mellmann
 */
public class MotionEditor extends javax.swing.JFrame
{

  private JointPrototypeConfiguration jointDefaultConfiguration;

  // Anfang Attribute
  private MotionNetLoader motionNetLoader;
  private File selectedFile = null;
  private JFileChooser FileChooser = null;

  private MessageServer messageServer;
  private ConnectionDialog connectionDialog;

  private Properties config;

  /** Creates new form MainForm */
  public MotionEditor()
  {
    config = new Properties();
    
    String value = System.getenv("NAOTH_BZR");
    
    if(value != null)
    {
      setDirectory(value);
    }
    else
    {
      try
      {
        String ResourceName="de/naoth/me/MotionEditor.class";
        String programPath = URLDecoder.decode(this.getClass().getClassLoader().getResource(ResourceName).getPath(), "UTF-8");
        programPath = programPath.replace("file:", "");
        //path replacement if MotionEditor is being started from console directly
        programPath = programPath.replace("/MotionEditor/dist/NaoScp_2.jar!/de/naoth/me/MotionEditor.class", "");
        //path replacement if MotionEditor is started from IDE (Netbeans)
        programPath = programPath.replace("/MotionEditor/build/classes/de/naoth/me/MotionEditor.class", "");
        File ProgramDir = new File(programPath);
        if(ProgramDir.exists())
        {
          setDirectory(ProgramDir.getAbsolutePath());
        }
      }
      catch(UnsupportedEncodingException ueEx)
      {
        Logger.getAnonymousLogger().log(Level.SEVERE, "could not determine current work directory\n" + ueEx.getMessage());
      }
    }
    
    System.setProperty("java.library.path", config.getProperty("pwd") + "/lib/gluegen-rt.dll");

    // set L&F to platform independent since the GTK-theme has problems with
    // the pin and close buttons
    String laf = "javax.swing.plaf.metal.MetalLookAndFeel";
    try
    {
        UIManager.setLookAndFeel(laf);
    }
    catch(Exception ex)
    {
      Logger.getAnonymousLogger().log(Level.INFO, "setting system Look & Feel failed", ex);
    }
    
    initComponents();
    initKneadEmulator();
//    initKneadEmulatorJOGL();

    this.setTitle("MotionNet Editor");
    this.motionNetLoader = new MotionNetLoader();
    this.jointDefaultConfiguration = new JointDefaultConfiguration();

    this.messageServer = new MessageServer();
    this.messageServer.addConnectionStatusListener(new ConnectionStatusListener() 
    {
        @Override
        public void connected(ConnectionStatusEvent event) {
            showConnected(true);
        }

        @Override
        public void disconnected(ConnectionStatusEvent event) {
            showConnected(false);
            if(event.getMessage() != null) {
                JOptionPane.showMessageDialog(MotionEditor.this,
                    event.getMessage(), "Disconnect", JOptionPane.ERROR_MESSAGE);
            }
        }
    });
    this.connectionDialog = new ConnectionDialog(this, this.messageServer);


    this.motionPlayer1.setMessageServer(this.messageServer);
    showConnected(false);
    
    this.motionNetEditorPanel.addPropertyChangeListener(this.keyFramePropertyViewer1);
    this.motionNetEditorPanel.addPropertyChangeListener(this.motionPlayer1);
    this.keyFramePropertyViewer1.addPropertyChangeListener(this.motionPlayer1);
    this.motionPlayer1.motionNetEditorPanel = motionNetEditorPanel;

    MotionNet motionNet = new MotionNet(jointDefaultConfiguration);
    this.motionNetEditorPanel.setMotionNet(motionNet);
    this.motionPlayer1.setMotionNet(motionNet);
    
    FileChooser = new JFileChooser(new File(config.getProperty("motions")));
    FileChooser.setFileSelectionMode(JFileChooser.FILES_ONLY);

    FileChooser.addPropertyChangeListener(JFileChooser.SELECTED_FILE_CHANGED_PROPERTY, new PropertyChangeListener()
    {
      public void propertyChange(PropertyChangeEvent arg0)
      {
        String FileName = "" + FileChooser.getSelectedFile();

        SimpleFileFilter NewValue = (SimpleFileFilter) FileChooser.getFileFilter();
        if (NewValue == null || FileName.compareTo("null") == 0 )
        {
          return;
        }
        String ExtNew = NewValue.getFirstExtension();
        if( FileName.endsWith(ExtNew))
        {
          return;
        }
        if (FileName.endsWith(".mef"))
        {
          FileName = FileName.replace(".mef", ExtNew);
        }
        else if (FileName.endsWith(".motion"))
        {
          FileName = FileName.replace(".motion", ExtNew);
        }
        else
        {
          FileName += ExtNew;
        }
        FileChooser.setSelectedFile(new File (FileName));
      }
    });
    FileChooser.addChoosableFileFilter(new SimpleFileFilter("MotionNet Editor File", "*.mef"));
  
  
    PrintWriter pw = new PrintWriter(new Writer() {
      
      private CharArrayWriter buffer = new CharArrayWriter();
      
      @Override
      public void write(char[] chars, int i, int i1) throws IOException {
          buffer.write(chars, i, i1);
      }

      @Override
      public void flush() throws IOException {
          jTextArea1.setText(buffer.toString());
          buffer.flush();
      }

      @Override
      public void close() throws IOException {}
  }, true);
    
    
  
    this.motionPlayer1.setOutStream(pw);
    
  }//end MainForm

  private void setDirectory(String directory)
  {
    File dir = new File(directory + "/NaoTHSoccer");
    config.setProperty("naothDir", dir.getAbsolutePath());
    dir = new File(directory + "/MotionEditor");
    config.setProperty("pwd", dir.getAbsolutePath());    
    dir = new File(directory + "/NaoTHSoccer/Config/scheme");
    config.setProperty("motions", dir.getAbsolutePath());
  }  

  // Anfang Methoden
  private void initKneadEmulator()
  {
    int tabIndex = -1;
    try
    {
      kneadEmulator = new de.naoth.me.emulation.KneadEmulator();
      javax.swing.GroupLayout kneadEmulatorLayout = new javax.swing.GroupLayout(kneadEmulator);
      kneadEmulator.setLayout(kneadEmulatorLayout);
      kneadEmulatorLayout.setHorizontalGroup
      (
        kneadEmulatorLayout.createParallelGroup
        (
          javax.swing.GroupLayout.Alignment.LEADING
        ).addGap(0, 574, Short.MAX_VALUE)
      );
      kneadEmulatorLayout.setVerticalGroup
      (
        kneadEmulatorLayout.createParallelGroup
        (
          javax.swing.GroupLayout.Alignment.LEADING
        ).addGap(0, 453, Short.MAX_VALUE)
      );
      jTabbedPane.addTab("3D", kneadEmulator);
      tabIndex = jTabbedPane.getTabCount() - 1;

      this.motionNetEditorPanel.addPropertyChangeListener(this.kneadEmulator);
      this.keyFramePropertyViewer1.addPropertyChangeListener(this.kneadEmulator);
      this.kneadEmulator.addPropertyChangeListener(this.keyFramePropertyViewer1);

    }
    catch(java.lang.UnsatisfiedLinkError e)
    {
      if(tabIndex != -1)
      {
        jTabbedPane.remove(tabIndex);
      }
      Logger.getAnonymousLogger().log(Level.INFO, "setting 3D emulation failed", e);
    }
  }

  private void initKneadEmulatorJOGL()
  {
    int tabIndex = -1;
    try
    {
      kneadEmulatorJOGL = new de.naoth.me.emulation.KneadEmulatorJOGL();
      javax.swing.GroupLayout kneadEmulatorLayoutJOGL = new javax.swing.GroupLayout(kneadEmulatorJOGL);
      kneadEmulatorJOGL.setLayout(kneadEmulatorLayoutJOGL);
      kneadEmulatorLayoutJOGL.setHorizontalGroup
      (
        kneadEmulatorLayoutJOGL.createParallelGroup
        (
          javax.swing.GroupLayout.Alignment.LEADING
        ).addGap(0, 574, Short.MAX_VALUE)
      );
      kneadEmulatorLayoutJOGL.setVerticalGroup
      (
        kneadEmulatorLayoutJOGL.createParallelGroup
        (
          javax.swing.GroupLayout.Alignment.LEADING
        ).addGap(0, 453, Short.MAX_VALUE)
      );

      jTabbedPane.addTab("3D (JOGL)", kneadEmulatorJOGL);
      tabIndex = jTabbedPane.getTabCount() - 1;
    }
    catch(java.lang.UnsatisfiedLinkError e)
    {
      if(tabIndex != -1)
      {
        jTabbedPane.remove(tabIndex);
      }
      Logger.getAnonymousLogger().log(Level.INFO, "setting 3D JOGL emulation failed", e);
    }
  }


  /** This method is called from within the constructor to
   * initialize the form.
   * WARNING: Do NOT modify this code. The content of this method is
   * always regenerated by the Form Editor.
   */
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        jFileChooser = new javax.swing.JFileChooser();
        jSplitPane = new javax.swing.JSplitPane();
        jPanel = new javax.swing.JPanel();
        jTabbedPane = new javax.swing.JTabbedPane();
        jTabbedPane1 = new javax.swing.JTabbedPane();
        motionNetEditorPanel = new de.naoth.me.controls.motionneteditor.MotionNetEditorPanel();
        keyFramePropertyViewer1 = new de.naoth.me.propertyviewer.KeyFramePropertyViewer();
        statusPanel = new javax.swing.JPanel();
        lblConnect = new javax.swing.JLabel();
        motionPlayer1 = new de.naoth.me.motionplayer.MotionPlayer();
        jScrollPane1 = new javax.swing.JScrollPane();
        jTextArea1 = new javax.swing.JTextArea();
        jMenuBar = new javax.swing.JMenuBar();
        jMenuFile = new javax.swing.JMenu();
        jMenuItemNew = new javax.swing.JMenuItem();
        jSeparator1 = new javax.swing.JSeparator();
        jMenuItemOpen = new javax.swing.JMenuItem();
        jMenuItemSave = new javax.swing.JMenuItem();
        jMenuItemSaveAs = new javax.swing.JMenuItem();
        jMenuItem1 = new javax.swing.JMenuItem();
        jMenuItem2 = new javax.swing.JMenuItem();
        jMenuItemExport = new javax.swing.JMenuItem();
        jSeparator2 = new javax.swing.JSeparator();
        jMenuItemExit = new javax.swing.JMenuItem();
        jMenuDebugRequests = new javax.swing.JMenu();
        jMenuItemStandupBack = new javax.swing.JMenuItem();
        jMenuItemStand = new javax.swing.JMenuItem();
        jMenuConnect = new javax.swing.JMenu();
        connectMenuItem = new javax.swing.JMenuItem();
        disconnectMenuItem = new javax.swing.JMenuItem();
        jMenuHelp = new javax.swing.JMenu();
        jMenuItemAbout = new javax.swing.JMenuItem();

        setDefaultCloseOperation(javax.swing.WindowConstants.EXIT_ON_CLOSE);
        setTitle("MotionEditor");
        setLocationByPlatform(true);

        jSplitPane.setDividerLocation(500);
        jSplitPane.setResizeWeight(1.0);

        jPanel.setPreferredSize(new java.awt.Dimension(590, 528));

        javax.swing.GroupLayout jPanelLayout = new javax.swing.GroupLayout(jPanel);
        jPanel.setLayout(jPanelLayout);
        jPanelLayout.setHorizontalGroup(
            jPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jTabbedPane, javax.swing.GroupLayout.DEFAULT_SIZE, 441, Short.MAX_VALUE)
        );
        jPanelLayout.setVerticalGroup(
            jPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jTabbedPane, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, 557, Short.MAX_VALUE)
        );

        jSplitPane.setRightComponent(jPanel);

        motionNetEditorPanel.addPropertyChangeListener(new java.beans.PropertyChangeListener() {
            public void propertyChange(java.beans.PropertyChangeEvent evt) {
                motionNetEditorPanelPropertyChange(evt);
            }
        });
        jTabbedPane1.addTab("MotionNet", motionNetEditorPanel);
        motionNetEditorPanel.getAccessibleContext().setAccessibleParent(jTabbedPane);

        jTabbedPane1.addTab("Joints", keyFramePropertyViewer1);

        jSplitPane.setLeftComponent(jTabbedPane1);

        statusPanel.setPreferredSize(new java.awt.Dimension(1100, 25));

        lblConnect.setText("Not connected");
        lblConnect.setToolTipText("Indicates if the RobotControl is connected to a Robot");

        javax.swing.GroupLayout statusPanelLayout = new javax.swing.GroupLayout(statusPanel);
        statusPanel.setLayout(statusPanelLayout);
        statusPanelLayout.setHorizontalGroup(
            statusPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, statusPanelLayout.createSequentialGroup()
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addComponent(lblConnect)
                .addContainerGap())
        );
        statusPanelLayout.setVerticalGroup(
            statusPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(lblConnect, javax.swing.GroupLayout.DEFAULT_SIZE, 26, Short.MAX_VALUE)
        );

        motionPlayer1.setMinimumSize(null);
        motionPlayer1.setMotionNet(null);
        motionPlayer1.setPreferredSize(null);
        motionPlayer1.setRequestFocusEnabled(false);

        jTextArea1.setColumns(20);
        jTextArea1.setRows(5);
        jScrollPane1.setViewportView(jTextArea1);

        jMenuFile.setText("File");

        jMenuItemNew.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_N, java.awt.event.InputEvent.CTRL_MASK));
        jMenuItemNew.setText("New");
        jMenuItemNew.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItemNewActionPerformed(evt);
            }
        });
        jMenuFile.add(jMenuItemNew);
        jMenuFile.add(jSeparator1);

        jMenuItemOpen.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_O, java.awt.event.InputEvent.CTRL_MASK));
        jMenuItemOpen.setText("Open");
        jMenuItemOpen.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItemOpenActionPerformed(evt);
            }
        });
        jMenuFile.add(jMenuItemOpen);

        jMenuItemSave.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_S, java.awt.event.InputEvent.CTRL_MASK));
        jMenuItemSave.setText("Save");
        jMenuItemSave.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItemSaveActionPerformed(evt);
            }
        });
        jMenuFile.add(jMenuItemSave);

        jMenuItemSaveAs.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_S, java.awt.event.InputEvent.SHIFT_MASK | java.awt.event.InputEvent.CTRL_MASK));
        jMenuItemSaveAs.setText("Save As...");
        jMenuItemSaveAs.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItemSaveAsActionPerformed(evt);
            }
        });
        jMenuFile.add(jMenuItemSaveAs);

        jMenuItem1.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_I, java.awt.event.InputEvent.CTRL_MASK));
        jMenuItem1.setText("Import from Webotsfile v1.0");
        jMenuItem1.setActionCommand("Import Webots File v1.0");
        jMenuItem1.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItemImportActionPerformed(evt);
            }
        });
        jMenuFile.add(jMenuItem1);

        jMenuItem2.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_E, java.awt.event.InputEvent.CTRL_MASK));
        jMenuItem2.setText("Export to Webotsfile v1.0");
        jMenuItem2.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItemExportWebotActionPerformed(evt);
            }
        });
        jMenuFile.add(jMenuItem2);

        jMenuItemExport.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_E, java.awt.event.InputEvent.ALT_MASK | java.awt.event.InputEvent.CTRL_MASK));
        jMenuItemExport.setText("Export to Image");
        jMenuItemExport.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItemExportActionPerformed(evt);
            }
        });
        jMenuFile.add(jMenuItemExport);
        jMenuFile.add(jSeparator2);

        jMenuItemExit.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_Q, java.awt.event.InputEvent.CTRL_MASK));
        jMenuItemExit.setText("Exit / Quit");
        jMenuItemExit.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItemExitActionPerformed(evt);
            }
        });
        jMenuFile.add(jMenuItemExit);

        jMenuBar.add(jMenuFile);

        jMenuDebugRequests.setText("Debug Requests");

        jMenuItemStandupBack.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_B, java.awt.event.InputEvent.ALT_MASK | java.awt.event.InputEvent.CTRL_MASK));
        jMenuItemStandupBack.setText("Stand up from back");
        jMenuItemStandupBack.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItemStandupBackActionPerformed(evt);
            }
        });
        jMenuDebugRequests.add(jMenuItemStandupBack);

        jMenuItemStand.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_S, java.awt.event.InputEvent.ALT_MASK | java.awt.event.InputEvent.CTRL_MASK));
        jMenuItemStand.setText("Stand");
        jMenuItemStand.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItemStandActionPerformed(evt);
            }
        });
        jMenuDebugRequests.add(jMenuItemStand);

        jMenuBar.add(jMenuDebugRequests);

        jMenuConnect.setText("Connect");

        connectMenuItem.setText("Connect");
        connectMenuItem.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                connectMenuItemActionPerformed(evt);
            }
        });
        jMenuConnect.add(connectMenuItem);

        disconnectMenuItem.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_D, java.awt.event.InputEvent.CTRL_MASK));
        disconnectMenuItem.setText("Disconnect");
        disconnectMenuItem.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                disconnectMenuItemActionPerformed(evt);
            }
        });
        jMenuConnect.add(disconnectMenuItem);

        jMenuBar.add(jMenuConnect);

        jMenuHelp.setText("Help");

        jMenuItemAbout.setText("About");
        jMenuHelp.add(jMenuItemAbout);

        jMenuBar.add(jMenuHelp);

        setJMenuBar(jMenuBar);

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
        getContentPane().setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jSplitPane, javax.swing.GroupLayout.DEFAULT_SIZE, 952, Short.MAX_VALUE)
            .addGroup(layout.createSequentialGroup()
                .addComponent(motionPlayer1, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jScrollPane1))
            .addComponent(statusPanel, javax.swing.GroupLayout.DEFAULT_SIZE, 952, Short.MAX_VALUE)
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addComponent(jSplitPane, javax.swing.GroupLayout.DEFAULT_SIZE, 559, Short.MAX_VALUE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                    .addComponent(motionPlayer1, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addComponent(jScrollPane1, javax.swing.GroupLayout.PREFERRED_SIZE, 0, Short.MAX_VALUE))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(statusPanel, javax.swing.GroupLayout.PREFERRED_SIZE, 26, javax.swing.GroupLayout.PREFERRED_SIZE))
        );

        pack();
    }// </editor-fold>//GEN-END:initComponents

    private void jMenuItemExportActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jMenuItemExportActionPerformed
      ExportDialog export = new ExportDialog();
      export.showExportDialog(this, "Export view as ...", this.motionNetEditorPanel, "export");
    }//GEN-LAST:event_jMenuItemExportActionPerformed

    private void jMenuItemStandupBackActionPerformed(java.awt.event.ActionEvent evt) {

    }

    private void jMenuItemStandActionPerformed(java.awt.event.ActionEvent evt) {

    }

    private void jMenuItemNewActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jMenuItemNewActionPerformed
      this.selectedFile = null;

      MotionNet motionNet = new MotionNet(jointDefaultConfiguration);
      this.motionNetEditorPanel.setMotionNet(motionNet);
      this.motionPlayer1.setMotionNet(motionNet);
      this.setTitle("MotionNet Editor");
    }//GEN-LAST:event_jMenuItemNewActionPerformed

    private void jMenuItemOpenActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jMenuItemOpenActionPerformed
      FileChooser.setDialogTitle("Open");      
      for(FileFilter filter : FileChooser.getChoosableFileFilters())
      {
        FileChooser.removeChoosableFileFilter(filter);        
      }      
      FileChooser.addChoosableFileFilter(new SimpleFileFilter("Textfile", "*.txt"));
      FileChooser.addChoosableFileFilter(new SimpleFileFilter("MotionNet Editor File", "*.mef"));
      FileChooser.showOpenDialog(this);
      this.selectedFile = FileChooser.getSelectedFile();
      if(this.selectedFile == null)
      {
        return;
      }
      try
      {
        MotionNet motionNet = null;
        String fileName = this.selectedFile.getName();
        if(fileName.endsWith(".mef")){
          motionNet = motionNetLoader.loadFromFile(this.selectedFile);
        } else if(fileName.endsWith(".txt")){
          motionNet = motionNetLoader.loadFromTxtFile(this.selectedFile);   
        } 
        this.motionNetEditorPanel.setMotionNet(motionNet);
        this.motionPlayer1.setMotionNet(motionNet);
        this.setTitle("MotionNet Editor : " + this.selectedFile.getName());
      }
      catch(IOException e)
      {
        JOptionPane.showMessageDialog(this,
          e.toString(), "The file could not be read.", JOptionPane.ERROR_MESSAGE);
      }//end catch
    }//GEN-LAST:event_jMenuItemOpenActionPerformed

    private void jMenuItemSaveActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jMenuItemSaveActionPerformed
      FileChooser.setDialogTitle("Save");
      for(FileFilter filter : FileChooser.getChoosableFileFilters())
      {
        FileChooser.removeChoosableFileFilter(filter);        
      }      
      FileChooser.addChoosableFileFilter(new SimpleFileFilter("Textfile", "*.txt"));
      FileChooser.addChoosableFileFilter(new SimpleFileFilter("MotionNet Editor File", "*.mef"));
      FileChooser.showSaveDialog(this);
      this.selectedFile = FileChooser.getSelectedFile();
      if(this.selectedFile == null)
      {
        FileChooser.setDialogTitle("Save As");
      for(FileFilter filter : FileChooser.getChoosableFileFilters())
      {
        FileChooser.removeChoosableFileFilter(filter);        
      }      
        FileChooser.addChoosableFileFilter(new SimpleFileFilter("Textfile", "*.txt"));
        FileChooser.addChoosableFileFilter(new SimpleFileFilter("MotionNet Editor File", "*.mef"));
        FileChooser.showSaveDialog(this);
        this.selectedFile = FileChooser.getSelectedFile();
      }

      if(this.selectedFile == null)
      {
        return;
      }
      try
      {
        MotionNet motionNet = null;
        String fileName = this.selectedFile.getName();
        if(fileName.endsWith(".mef")){
          motionNetLoader.saveToFile(this.motionNetEditorPanel.getMotionNet(), this.selectedFile);
        } else if(fileName.endsWith(".txt")){
             motionNetLoader.saveToTxtFile(this.motionNetEditorPanel.getMotionNet(),this.selectedFile);
        } 
        this.motionNetEditorPanel.setMotionNet(motionNet);
        this.motionPlayer1.setMotionNet(motionNet);
        this.setTitle("MotionNet Editor : " + this.selectedFile.getName());
      }
      catch(IOException e)
      {
        JOptionPane.showMessageDialog(this,
          e.toString(), "The file could not be read.", JOptionPane.ERROR_MESSAGE);
      }//end catch
    }//GEN-LAST:event_jMenuItemSaveActionPerformed

    private void jMenuItemSaveAsActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jMenuItemSaveAsActionPerformed
      for(FileFilter filter : FileChooser.getChoosableFileFilters())
      {
        FileChooser.removeChoosableFileFilter(filter);        
      }      
      FileChooser.addChoosableFileFilter(new SimpleFileFilter("Textfile", "*.txt"));
      FileChooser.addChoosableFileFilter(new SimpleFileFilter("MotionNet Editor File", "*.mef"));
      FileChooser.showSaveDialog(this);
      File selectFile = FileChooser.getSelectedFile();
      if(selectFile == null)
      {
        return;
      }
      try
      {
        String fileName = selectFile.getName();
        if(fileName.endsWith(".mef")){
            motionNetLoader.saveToFile(this.motionNetEditorPanel.getMotionNet(), selectFile);
            this.selectedFile = selectFile;
            this.setTitle("MotionNet Editor : " + this.selectedFile.getName());
        }else if(fileName.endsWith(".txt")){
            motionNetLoader.saveToTxtFile(this.motionNetEditorPanel.getMotionNet(), selectFile);
            this.selectedFile = selectFile;
            this.setTitle("MotionNet Editor : " + this.selectedFile.getName());
        }
          
      }
      catch(IOException e)
      {
        JOptionPane.showMessageDialog(this,
          e.toString(), "The file could not be saved.", JOptionPane.ERROR_MESSAGE);
      }//end catch
    }//GEN-LAST:event_jMenuItemSaveAsActionPerformed

    private void jMenuItemExitActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jMenuItemExitActionPerformed
      System.exit(0);
    }//GEN-LAST:event_jMenuItemExitActionPerformed

    private void jMenuItemImportActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jMenuItemImportActionPerformed
      FileChooser.setDialogTitle("Import from Webot File");
      for(FileFilter filter : FileChooser.getChoosableFileFilters())
      {
        FileChooser.removeChoosableFileFilter(filter);        
      }      
      FileChooser.addChoosableFileFilter(new SimpleFileFilter("Webot File V1.0", "*.motion"));
      FileChooser.showOpenDialog(this);
      this.selectedFile = FileChooser.getSelectedFile();
      if(this.selectedFile == null)
      {
        return;
      }
      try
      {
        MotionNet motionNet = motionNetLoader.importFromFile(this.selectedFile);
        this.motionNetEditorPanel.setMotionNet(motionNet);
        this.motionPlayer1.setMotionNet(motionNet);
        this.setTitle("MotionNet Editor (imported): " + this.selectedFile.getName());
        this.selectedFile = null;
      }
      catch(IOException e)
      {
        JOptionPane.showMessageDialog(this,
          e.toString(), "The file could not be read.", JOptionPane.ERROR_MESSAGE);
      }//end catch
      FileChooser.removeChoosableFileFilter(FileChooser.getFileFilter());
      FileChooser.addChoosableFileFilter(new SimpleFileFilter("MotionNet Editor File", "*.mef"));
    }//GEN-LAST:event_jMenuItemImportActionPerformed

    private void jMenuItemExportWebotActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jMenuItemExportWebotActionPerformed
      for(FileFilter filter : FileChooser.getChoosableFileFilters())
      {
        FileChooser.removeChoosableFileFilter(filter);        
      }      
      FileChooser.setDialogTitle("Export to Webot File");
      FileChooser.setDialogType(JFileChooser.SAVE_DIALOG);
      FileChooser.addChoosableFileFilter(new SimpleFileFilter("Webot File V1.0", "*.motion"));
      FileChooser.showSaveDialog(this);
      File selectFile = FileChooser.getSelectedFile();
      if(selectFile == null)
      {
        return;
      }
      try
      {
        motionNetLoader.exportToFile(this.motionNetEditorPanel.getMotionNet(), selectFile);
      }
      catch(IOException e)
      {
        JOptionPane.showMessageDialog(this,
          e.toString(), "The file could not be saved.", JOptionPane.ERROR_MESSAGE);
      }//end catch
      FileChooser.removeChoosableFileFilter(FileChooser.getFileFilter());
      FileChooser.addChoosableFileFilter(new SimpleFileFilter("MotionNet Editor File", "*.mef"));
    }//GEN-LAST:event_jMenuItemExportWebotActionPerformed

    private void connectMenuItemActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_connectMenuItemActionPerformed
      connectionDialog.setVisible(true);
    }//GEN-LAST:event_connectMenuItemActionPerformed

    private void disconnectMenuItemActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_disconnectMenuItemActionPerformed
      messageServer.disconnect();
    }//GEN-LAST:event_disconnectMenuItemActionPerformed

    private void motionNetEditorPanelPropertyChange(java.beans.PropertyChangeEvent evt) {//GEN-FIRST:event_motionNetEditorPanelPropertyChange
      // TODO add your handling code here:
    }//GEN-LAST:event_motionNetEditorPanelPropertyChange

    
  /**
   * Set the controls as if we are disconnected or not
   * @param isConnected
   */
  public void showConnected(boolean isConnected)
  {
    disconnectMenuItem.setEnabled(isConnected);
    connectMenuItem.setEnabled(!isConnected);
    
    if(isConnected)
    {
      lblConnect.setText("Connected to " + messageServer.getAddress().toString());
    }
    else
    {
      lblConnect.setText("Not connected");
    }
  }//end showConnected


  public MessageServer getMessageServer()
  {
    return messageServer;
  }

  public Properties getConfig()
  {
    // TODO: load the config from file
    if(config == null)
    {
      config = new Properties();
    }
    
    return config;
  }//end getConfig

  /**
   * @param args the command line arguments
   */
  public static void main(String args[])
  {
    java.awt.EventQueue.invokeLater(new Runnable()
    {

      public void run()
      {
        new MotionEditor().setVisible(true);
      }
    });
  }//end main

  // Ende Methoden
  public class SimpleFileFilter extends FileFilter
  {
    private String desc;

  // Anfang Attribute1
    private List<String> extensions;
    private boolean showDirectories;

  // Ende Attribute1

    /**
    * @param name example: "Data files"
    * @param glob example: "*.txt|*.csv"
    */
    public SimpleFileFilter (String name, String globs)
    {
      this.showDirectories = true;
      extensions = new ArrayList<String>();
      for (String glob : globs.split("\\|"))
      {
        if (!glob.startsWith("*."))
          throw new IllegalArgumentException("expected list of globs like \"*.txt|*.csv\"");
        // cut off "*"
        // store only lower case (make comparison case insensitive)
        extensions.add (glob.substring(1).toLowerCase());
      }
      desc = name + " (" + globs + ")";
    }

    public SimpleFileFilter(String name, String globs, boolean showDirectories)
    {
      this(name, globs);
      this.showDirectories = showDirectories;
    }

    // Anfang Komponenten1
    // Ende Komponenten1


  // Anfang Methoden1
    public boolean accept(File file)
    {
      if(showDirectories && file.isDirectory())
      {
        return true;
      }
      String fileName = file.toString().toLowerCase();

      for (String extension : extensions)
      {
        if (fileName.endsWith (extension))
        {
          return true;
        }
      }
      return false;
    }

    public String getDescription()
    {
      return desc;
    }

    /**
    * @return includes '.'
    */
    public String getFirstExtension()
    {
      return extensions.get(0);
    }
  }
  // Ende Methoden1
  private de.naoth.me.emulation.KneadEmulator kneadEmulator;
  private de.naoth.me.emulation.KneadEmulatorJOGL kneadEmulatorJOGL;

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JMenuItem connectMenuItem;
    private javax.swing.JMenuItem disconnectMenuItem;
    private javax.swing.JFileChooser jFileChooser;
    private javax.swing.JMenuBar jMenuBar;
    private javax.swing.JMenu jMenuConnect;
    private javax.swing.JMenu jMenuDebugRequests;
    private javax.swing.JMenu jMenuFile;
    private javax.swing.JMenu jMenuHelp;
    private javax.swing.JMenuItem jMenuItem1;
    private javax.swing.JMenuItem jMenuItem2;
    private javax.swing.JMenuItem jMenuItemAbout;
    private javax.swing.JMenuItem jMenuItemExit;
    private javax.swing.JMenuItem jMenuItemExport;
    private javax.swing.JMenuItem jMenuItemNew;
    private javax.swing.JMenuItem jMenuItemOpen;
    private javax.swing.JMenuItem jMenuItemSave;
    private javax.swing.JMenuItem jMenuItemSaveAs;
    private javax.swing.JMenuItem jMenuItemStand;
    private javax.swing.JMenuItem jMenuItemStandupBack;
    private javax.swing.JPanel jPanel;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JSeparator jSeparator1;
    private javax.swing.JSeparator jSeparator2;
    private javax.swing.JSplitPane jSplitPane;
    private javax.swing.JTabbedPane jTabbedPane;
    private javax.swing.JTabbedPane jTabbedPane1;
    private javax.swing.JTextArea jTextArea1;
    private de.naoth.me.propertyviewer.KeyFramePropertyViewer keyFramePropertyViewer1;
    private javax.swing.JLabel lblConnect;
    private de.naoth.me.controls.motionneteditor.MotionNetEditorPanel motionNetEditorPanel;
    private de.naoth.me.motionplayer.MotionPlayer motionPlayer1;
    private javax.swing.JPanel statusPanel;
    // End of variables declaration//GEN-END:variables
  // Ende Attribute

}
