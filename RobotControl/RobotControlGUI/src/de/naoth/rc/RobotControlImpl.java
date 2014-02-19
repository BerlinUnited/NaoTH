/*
 * RobotControlGUI.java
 *
 * Created on 08.10.2010, 16:31:30
 */
package de.naoth.rc;

import bibliothek.gui.DockUI;
import bibliothek.gui.dock.util.laf.Nimbus6u10;
import de.naoth.rc.interfaces.ByteRateUpdateHandler;
import de.naoth.rc.server.ConnectionDialog;
import de.naoth.rc.server.ConnectionStatusEvent;
import de.naoth.rc.server.ConnectionStatusListener;
import de.naoth.rc.server.MessageServer;
import java.awt.*;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.io.*;
import java.net.URI;
import java.net.URISyntaxException;
import java.util.Properties;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;
import javax.swing.plaf.nimbus.NimbusLookAndFeel;
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

  private static final String configlocation = System.getProperty("user.home")
    + "/.naoth/robotcontrol/";
  private final File layoutFile = new File(configlocation, "layout.dat");
  private final File fConfig = new File(configlocation, "config");
  
  private final MessageServer messageServer;
  
  private final Properties config = new Properties();
  private final ConnectionDialog connectionDialog;
  
  private final DialogRegistry dialogRegistry;

  // remember the window position and size to restore it later
  private Rectangle defaultWindowBounds = new Rectangle();
  
  /**
   * Creates new form RobotControlGUI
   */
  public RobotControlImpl()
  {
    splashScreenMessage("Welcome to RobotControl");
    try
    {
      //UIManager.setLookAndFeel(new PlasticXPLookAndFeel());
      UIManager.setLookAndFeel(new NimbusLookAndFeel());
      // set explicitely the Nimbus colors to be used
      DockUI.getDefaultDockUI().registerColors("javax.swing.plaf.nimbus.NimbusLookAndFeel", new Nimbus6u10());
    }
    catch(UnsupportedLookAndFeelException ex)
    {
      Logger.getLogger(RobotControlImpl.class.getName()).log(Level.SEVERE, null, ex);
    }
    
    // icon
    Image icon = Toolkit.getDefaultToolkit().getImage(
      this.getClass().getResource("res/RobotControlLogo128.png"));
    setIconImage(icon);

    initComponents();

    // load the configuration
    readConfigFromFile();
    
    // restore the bounds and the state of the frame from the config
    try {
        defaultWindowBounds.x = Integer.parseInt(getConfig().getProperty("frame.position.x"));
        defaultWindowBounds.y = Integer.parseInt(getConfig().getProperty("frame.position.y"));
        defaultWindowBounds.width = Integer.parseInt(getConfig().getProperty("frame.width"));
        defaultWindowBounds.height = Integer.parseInt(getConfig().getProperty("frame.height"));
        int extendedstate = Integer.parseInt(getConfig().getProperty("frame.extendedstate"));
        setBounds(defaultWindowBounds);
        setExtendedState(extendedstate);
    } catch (NumberFormatException ex) {
        // no info necessary
    }
    
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
    this.dialogRegistry = new DialogRegistry(this, dialogsMenu);

    
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
  }//end constructor

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
    if(messageServer.isConnected())
    {
      return true;
    }

    // show connection dialog
    connectionDialog.setVisible(true);

    return messageServer.isConnected();
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
        menuBar = new javax.swing.JMenuBar();
        mainControlMenu = new javax.swing.JMenu();
        connectMenuItem = new javax.swing.JMenuItem();
        disconnectMenuItem = new javax.swing.JMenuItem();
        resetLayoutMenuItem = new javax.swing.JMenuItem();
        jSeparator1 = new javax.swing.JSeparator();
        exitMenuItem = new javax.swing.JMenuItem();
        dialogsMenu = new javax.swing.JMenu();
        helpMenu = new javax.swing.JMenu();
        aboutMenuItem = new javax.swing.JMenuItem();

        setDefaultCloseOperation(javax.swing.WindowConstants.EXIT_ON_CLOSE);
        setTitle("RobotControl for Nao");
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
        btManager.setBorder(null);
        btManager.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btManagerActionPerformed(evt);
            }
        });

        lblReceivedBytesS.setText("Recived byte/s: ");

        lblSentBytesS.setText("Sent byte/s: ");

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
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 410, Short.MAX_VALUE)
                .addComponent(lblConnect)
                .addContainerGap())
        );
        statusPanelLayout.setVerticalGroup(
            statusPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, statusPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                .addComponent(btManager, javax.swing.GroupLayout.PREFERRED_SIZE, 25, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addComponent(lblConnect, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addComponent(lblReceivedBytesS)
                .addComponent(lblSentBytesS))
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

        resetLayoutMenuItem.setText("Reset layout");
        resetLayoutMenuItem.setToolTipText("\"Resets all layout information");
        resetLayoutMenuItem.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                resetLayoutMenuItemActionPerformed(evt);
            }
        });
        mainControlMenu.add(resetLayoutMenuItem);
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

        menuBar.add(mainControlMenu);

        dialogsMenu.setMnemonic('d');
        dialogsMenu.setText("Dialogs");
        menuBar.add(dialogsMenu);

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

        menuBar.add(helpMenu);

        setJMenuBar(menuBar);

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

  @Override
  public MessageServer getMessageServer()
  {
    return messageServer;
  }

  /**
   * @param args the command line arguments
   */
  public static void main(String args[])
  {
    java.awt.EventQueue.invokeLater(new Runnable()
    {

      @Override
      public void run()
      {
        final JSPFProperties props = new JSPFProperties();
        props.setProperty(PluginManager.class, "cache.enabled", "true");
        props.setProperty(PluginManager.class, "cache.mode", "stong"); //optional
        props.setProperty(PluginManager.class, "cache.file", configlocation+"robot-control.jspf.cache");

        PluginManager pluginManager = PluginManagerFactory.createPluginManager(props);

        try
        {
          // make sure the main frame if loaded first
          pluginManager.addPluginsFrom(new ClassURI(RobotControlImpl.class).toURI());
          
          //
          pluginManager.addPluginsFrom(new URI("classpath://*"));

          //
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
          Logger.getLogger(RobotControlImpl.class.getName()).log(Level.SEVERE, null, ex);
        }
      }
    });
  }
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JMenuItem aboutMenuItem;
    private javax.swing.JButton btManager;
    private javax.swing.JMenuItem connectMenuItem;
    private javax.swing.JMenu dialogsMenu;
    private javax.swing.JMenuItem disconnectMenuItem;
    private javax.swing.JMenuItem exitMenuItem;
    private javax.swing.JMenu helpMenu;
    private javax.swing.JSeparator jSeparator1;
    private javax.swing.JLabel lblConnect;
    private javax.swing.JLabel lblReceivedBytesS;
    private javax.swing.JLabel lblSentBytesS;
    private javax.swing.JMenu mainControlMenu;
    private javax.swing.JMenuBar menuBar;
    private javax.swing.JMenuItem resetLayoutMenuItem;
    private javax.swing.JPanel statusPanel;
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
      new File(configlocation).mkdirs();
      getConfig().store(new FileWriter(fConfig), "");

      // save layout
     this.dialogRegistry.saveToFile(layoutFile);
    }
    catch(IOException ex)
    {
      Helper.handleException(ex);
    }
  }

  private void readConfigFromFile()
  {
    try {
      config.load(new FileReader(fConfig));
    } catch(IOException ex) {
      Logger.getLogger(RobotControlImpl.class.getName()).log(Level.INFO, 
              "Could not open the config file. It will be created after the first execution.", ex);
    }
  }

  private void loadLayout()
  {
    try {
      this.dialogRegistry.loadFromFile(layoutFile);
    } catch(FileNotFoundException ex) {
      Logger.getLogger(RobotControlImpl.class.getName()).log(Level.INFO, 
              "Could not find the layout file: " + layoutFile.getAbsolutePath());
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
}
