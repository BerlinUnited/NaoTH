/*
 * RobotControlGUI.java
 *
 * Created on 08.10.2010, 16:31:30
 */
package de.naoth.rc;

import de.naoth.rc.core.dialog.Dialog;
import bibliothek.gui.DockUI;
import bibliothek.gui.dock.util.laf.Nimbus6u10;
import de.naoth.rc.components.preferences.PreferencesDialog;
import de.naoth.rc.server.ConnectionDialog;
import de.naoth.rc.server.ConnectionStatusEvent;
import de.naoth.rc.server.ConnectionStatusListener;
import de.naoth.rc.server.MessageServer;
import java.awt.*;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.io.*;
import java.lang.reflect.Field;
import java.net.URISyntaxException;
import java.util.Properties;
import java.util.logging.Level;
import java.util.logging.LogManager;
import java.util.logging.Logger;
import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;
import net.xeoh.plugins.base.PluginManager;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.events.PluginLoaded;
import net.xeoh.plugins.base.impl.PluginManagerFactory;
import net.xeoh.plugins.base.util.JSPFProperties;
import net.xeoh.plugins.base.util.uri.ClassURI;

/**
 *
 * @author thomas
 */
@PluginImplementation
public class RobotControlImpl extends javax.swing.JFrame
  implements ByteRateUpdateHandler, RobotControl
{

  private static final String USER_CONFIG_DIR = System.getProperty("user.home")
    + "/.naoth/robotcontrol/";
  private final File userLayoutFile = new File(USER_CONFIG_DIR, "layout_df1.1.1.xml");
  private final File userConfigFile = new File(USER_CONFIG_DIR, "config");
  
  private final MessageServer messageServer;
  
  private final Properties config = new Properties();
  private final PreferencesDialog preferencesDialog;
  private final ConnectionDialog connectionDialog;
  
  private final DialogRegistry dialogRegistry;

  // remember the window position and size to restore it later
  private Rectangle defaultWindowBounds = new Rectangle();
  
  private static final Logger logger = Logger.getLogger(RobotControlImpl.class.getName());
  private static Logger getLogger() { return logger; }
  
  // HACK: set the path to the native libs
  static 
  {
  	// load the logger properties
    InputStream stream = RobotControlImpl.class.getResourceAsStream("logging.properties");
    try {
        LogManager.getLogManager().readConfiguration(stream);
    } catch (IOException e) {
        e.printStackTrace();
    }
    
    try
    {
        String separator = System.getProperty("path.separator");
        String path = System.getProperty("java.library.path", "./bin" );
        
        String arch = System.getProperty("os.arch").toLowerCase();
        String name = System.getProperty("os.name").toLowerCase();
        
        if("linux".equals(name)) {
            if("amd64".equals(arch)) {
                path += separator + "./bin/linux64";
            } else {
                path += separator + "./bin/linux32";
            }
        } else {
            if("amd64".equals(arch)) {
                path += separator + "./bin/win64";
            } else {
                path += separator + "./bin/win32";
            }
            path += separator + "./bin/macos";
        }
        
        System.setProperty("java.library.path", path );
        
        System.getProperties().list(System.out);

        Field fieldSysPath = ClassLoader.class.getDeclaredField( "sys_paths" );
        fieldSysPath.setAccessible( true );
        fieldSysPath.set( null, null );

        getLogger().log(Level.INFO, 
            "Set java.library.path={0}", System.getProperty("java.library.path", "./bin" ));
    } catch(IllegalAccessException | NoSuchFieldException  ex) {
        getLogger().log(Level.SEVERE, "[ERROR] could not set the java.library.path", ex);
    }
  }
  
  
  /**
   * Creates new form RobotControlGUI
   */
  public RobotControlImpl()
  {  
    splashScreenMessage("Welcome to RobotControl");
    
    // load the configuration
    readConfigFromFile();
    
    try
    {
      //UIManager.setLookAndFeel(new PlasticXPLookAndFeel());
      UIManager.setLookAndFeel(new CustomNimbusLookAndFeel(RobotControlImpl.this));
      // set explicitely the Nimbus colors to be used
      DockUI.getDefaultDockUI().registerColors("de.naoth.rc.CustomNimbusLookAndFeel", new Nimbus6u10());
    }
    catch(UnsupportedLookAndFeelException ex)
    {
      getLogger().log(Level.SEVERE, null, ex);
    }
    
    // icon
    Image icon = Toolkit.getDefaultToolkit().getImage(
      this.getClass().getResource("res/RobotControlLogo128.png"));
    setIconImage(icon);

    initComponents();

    // restore the bounds and the state of the frame from the config
    defaultWindowBounds = getBounds();
    defaultWindowBounds.x = readValueFromConfig("frame.position.x", defaultWindowBounds.x);
    defaultWindowBounds.y = readValueFromConfig("frame.position.y", defaultWindowBounds.y);
    defaultWindowBounds.width = readValueFromConfig("frame.width", defaultWindowBounds.width);
    defaultWindowBounds.height = readValueFromConfig("frame.height", defaultWindowBounds.height);
    int extendedstate = readValueFromConfig("frame.extendedstate", getExtendedState());
    setBounds(defaultWindowBounds);
    setExtendedState(extendedstate);
    
    // remember the bounds of the frame when not maximized
    this.addComponentListener(new ComponentAdapter() {
        @Override
        public void componentMoved(ComponentEvent event) {
            if ((getExtendedState() & JFrame.MAXIMIZED_BOTH) != JFrame.MAXIMIZED_BOTH) {
                defaultWindowBounds = getBounds();
            }
        }
    });
    
    // set up a list of all dialogs
    this.dialogRegistry = new DialogRegistry(this, this.mainMenuBar);

    
    // initialize the message server
    this.messageServer = new MessageServer();
    this.messageServer.addConnectionStatusListener(new ConnectionStatusListener() 
    {
        @Override
        public void connected(ConnectionStatusEvent event) {
            disconnectMenuItem.setEnabled(true);
            connectMenuItem.setEnabled(false);
            lblConnect.setText("Connected to " + event.getAddress());
        }

        @Override
        public void disconnected(ConnectionStatusEvent event) {
            disconnectMenuItem.setEnabled(false);
            connectMenuItem.setEnabled(true);
            lblConnect.setText("Not connected");
            if(event.getMessage() != null) {
                JOptionPane.showMessageDialog(RobotControlImpl.this,
                    event.getMessage(), "Disconnect", JOptionPane.ERROR_MESSAGE);
            }
        }
    });

    // connection dialog
    this.connectionDialog = new ConnectionDialog(this, this.messageServer, this.getConfig());
    this.connectionDialog.setLocationRelativeTo(this);
    this.disconnectMenuItem.setEnabled(false);
    // preference dialog
    this.preferencesDialog = new PreferencesDialog(this, this.getConfig());
    this.preferencesDialog.setLocationRelativeTo(this);
  }//end constructor

  
  private int readValueFromConfig(String key, int default_value) {
    try {
      String value = getConfig().getProperty(key);
      if(value != null) {
        return Integer.parseInt(value);
      }
    } catch(NumberFormatException e){}
    return default_value;
  }
  
  private void splashScreenMessage(String message)
  {
    final SplashScreen splash = SplashScreen.getSplashScreen();
    if(splash == null)
    {
      return;
    }
    Graphics2D g = splash.createGraphics();
    if(g == null)
    {
      return;
    }
    
    int x = 120;
    int y = 140;
    
    g.setComposite(AlphaComposite.Clear);
    g.fillRect(x-10,y-10,290,100);
    g.setPaintMode();
    g.setColor(Color.BLACK);
    g.drawString(message, x, y);
    
    splash.update();
  }

  @PluginLoaded
  public void registerDialog(final Dialog dialog)
  {
    splashScreenMessage("Loading dialog " + dialog.getDisplayName() + "...");
    this.dialogRegistry.registerDialog(dialog);
  }

  @Override
  public boolean checkConnected()
  {
    if(enforceConnection.isSelected() && !messageServer.isConnected())
    {
      connectionDialog.setVisible(true);
      return messageServer.isConnected();
    }
    else
    {
      return true;
    }
  }//end checkConnected

  /**
   * This method is called from within the constructor to initialize the form.
   * WARNING: Do NOT modify this code. The content of this method is always
   * regenerated by the Form Editor.
   */
  @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        statusPanel = new javax.swing.JPanel();
        lblConnect = new javax.swing.JLabel();
        btManager = new javax.swing.JButton();
        lblReceivedBytesS = new javax.swing.JLabel();
        lblSentBytesS = new javax.swing.JLabel();
        lblFramesS = new javax.swing.JLabel();
        statusPanelPlugins = new javax.swing.JPanel();
        mainMenuBar = new de.naoth.rc.MainMenuBar();
        mainControlMenu = new javax.swing.JMenu();
        connectMenuItem = new javax.swing.JMenuItem();
        disconnectMenuItem = new javax.swing.JMenuItem();
        enforceConnection = new javax.swing.JCheckBoxMenuItem();
        resetLayoutMenuItem = new javax.swing.JMenuItem();
        preferences = new javax.swing.JMenuItem();
        jSeparator1 = new javax.swing.JSeparator();
        exitMenuItem = new javax.swing.JMenuItem();
        helpMenu = new javax.swing.JMenu();
        aboutMenuItem = new javax.swing.JMenuItem();

        setDefaultCloseOperation(javax.swing.WindowConstants.EXIT_ON_CLOSE);
        setTitle("RobotControl for Nao v2015");
        addWindowListener(new java.awt.event.WindowAdapter() {
            public void windowClosing(java.awt.event.WindowEvent evt) {
                formWindowClosing(evt);
            }
        });

        statusPanel.setBackground(java.awt.Color.lightGray);
        statusPanel.setPreferredSize(new java.awt.Dimension(966, 25));

        lblConnect.setText("Not connected");
        lblConnect.setToolTipText("Indicates if the RobotControl is connected to a Robot");

        btManager.setText("Running Manager --");
        btManager.setToolTipText("Shows the number of currently registered Manager");
        btManager.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btManagerActionPerformed(evt);
            }
        });

        lblReceivedBytesS.setText("Recived byte/s: ");

        lblSentBytesS.setText("Sent byte/s: ");

        lblFramesS.setText("Frames/s: ");

        statusPanelPlugins.setFocusable(false);
        statusPanelPlugins.setMaximumSize(new java.awt.Dimension(32767, 24));
        statusPanelPlugins.setMinimumSize(new java.awt.Dimension(0, 24));
        statusPanelPlugins.setOpaque(false);
        statusPanelPlugins.setPreferredSize(new java.awt.Dimension(100, 24));
        statusPanelPlugins.setLayout(new java.awt.FlowLayout(java.awt.FlowLayout.RIGHT));

        javax.swing.GroupLayout statusPanelLayout = new javax.swing.GroupLayout(statusPanel);
        statusPanel.setLayout(statusPanelLayout);
        statusPanelLayout.setHorizontalGroup(
            statusPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(statusPanelLayout.createSequentialGroup()
                .addComponent(btManager, javax.swing.GroupLayout.PREFERRED_SIZE, 121, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(lblReceivedBytesS, javax.swing.GroupLayout.PREFERRED_SIZE, 173, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(lblSentBytesS, javax.swing.GroupLayout.PREFERRED_SIZE, 164, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(lblFramesS, javax.swing.GroupLayout.PREFERRED_SIZE, 173, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(statusPanelPlugins, javax.swing.GroupLayout.DEFAULT_SIZE, 189, Short.MAX_VALUE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(lblConnect)
                .addContainerGap())
        );
        statusPanelLayout.setVerticalGroup(
            statusPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(statusPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                .addComponent(btManager, javax.swing.GroupLayout.PREFERRED_SIZE, 25, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addComponent(lblConnect, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addComponent(lblReceivedBytesS)
                .addComponent(lblSentBytesS)
                .addComponent(lblFramesS))
            .addGroup(statusPanelLayout.createSequentialGroup()
                .addComponent(statusPanelPlugins, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(0, 0, Short.MAX_VALUE))
        );

        getContentPane().add(statusPanel, java.awt.BorderLayout.PAGE_END);

        mainControlMenu.setMnemonic('R');
        mainControlMenu.setText("Main");

        connectMenuItem.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_K, java.awt.event.InputEvent.CTRL_MASK));
        connectMenuItem.setMnemonic('c');
        connectMenuItem.setText("Connect");
        connectMenuItem.setToolTipText("Connect to robot");
        connectMenuItem.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                connectMenuItemActionPerformed(evt);
            }
        });
        mainControlMenu.add(connectMenuItem);

        disconnectMenuItem.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_D, java.awt.event.InputEvent.CTRL_MASK));
        disconnectMenuItem.setMnemonic('d');
        disconnectMenuItem.setText("Disconnect");
        disconnectMenuItem.setToolTipText("Disconnect from robot");
        disconnectMenuItem.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                disconnectMenuItemActionPerformed(evt);
            }
        });
        mainControlMenu.add(disconnectMenuItem);

        enforceConnection.setSelected(true);
        enforceConnection.setText("Enforce Connection");
        enforceConnection.setToolTipText("Make sure that RobotControl is connected to a robot when dialogs try to receive data");
        mainControlMenu.add(enforceConnection);

        resetLayoutMenuItem.setText("Reset layout");
        resetLayoutMenuItem.setToolTipText("\"Resets all layout information");
        resetLayoutMenuItem.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                resetLayoutMenuItemActionPerformed(evt);
            }
        });
        mainControlMenu.add(resetLayoutMenuItem);

        preferences.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_P, java.awt.event.InputEvent.CTRL_MASK));
        preferences.setText("Preferences");
        preferences.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                preferencesActionPerformed(evt);
            }
        });
        mainControlMenu.add(preferences);
        mainControlMenu.add(jSeparator1);

        exitMenuItem.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_X, java.awt.event.InputEvent.CTRL_MASK));
        exitMenuItem.setMnemonic('e');
        exitMenuItem.setText("Exit");
        exitMenuItem.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                exitMenuItemActionPerformed(evt);
            }
        });
        mainControlMenu.add(exitMenuItem);

        mainMenuBar.add(mainControlMenu);

        helpMenu.setMnemonic('h');
        helpMenu.setText("Help");

        aboutMenuItem.setMnemonic('a');
        aboutMenuItem.setText("About");
        aboutMenuItem.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                aboutMenuItemActionPerformed(evt);
            }
        });
        helpMenu.add(aboutMenuItem);

        helpMenu.setComponentOrientation(ComponentOrientation.RIGHT_TO_LEFT);
        mainMenuBar.add(javax.swing.Box.createHorizontalGlue());

        mainMenuBar.add(helpMenu);

        setJMenuBar(mainMenuBar);

        setSize(new java.awt.Dimension(974, 626));
        setLocationRelativeTo(null);
    }// </editor-fold>//GEN-END:initComponents

    private void connectMenuItemActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_connectMenuItemActionPerformed
    {//GEN-HEADEREND:event_connectMenuItemActionPerformed
      connectionDialog.setVisible(true);
    }//GEN-LAST:event_connectMenuItemActionPerformed

    private void disconnectMenuItemActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_disconnectMenuItemActionPerformed
    {//GEN-HEADEREND:event_disconnectMenuItemActionPerformed

      messageServer.disconnect();

    }//GEN-LAST:event_disconnectMenuItemActionPerformed

    private void exitMenuItemActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_exitMenuItemActionPerformed
    {//GEN-HEADEREND:event_exitMenuItemActionPerformed

      beforeClose();
      System.exit(0);

    }//GEN-LAST:event_exitMenuItemActionPerformed

    private void aboutMenuItemActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_aboutMenuItemActionPerformed
    {//GEN-HEADEREND:event_aboutMenuItemActionPerformed

      AboutDialog dlg = new AboutDialog(this, true);
      dlg.setLocationRelativeTo(this);
      dlg.setVisible(true);

    }//GEN-LAST:event_aboutMenuItemActionPerformed

    private void btManagerActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_btManagerActionPerformed
    {//GEN-HEADEREND:event_btManagerActionPerformed

      TaskManager taskManager = new TaskManager(this, true);
      //String str = "Currently registeres Manager:\n\n";
      for(int i = 0; i < messageServer.getListeners().size(); i++)
      {
        String name = messageServer.getListeners().get(i).getClass().getSimpleName();
        taskManager.addTask(name, "0", null);
        //str += messageServer.getListeners().get(i).getClass().getSimpleName() + "\n";
      }//end for
      //str += "\n";

      taskManager.setVisible(true);
      //JOptionPane.showMessageDialog(this, str);
}//GEN-LAST:event_btManagerActionPerformed

    private void resetLayoutMenuItemActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_resetLayoutMenuItemActionPerformed
    {//GEN-HEADEREND:event_resetLayoutMenuItemActionPerformed

        this.dialogRegistry.setDefaultLayout();
    }//GEN-LAST:event_resetLayoutMenuItemActionPerformed

    private void formWindowClosing(java.awt.event.WindowEvent evt)//GEN-FIRST:event_formWindowClosing
    {//GEN-HEADEREND:event_formWindowClosing

      beforeClose();

    }//GEN-LAST:event_formWindowClosing

    private void preferencesActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_preferencesActionPerformed
        preferencesDialog.setVisible(true);
    }//GEN-LAST:event_preferencesActionPerformed

  @Override
  public MessageServer getMessageServer()
  {
    return messageServer;
  }

  /**
   * @param args the command line arguments
   */
  public static void main(final String args[])
  {
    java.awt.EventQueue.invokeLater(new Runnable()
    {

      @Override
      public void run()
      {
        // create the configlocation is not existing
        File configDir = new File(USER_CONFIG_DIR);
        if(!(configDir.exists() && configDir.isDirectory()) && !configDir.mkdirs()) {
            getLogger().log(Level.SEVERE, "Could not create the configuration path: \"{0}\".", USER_CONFIG_DIR);
        }
        
        final JSPFProperties props = new JSPFProperties();
        props.setProperty(PluginManager.class, "cache.enabled", "false");
//        props.setProperty(PluginManager.class, "cache.mode", "strong"); //optional
//        props.setProperty(PluginManager.class, "cache.file", configlocation+"robot-control.jspf.cache");

        PluginManager pluginManager = PluginManagerFactory.createPluginManager(props);

        try
        {
          // make sure the main frame if loaded first
          pluginManager.addPluginsFrom(new ClassURI(RobotControlImpl.class).toURI());

          File selfFile = new File(RobotControlImpl.class.getProtectionDomain().getCodeSource().getLocation().toURI());
          // load all plugins from 
          pluginManager.addPluginsFrom(selfFile.toURI());

          // NOTE: this is very slow to search in the whole classpath
          //pluginManager.addPluginsFrom(new URI("classpath://*"));
          
          // JFX plugins
          {
            File parentDir = selfFile.getParentFile();
            while(parentDir != null && !"robotcontrol".equalsIgnoreCase(parentDir.getName()))
            {
              parentDir = parentDir.getParentFile();
            }
            if(parentDir != null)
            {
              File jfxCandidate = new File(parentDir, "JFXPlugins/dist");
              File[] jarFiles = jfxCandidate.listFiles(new FileFilter()
              {
                @Override
                public boolean accept(File pathname)
                {
                  return pathname.isFile() && pathname.getName().endsWith(".jar");
                }
              });
              if(jarFiles != null)
              {
                for (File j : jarFiles)
                {
                  pluginManager.addPluginsFrom(j.toURI());
                }
              }
            }
          }
          
          
          
          // relative "plugins/" directory
          File workingDirectoryPlugin = new File("plugins/");
          if(workingDirectoryPlugin.isDirectory())
          {
            pluginManager.addPluginsFrom(workingDirectoryPlugin.toURI());
          }

          //
          File userHomePlugin = new File(System.getProperty("user.home")
            + "/.naoth/robotcontrol/plugins/");
          if(userHomePlugin.isDirectory())
          {
            pluginManager.addPluginsFrom(userHomePlugin.toURI());
          }

          // load the robot control itself
          pluginManager.getPlugin(RobotControl.class).setVisible(true);
        }
        catch(URISyntaxException ex)
        {
          getLogger().log(Level.SEVERE, null, ex);
        }
      }
    });
  }
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JMenuItem aboutMenuItem;
    private javax.swing.JButton btManager;
    private javax.swing.JMenuItem connectMenuItem;
    private javax.swing.JMenuItem disconnectMenuItem;
    private javax.swing.JCheckBoxMenuItem enforceConnection;
    private javax.swing.JMenuItem exitMenuItem;
    private javax.swing.JMenu helpMenu;
    private javax.swing.JSeparator jSeparator1;
    private javax.swing.JLabel lblConnect;
    private javax.swing.JLabel lblFramesS;
    private javax.swing.JLabel lblReceivedBytesS;
    private javax.swing.JLabel lblSentBytesS;
    private javax.swing.JMenu mainControlMenu;
    private de.naoth.rc.MainMenuBar mainMenuBar;
    private javax.swing.JMenuItem preferences;
    private javax.swing.JMenuItem resetLayoutMenuItem;
    private javax.swing.JPanel statusPanel;
    private javax.swing.JPanel statusPanelPlugins;
    // End of variables declaration//GEN-END:variables

    
    @Override
    public void setVisible(boolean t)
    {
        splashScreenMessage("Loading layout and open dialogs ...");
        loadLayout();
        super.setVisible(t);
    }

  @Override
  final public Properties getConfig()
  {
    return config;
  }

  private void beforeClose()
  {
    messageServer.disconnect();
    
    // remember the window size and position
    getConfig().put("frame.position.x", Integer.toString(defaultWindowBounds.x));
    getConfig().put("frame.position.y", Integer.toString(defaultWindowBounds.y));
    getConfig().put("frame.width", Integer.toString(defaultWindowBounds.width));
    getConfig().put("frame.height", Integer.toString(defaultWindowBounds.height));
    getConfig().put("frame.extendedstate", Integer.toString(getExtendedState()));
    
    try
    {
      // save configuration to file
      new File(USER_CONFIG_DIR).mkdirs();
      getConfig().store(new FileWriter(userConfigFile), "");

      // save layout
     this.dialogRegistry.saveToFile(userLayoutFile);
     // notify all dialogs, so they have the chance to clean up
     this.dialogRegistry.disposeOnClose();
    }
    catch(IOException ex)
    {
      Helper.handleException(ex);
    }
  }

  private void readConfigFromFile()
  {
    splashScreenMessage("Loading config file ...");
    try {
      // load main config
      Properties mainConfig = new Properties();
      mainConfig.load(RobotControlImpl.class.getResourceAsStream("config"));
      config.putAll(mainConfig);
      
      if(userConfigFile.exists() && !userConfigFile.isDirectory()) {
        Properties userConfig = new Properties();
        userConfig.load(new FileReader(userConfigFile));
        config.putAll(userConfig);
      }
      
    } catch(IOException ex) {
        getLogger().info("Could not open the config file. It will be created after the first execution.");
    }
  }

  private void loadLayout()
  {
    try {
      this.dialogRegistry.loadFromFile(userLayoutFile);
    } catch(FileNotFoundException ex) {
      getLogger().log(Level.INFO, "Could not find the layout file: {0}", userLayoutFile.getAbsolutePath());
    } catch(IOException ex) {
        Helper.handleException("Error while reading the layout file.", ex);
    }
  }//end configureDocking

  @Override
  public void setReceiveByteRate(double rate)
  {
    lblReceivedBytesS.setText(String.format("Received KB/s: %4.2f", rate));
  }

  @Override
  public void setSentByteRate(double rate)
  {
    lblSentBytesS.setText(String.format("Sent KB/s: %4.2f", rate));
  }
  
  @Override
  public void setServerLoopFPS(double fps)
  {
    lblFramesS.setText(String.format("Frames/s: %4.2f", fps));
  }
  
  @Override
  public int getFontSize() 
  {
      int size = UIManager.getDefaults().getFont("defaultFont") != null ? UIManager.getDefaults().getFont("defaultFont").getSize() : 12;
      return Integer.parseInt(this.getConfig().getProperty("fontSize", String.valueOf(size)));
  }
  
  public void addToStatusBar(Component c) {
      this.statusPanelPlugins.add(c);
  }
}
