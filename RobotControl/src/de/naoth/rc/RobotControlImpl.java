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
import de.naoth.rc.core.server.ConnectionManager;
import de.naoth.rc.core.server.ConnectionStatusEvent;
import de.naoth.rc.core.server.ConnectionStatusListener;
import de.naoth.rc.core.server.MessageServer;
import de.naoth.rc.statusbar.StatusbarPluginImpl;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.awt.event.InputEvent;
import java.awt.event.KeyEvent;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.*;
import java.lang.invoke.MethodHandle;
import java.lang.invoke.MethodHandles;
import java.lang.invoke.MethodHandles.Lookup;
import java.lang.reflect.Field;
import java.net.URISyntaxException;
import java.net.URL;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Properties;
import java.util.logging.Level;
import java.util.logging.LogManager;
import java.util.logging.Logger;
import javax.swing.JComponent;
import javax.swing.JFrame;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.KeyStroke;
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
  private final ConnectionManager connectionManager;
  
  private final DialogRegistry dialogRegistry;
  private final HelpDialog helpDialog = new HelpDialog(this);

  // remember the window position and size to restore it later
  private Rectangle defaultWindowBounds = new Rectangle();
  
  private static final Logger logger = Logger.getLogger(RobotControlImpl.class.getName());
  private static Logger getLogger() { return logger; }
  
  private final GridBagConstraints statusPanelPluginsConstraints = new GridBagConstraints();
  
  //FIXME: why is this needed?
  //private final String RC_TITLE = "";
  
  // HACK: set the path to the native libs
  static 
  {
  	// load the logger properties
    InputStream stream = RobotControlImpl.class.getResourceAsStream("logging.properties");
    try {
        LogManager.getLogManager().readConfiguration(stream);
    } catch (IOException e) {
        e.printStackTrace(System.err);
    }
    
    try
    {
        // we need an absolute path to import native libs
        // in netbeans we can use the relative execution path
        File bin = new File("./bin");
        if (!bin.isDirectory()) {
            // with a jar file, we need to determine the correct path relative to the jar file
            File jar = new File(RobotControlImpl.class.getProtectionDomain().getCodeSource().getLocation().toURI());
            // ../bin
            bin = new File(jar.getParentFile().getParentFile(), "bin");
        }
        
        String arch = System.getProperty("os.arch").toLowerCase();
        String name = System.getProperty("os.name").toLowerCase();
        
        if("linux".equals(name)) {
            if("amd64".equals(arch)) {
                addLibraryPath(bin.getAbsolutePath() + "/linux64");
            } else {
                addLibraryPath(bin.getAbsolutePath() + "/linux32");
            }
        } else {
            if("amd64".equals(arch)) {
                addLibraryPath(bin.getAbsolutePath() + "/win64");
            } else {
                addLibraryPath(bin.getAbsolutePath() + "/win32");
            }
            addLibraryPath(bin.getAbsolutePath() + "/macos");
        }
        
        // list all properteies
        System.getProperties().list(System.out);

    } catch (Throwable ex) {
        Logger.getLogger(RobotControlImpl.class.getName()).log(Level.SEVERE, null, ex);
    }
  }

    /**
     * Adds the specified path to the java library path
     * Option 2 from: https://stackoverflow.com/a/15409446
     * 
     * Black Magic to get access to private fields in Java > 9:
     * https://stackoverflow.com/questions/28184065/java-8-access-private-member-with-lambda
     * https://gist.github.com/Andrei-Pozolotin/dc8b448dc590183f5459
     * 
     * @param pathToAdd the path to add
     * @throws Exception
     */
    public static void addLibraryPath(String pathToAdd) throws Throwable {
        
        // Define black magic: IMPL_LOOKUP is "trusted" and can access prvae variables.
        final Lookup original = MethodHandles.lookup();
        final Field internal = Lookup.class.getDeclaredField("IMPL_LOOKUP");
        internal.setAccessible(true);
        final Lookup trusted = (Lookup) internal.get(original);
        
        // Invoke black magic. Get access to the private field usr_paths
        MethodHandle set = trusted.findStaticSetter(ClassLoader.class, "usr_paths", String[].class);
        MethodHandle get = trusted.findStaticGetter(ClassLoader.class, "usr_paths", String[].class);
        
        //get array of paths
        final String[] paths = (String[]) get.invoke();

        //check if the path to add is already present
        for (String path : paths) {
            if (path.equals(pathToAdd)) {
                return;
            }
        }

        //add the new path
        final String[] newPaths = Arrays.copyOf(paths, paths.length + 1);
        newPaths[newPaths.length - 1] = pathToAdd;
        set.invoke(newPaths);
    }

  /**
   * Creates new form RobotControlGUI
   */
  public RobotControlImpl()
  {  
    splashScreenMessage("Welcome to RobotControl");
    
    // load the configuration
    readConfigFromFile();
    
    try {
      //UIManager.setLookAndFeel(new PlasticXPLookAndFeel());
      UIManager.setLookAndFeel(new CustomNimbusLookAndFeel(RobotControlImpl.this));
      // set explicitely the Nimbus colors to be used
      DockUI.getDefaultDockUI().registerColors("de.naoth.rc.CustomNimbusLookAndFeel", new Nimbus6u10());
    }
    catch(UnsupportedLookAndFeelException ex) {
      getLogger().log(Level.SEVERE, null, ex);
    }
    
    // icon
    Image icon = Toolkit.getDefaultToolkit().getImage(this.getClass().getResource("res/RobotControlLogo128.png"));
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
    this.dialogRegistry = new DialogRegistry(this, this.mainMenuBar, this.dialogFastAccessPanel, helpDialog);

    
    // initialize the message server
    this.messageServer = new MessageServer();
    this.messageServer.addConnectionStatusListener(new ConnectionStatusListener() 
    {
        @Override
        public void connected(ConnectionStatusEvent event) {
            disconnectMenuItem.setEnabled(true);
            connectMenuItem.setEnabled(false);
        }

        @Override
        public void disconnected(ConnectionStatusEvent event) {
            
            disconnectMenuItem.setEnabled(false);
            connectMenuItem.setEnabled(true);
            
            if(event.getMessage() != null) {
                connectionManager.showConnectionDialog(event.getMessage());
            }
        }
    });

    this.connectionManager = new ConnectionManager(this, this.messageServer, this.getConfig());
    
    this.disconnectMenuItem.setEnabled(false);
    // preference dialog
    this.preferencesDialog = new PreferencesDialog(this, this.getConfig());
    this.preferencesDialog.setLocationRelativeTo(this);

    // set the constraints for the statusbar plugins
    statusPanelPluginsConstraints.fill = GridBagConstraints.VERTICAL;
    statusPanelPluginsConstraints.ipadx = 5;
    
    
    // fast dialog access
    dialogFastAccess.setLocationRelativeTo(this);
    dialogFastAccess.getRootPane().registerKeyboardAction(
        (e) -> {dialogFastAccess.setVisible(false); dialogFastAccessPanel.close(); }, 
        KeyStroke.getKeyStroke(KeyEvent.VK_ESCAPE, 0), 
        JComponent.WHEN_IN_FOCUSED_WINDOW
    );
    // close fast dialog access when RobotControll is in the background
    dialogFastAccess.addWindowFocusListener(new WindowAdapter() {
        @Override
        public void windowLostFocus(WindowEvent e) {
            dialogFastAccess.setVisible(false); 
            dialogFastAccessPanel.close();
        }
    });
    
    // make fast dialog access visible
    DefaultKeyboardFocusManager.getCurrentKeyboardFocusManager().addKeyEventPostProcessor(
    (e) -> {
        if(e.getID()        == KeyEvent.KEY_PRESSED && 
           e.getKeyCode()   == KeyEvent.VK_F && 
           e.getModifiers() == InputEvent.ALT_MASK) 
        {
            dialogFastAccess.setContentPane(dialogFastAccessPanel);
            dialogFastAccess.pack();
            dialogFastAccess.setLocationRelativeTo(this);
            dialogFastAccess.setVisible(true);
            return true;
        }

        return false;
    });
    
    // read & set the userdefined dialog configurations
    setMenuDialogConfiguration();
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
    if(splash == null) {
      return;
    }
    
    Graphics2D g = splash.createGraphics();
    if(g == null) {
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
      // NOTE: this call will block until the dialog is closed
      //connectionDialog.setVisible(true);
      this.connectionManager.showConnectionDialog();
      return messageServer.isConnected();
    }
    else
    {
      return true;
    }
  }//end checkConnected
  
  /**
   * Reads all user-defined dialog configurations and creates appropriate menu items.
   */
  private void setMenuDialogConfiguration() {
      String suffix = "_" + userLayoutFile.getName();
      Arrays.asList(userLayoutFile.getParentFile().listFiles((dir, name) -> {
          return name.endsWith(suffix);
      })).forEach((f) -> {
          String name = f.getName().substring(0, f.getName().length()-suffix.length());
          createDialogConfigMenuItem(name);
      });
  }
  
  /**
   * Returns the user dialog configuration file for the given configuration name.
   * 
   * @param configName the name of the dialog configuration
   * @return the File object of this dialog configuration
   */
  private File createUserDialogConfigFile(String configName) {
    return new File(userLayoutFile.getParent() + "/" + configName + "_" + userLayoutFile.getName());
  }

  /**
   * Creates a new dialog configuration menu item and adds it to the restore menu.
   * 
   * @param name the name of the dialog configuration item
   */
  private void createDialogConfigMenuItem(String name) {
        JMenuItem item = new JMenuItem(name);
        item.setToolTipText("<html>Restores this dialog layout.<br>Use <i>Ctrl+Click</i> to delete this dialog layout.</html>");
        item.addActionListener((e) -> {
            JMenuItem source = (JMenuItem) e.getSource();
            if((e.getModifiers() & ActionEvent.CTRL_MASK) == ActionEvent.CTRL_MASK) {
                // delete requested
                if(JOptionPane.showConfirmDialog(this, "Do you want to remove the dialog layout '"+name+"'?", "Remove dialog layout", JOptionPane.YES_NO_OPTION) == JOptionPane.YES_OPTION) {
                    if(createUserDialogConfigFile(name).delete()) {
                        layout.remove(source);
                    }
                }
            } else {
                // restore dialog configuration
                File f = createUserDialogConfigFile(source.getText());
                if(f.isFile()) {
                    try {
                        dialogRegistry.loadFromFile(f);
                    } catch (IOException ex) {
                        Logger.getLogger(RobotControlImpl.class.getName()).log(Level.SEVERE, null, ex);
                    }
                } else {
                    JOptionPane.showMessageDialog(this, "The '"+source.getText()+"' dialog layout file doesn't exists!?", "Missing layout file", JOptionPane.ERROR_MESSAGE);
                }
            }
        });
        layout.add(item);
  }

  /**
   * This method is called from within the constructor to initialize the form.
   * WARNING: Do NOT modify this code. The content of this method is always
   * regenerated by the Form Editor.
   */
  @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {
        java.awt.GridBagConstraints gridBagConstraints;

        dialogFastAccess = new javax.swing.JDialog();
        dialogFastAccessPanel = new de.naoth.rc.DialogFastAccessPanel();
        statusPanel = new javax.swing.JPanel();
        btManager = new javax.swing.JButton();
        lblReceivedBytesS = new javax.swing.JLabel();
        jSeparator4 = new javax.swing.JSeparator();
        lblSentBytesS = new javax.swing.JLabel();
        jSeparator5 = new javax.swing.JSeparator();
        lblFramesS = new javax.swing.JLabel();
        statusPanelPlugins = new javax.swing.JPanel();
        statusPanelSpacer = new javax.swing.Box.Filler(new java.awt.Dimension(0, 0), new java.awt.Dimension(0, 0), new java.awt.Dimension(32767, 0));
        mainMenuBar = new de.naoth.rc.MainMenuBar();
        mainControlMenu = new javax.swing.JMenu();
        connectMenuItem = new javax.swing.JMenuItem();
        disconnectMenuItem = new javax.swing.JMenuItem();
        enforceConnection = new javax.swing.JCheckBoxMenuItem();
        layout = new javax.swing.JMenu();
        resetLayoutMenuItem = new javax.swing.JMenuItem();
        miSaveDialogConfig = new javax.swing.JMenuItem();
        jSeparator2 = new javax.swing.JPopupMenu.Separator();
        preferences = new javax.swing.JMenuItem();
        jSeparator1 = new javax.swing.JSeparator();
        exitMenuItem = new javax.swing.JMenuItem();
        helpMenu = new javax.swing.JMenu();
        helpMenuItem = new javax.swing.JMenuItem();
        aboutMenuItem = new javax.swing.JMenuItem();

        dialogFastAccess.setAlwaysOnTop(true);
        dialogFastAccess.setLocationByPlatform(true);
        dialogFastAccess.setName("dialogFastAccessDialog"); // NOI18N
        dialogFastAccess.setUndecorated(true);
        dialogFastAccess.getContentPane().add(dialogFastAccessPanel, java.awt.BorderLayout.CENTER);

        setDefaultCloseOperation(javax.swing.WindowConstants.EXIT_ON_CLOSE);
        setTitle("RobotControl for NAO v2019");
        addWindowListener(new java.awt.event.WindowAdapter() {
            public void windowClosing(java.awt.event.WindowEvent evt) {
                formWindowClosing(evt);
            }
        });

        statusPanel.setBackground(java.awt.Color.lightGray);
        statusPanel.setPreferredSize(new java.awt.Dimension(966, 25));

        btManager.setText("Running Manager --");
        btManager.setToolTipText("Shows the number of currently registered Manager");
        btManager.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btManagerActionPerformed(evt);
            }
        });

        lblReceivedBytesS.setText("Received (byte/s): ");

        jSeparator4.setOrientation(javax.swing.SwingConstants.VERTICAL);

        lblSentBytesS.setText("Sent (byte/s): ");

        jSeparator5.setOrientation(javax.swing.SwingConstants.VERTICAL);

        lblFramesS.setText("Update Loop (frame/s):");

        statusPanelPlugins.setFocusable(false);
        statusPanelPlugins.setMaximumSize(new java.awt.Dimension(32767, 24));
        statusPanelPlugins.setMinimumSize(new java.awt.Dimension(0, 24));
        statusPanelPlugins.setOpaque(false);
        statusPanelPlugins.setPreferredSize(new java.awt.Dimension(100, 24));
        statusPanelPlugins.setLayout(new java.awt.GridBagLayout());
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.weightx = 1.0;
        statusPanelPlugins.add(statusPanelSpacer, gridBagConstraints);

        javax.swing.GroupLayout statusPanelLayout = new javax.swing.GroupLayout(statusPanel);
        statusPanel.setLayout(statusPanelLayout);
        statusPanelLayout.setHorizontalGroup(
            statusPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(statusPanelLayout.createSequentialGroup()
                .addComponent(btManager, javax.swing.GroupLayout.PREFERRED_SIZE, 121, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(lblReceivedBytesS, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(jSeparator4, javax.swing.GroupLayout.PREFERRED_SIZE, 1, Short.MAX_VALUE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(lblSentBytesS, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(jSeparator5)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(lblFramesS, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addGap(63, 63, 63)
                .addComponent(statusPanelPlugins, javax.swing.GroupLayout.DEFAULT_SIZE, 464, Short.MAX_VALUE))
        );
        statusPanelLayout.setVerticalGroup(
            statusPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jSeparator5, javax.swing.GroupLayout.Alignment.TRAILING)
            .addComponent(lblSentBytesS, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
            .addComponent(lblFramesS, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
            .addGroup(statusPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                .addComponent(btManager, javax.swing.GroupLayout.PREFERRED_SIZE, 25, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addComponent(lblReceivedBytesS, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
            .addGroup(statusPanelLayout.createSequentialGroup()
                .addComponent(statusPanelPlugins, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
            .addGroup(statusPanelLayout.createSequentialGroup()
                .addComponent(jSeparator4, javax.swing.GroupLayout.PREFERRED_SIZE, 24, javax.swing.GroupLayout.PREFERRED_SIZE)
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

        layout.setText("Layout");

        resetLayoutMenuItem.setText("Reset layout");
        resetLayoutMenuItem.setToolTipText("Resets all layout information");
        resetLayoutMenuItem.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                resetLayoutMenuItemActionPerformed(evt);
            }
        });
        layout.add(resetLayoutMenuItem);

        miSaveDialogConfig.setText("Save dialog layout");
        miSaveDialogConfig.setToolTipText("Saves the current active dialog configuration.");
        miSaveDialogConfig.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                miSaveDialogConfigActionPerformed(evt);
            }
        });
        layout.add(miSaveDialogConfig);
        layout.add(jSeparator2);

        mainControlMenu.add(layout);

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

        helpMenuItem.setText("Help");
        helpMenuItem.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                helpMenuItemActionPerformed(evt);
            }
        });
        helpMenu.add(helpMenuItem);

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
        connectionManager.showConnectionDialog();
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

    private void miSaveDialogConfigActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_miSaveDialogConfigActionPerformed
        // Ask for a name for this dialog configuration
        String inputName = JOptionPane.showInputDialog("Set a name for the current dialog layout");
        // ignore canceled inputs
        if(inputName == null) { return; }
        // replace "invalid" characters
        String name = inputName.trim().replaceAll("[^A-Za-z0-9_-]", "");
        // ignore empty inputs
        if(name.isEmpty()) { return; }
        // create layout file
        File f = createUserDialogConfigFile(name);
        // if configuration name already exists - ask to overwrite
        if(f.isFile() && JOptionPane.showConfirmDialog(this, "This dialog layout name already exists. Overwrite?", "Overwrite", JOptionPane.YES_NO_OPTION) != JOptionPane.YES_OPTION) {
            // don't overwrite!
            return;
        }
        // save configuration
        try {
            boolean exists = f.isFile();
            dialogRegistry.saveToFile(f);
            // if this configuration already exists, don't create a new menu entry!
            if(!exists) {
                createDialogConfigMenuItem(name);
            }
        } catch (IOException ex) {
            Logger.getLogger(RobotControlImpl.class.getName()).log(Level.SEVERE, null, ex);
        }
    }//GEN-LAST:event_miSaveDialogConfigActionPerformed

    private void btManagerActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btManagerActionPerformed

        TaskManager taskManager = new TaskManager(this, true);
        String str = "Currently registeres Manager:\n\n";
        for(int i = 0; i < messageServer.getListeners().size(); i++)
        {
            String name = messageServer.getListeners().get(i).getClass().getSimpleName();
            taskManager.addTask(name, "0", null);
            str += messageServer.getListeners().get(i).getClass().getSimpleName() + "\n";
        }//end for
        str += "\n";

        //taskManager.setVisible(true);
        JOptionPane.showMessageDialog(this, str);
    }//GEN-LAST:event_btManagerActionPerformed

    private void helpMenuItemActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_helpMenuItemActionPerformed
        helpDialog.showHelp(null);
    }//GEN-LAST:event_helpMenuItemActionPerformed

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
            // find the parent top directory called 'robotcontrol'
            File parentDir = selfFile.getParentFile();
            while(parentDir != null && !"robotcontrol".equalsIgnoreCase(parentDir.getName())) {
              parentDir = parentDir.getParentFile();
            }
            
            // check for JFX plugins in the directory 'JFXPlugins'
            if(parentDir != null)
            {
              File jfxCandidate = new File(parentDir, "JFXPlugins/dist");
              File[] jarFiles = jfxCandidate.listFiles(new FileFilter()
              {
                @Override
                public boolean accept(File pathname) {
                  return pathname.isFile() && pathname.getName().endsWith(".jar");
                }
              });
              
              if(jarFiles != null)
              {
                for (File j : jarFiles) {
                  pluginManager.addPluginsFrom(j.toURI());
                }
              }
            }
          }
          
          
          // relative "plugins/" directory
          File workingDirectoryPlugin = new File("plugins/");
          if(workingDirectoryPlugin.isDirectory()) {
            pluginManager.addPluginsFrom(workingDirectoryPlugin.toURI());
          }

          //
          File userHomePlugin = new File(System.getProperty("user.home")
            + "/.naoth/robotcontrol/plugins/");
          if(userHomePlugin.isDirectory()){
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
    private javax.swing.JDialog dialogFastAccess;
    private de.naoth.rc.DialogFastAccessPanel dialogFastAccessPanel;
    private javax.swing.JMenuItem disconnectMenuItem;
    private javax.swing.JCheckBoxMenuItem enforceConnection;
    private javax.swing.JMenuItem exitMenuItem;
    private javax.swing.JMenu helpMenu;
    private javax.swing.JMenuItem helpMenuItem;
    private javax.swing.JSeparator jSeparator1;
    private javax.swing.JPopupMenu.Separator jSeparator2;
    private javax.swing.JSeparator jSeparator4;
    private javax.swing.JSeparator jSeparator5;
    private javax.swing.JMenu layout;
    private javax.swing.JLabel lblFramesS;
    private javax.swing.JLabel lblReceivedBytesS;
    private javax.swing.JLabel lblSentBytesS;
    private javax.swing.JMenu mainControlMenu;
    private de.naoth.rc.MainMenuBar mainMenuBar;
    private javax.swing.JMenuItem miSaveDialogConfig;
    private javax.swing.JMenuItem preferences;
    private javax.swing.JMenuItem resetLayoutMenuItem;
    private javax.swing.JPanel statusPanel;
    private javax.swing.JPanel statusPanelPlugins;
    private javax.swing.Box.Filler statusPanelSpacer;
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
  
  @Override
  final public String getConfigPath()
  {
      return USER_CONFIG_DIR;
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
      // save layout
      this.dialogRegistry.saveToFile(userLayoutFile);
      // notify all dialogs, so they have the chance to clean up
      this.dialogRegistry.disposeOnClose();
     
      // save configuration to file
      new File(USER_CONFIG_DIR).mkdirs();
      getConfig().store(new FileWriter(userConfigFile), "");
    }
    catch(IOException ex)
    {
      Helper.handleException(ex);
    }
    // make sure the JavaFX thread is closed (otherwise exceptions get sometimes thrown)
    javafx.application.Platform.exit();
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
    String unit = "byte";
    if(rate > 1024*1024) {
        rate /= 1024*1024;
        unit = "MB";
    } else if (rate > 1024) {
        rate /= 1024;
        unit = "KB";
    }
    lblReceivedBytesS.setText(String.format("Received (%s/s): %4.2f", unit, rate));
  }

  @Override
  public void setSentByteRate(double rate)
  {
    String unit = "byte";
    if(rate > 1024*1024) {
        rate /= 1024*1024;
        unit = "MB";
    } else if (rate > 1024) {
        rate /= 1024;
        unit = "KB";
    }
    lblSentBytesS.setText(String.format("Sent (%s/s): %4.2f", unit, rate));
  }
  
  @Override
  public void setServerLoopFPS(double fps)
  {
    lblFramesS.setText(String.format("Update Loop (frame/s): %4.2f", fps));
  }
  
  @Override
  public int getFontSize() 
  {
      int size = UIManager.getDefaults().getFont("defaultFont") != null ? UIManager.getDefaults().getFont("defaultFont").getSize() : 12;
      return Integer.parseInt(this.getConfig().getProperty("fontSize", String.valueOf(size)));
  }

  @Override
  public String getTheme() {
      URL f = getClass().getResource("/de/naoth/rc/res/themes/" + config.getProperty("theme", "") + ".css");
      if (f != null) {
          return f.toExternalForm();
      }
      return null;
  }
  
  private ArrayList<Component> statusPanelComponents = new ArrayList<>();
  @Override
  public void addToStatusBar(Component c) {
      // add to "weighted" list
      statusPanelComponents.add(c);
      // sort (modified) list
      statusPanelComponents.sort((l1,l2)->{
          if(l1 instanceof StatusbarPluginImpl && l2 instanceof StatusbarPluginImpl) {
              return ((StatusbarPluginImpl)l1).getWeight() - ((StatusbarPluginImpl)l2).getWeight();
          }
          return 0;
      });
      // remove all statusbar components, to re-add
      statusPanelComponents.stream().forEachOrdered((t) -> {
          this.statusPanelPlugins.remove(t);
      });
      // iterate through sorted list and add to statusbar
      statusPanelComponents.stream().forEachOrdered((t) -> {
          this.statusPanelPlugins.add(t, statusPanelPluginsConstraints);
      });
  }
}
