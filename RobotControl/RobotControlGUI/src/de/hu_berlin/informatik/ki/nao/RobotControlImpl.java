/*
 * RobotControlGUI.java
 *
 * Created on 08.10.2010, 16:31:30
 */
package de.hu_berlin.informatik.ki.nao;

import bibliothek.extension.gui.dock.theme.EclipseTheme;
import bibliothek.gui.DockFrontend;
import bibliothek.gui.Dockable;
import bibliothek.gui.dock.SplitDockStation;
import bibliothek.gui.dock.frontend.MissingDockableStrategy;
import com.jgoodies.looks.plastic.PlasticXPLookAndFeel;
import de.hu_berlin.informatik.ki.nao.interfaces.ByteRateUpdateHandler;
import de.hu_berlin.informatik.ki.nao.server.ConnectionDialog;
import de.hu_berlin.informatik.ki.nao.server.IMessageServerParent;
import de.hu_berlin.informatik.ki.nao.server.MessageServer;
import java.awt.BorderLayout;
import java.awt.Frame;
import java.awt.Image;
import java.awt.Point;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.net.URI;
import java.net.URISyntaxException;
import java.util.HashSet;
import java.util.List;
import java.util.Properties;
import java.util.Set;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.AbstractAction;
import javax.swing.JComponent;
import javax.swing.JOptionPane;
import javax.swing.KeyStroke;
import javax.swing.UIManager;
import net.xeoh.plugins.base.PluginManager;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.events.PluginLoaded;
import net.xeoh.plugins.base.impl.PluginManagerFactory;
import org.apache.commons.lang.StringUtils;

/**
 *
 * @author thomas
 */
@PluginImplementation
public class RobotControlImpl extends javax.swing.JFrame
  implements ByteRateUpdateHandler,
  IMessageServerParent, RobotControl
{

  private final String configlocation = System.getProperty("user.home")
    + "/.naoth/robotcontrol/";
  private File layoutFile =
    new File(configlocation + "layout.dat");

  private DockFrontend frontend;
  private DialogRegistry dialogRegistry;

  private MessageServer messageServer;
  
  // Propertes
  private File fConfig;
  private Properties config;
  private ConnectionDialog connectionDialog;
  private boolean doNotSaveLayoutOnClose = false;

  /** Creates new form RobotControlGUI */
  public RobotControlImpl()
  {
    
    try
    {
      // set Look and Feel before adding all the components
      if("Linux".equals(System.getProperty("os.name")))
      {
        UIManager.setLookAndFeel(new PlasticXPLookAndFeel());
      }
      else
      {
        UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
      }
      //UIManager.setLookAndFeel(new GTKLookAndFeel());
      
    }
    catch (Exception ex)
    {
      Logger.getLogger(RobotControlImpl.class.getName()).log(Level.SEVERE, null, ex);
    }
    initComponents();

    messageServer = new MessageServer(this);
    frontend = new DockFrontend(this);
    frontend.getController().setTheme(new EclipseTheme());
    SplitDockStation station = new SplitDockStation();
    frontend.addRoot("split", station);
    frontend.setShowHideAction(true);
    frontend.setMissingDockableStrategy(MissingDockableStrategy.DISCARD_ALL);
    
    this.connectionDialog = new ConnectionDialog(this, this);
    dialogRegistry = new DialogRegistry(dialogsMenu, frontend, station);

    // icon
    Image icon = Toolkit.getDefaultToolkit().getImage(
      this.getClass().getResource("res/RobotControlLogo128.png"));
    setIconImage(icon);

    this.getContentPane().add(station, BorderLayout.CENTER);

    readLayoutFromFile();

    disconnectMenuItem.setEnabled(false);
  }
  
  @PluginLoaded
  public void registerDialog(final Dialog dialog)
  {
    dialogRegistry.registerDialog(dialog);
    // load dialog if it was open in the last session
    String openDialogsString = getConfig().getProperty("dialogs");
    if(openDialogsString != null)
    {
      String[] splitted = openDialogsString.split(",");
      for(String s : splitted)
      {
        if(s.trim().equals(dialog.getDisplayName()))
        {
          dialogRegistry.dockDialog(dialog);
          loadLayout();
          break;
        }
      }
    }
  }

  @Override
  public boolean checkConnected()
  {
    if(messageServer.isConnected())
    {
      return true;
    }

    // show a warning: we are not connected
    /*
    JOptionPane.showMessageDialog(this,
        "Not connected. Please connect first to a robot.", "WARNING",
        JOptionPane.WARNING_MESSAGE);
    */
    
    // show connection dialog
    connectionDialog.setVisible(true);

    return messageServer.isConnected();
  }//end checkConnected

  
  private void attachHelpDialog(JComponent component, ShowHelpAction action)
  {
    KeyStroke strokeH = KeyStroke.getKeyStroke("control pressed H");
    KeyStroke strokeF1 = KeyStroke.getKeyStroke("pressed F1");

    component.registerKeyboardAction(action, strokeH, JComponent.WHEN_ANCESTOR_OF_FOCUSED_COMPONENT);
    component.registerKeyboardAction(action, strokeF1, JComponent.WHEN_ANCESTOR_OF_FOCUSED_COMPONENT);
  }//end attachHelpDialog

  /** This method is called from within the constructor to
   * initialize the form.
   * WARNING: Do NOT modify this code. The content of this method is
   * always regenerated by the Form Editor.
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
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 381, Short.MAX_VALUE)
        .addComponent(lblConnect)
        .addContainerGap())
    );
    statusPanelLayout.setVerticalGroup(
      statusPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, statusPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
        .addComponent(btManager, javax.swing.GroupLayout.PREFERRED_SIZE, 25, javax.swing.GroupLayout.PREFERRED_SIZE)
        .addComponent(lblConnect, javax.swing.GroupLayout.DEFAULT_SIZE, 23, Short.MAX_VALUE)
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

    java.awt.Dimension screenSize = java.awt.Toolkit.getDefaultToolkit().getScreenSize();
    setBounds((screenSize.width-974)/2, (screenSize.height-626)/2, 974, 626);
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
      Point location = this.getLocation();
      location.translate(100, 100);
      dlg.setLocation(location);
      dlg.setVisible(true);

    }//GEN-LAST:event_aboutMenuItemActionPerformed

    private void btManagerActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_btManagerActionPerformed
    {//GEN-HEADEREND:event_btManagerActionPerformed

      TaskManager taskManager = new TaskManager(this, true);
      //String str = "Currently registeres Manager:\n\n";
      for (int i = 0; i < messageServer.getListeners().size(); i++)
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

      doNotSaveLayoutOnClose = true;
      if(layoutFile.exists() && layoutFile.isFile() && layoutFile.canWrite())
      {
        layoutFile.delete();
        JOptionPane.showMessageDialog(null, "You need to restart RobotControl now.");
      }//end if
      
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
        PluginManager pluginManager = PluginManagerFactory.createPluginManager();
        try
        {
          pluginManager.addPluginsFrom(new URI("classpath://*"));
          File workingDirectoryPlugin = new File("plugins/");
          if(workingDirectoryPlugin.isDirectory())
          {
            pluginManager.addPluginsFrom(workingDirectoryPlugin.toURI());
          }
          
          File userHomePlugin = new File(System.getProperty("user.home")
            + "/.naoth/robotcontrol/plugins/");
          if(userHomePlugin.isDirectory())
          {
            pluginManager.addPluginsFrom(userHomePlugin.toURI());
          }
          pluginManager.getPlugin(RobotControl.class).setVisible(true);
        }
        catch (URISyntaxException ex)
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
  public void showConnected(boolean isConnected)
  {
    disconnectMenuItem.setEnabled(isConnected);
    connectMenuItem.setEnabled(!isConnected);

    if (isConnected)
    {
      lblConnect.setText("Connected to " + messageServer.getAddress().toString());
    }
    else
    {
      lblConnect.setText("Not connected");
    }
  }

  @Override
  public Properties getConfig()
  {
    if (fConfig == null || config == null)
    {
      fConfig = new File(configlocation + "config");
      config = new Properties();
      try
      {
        config.load(new FileReader(fConfig));
      }
      catch (Exception ex)
      {
        ex.printStackTrace();
        JOptionPane.showMessageDialog(this,
          "Config could not be loaded from "+fConfig, "WARNING",
          JOptionPane.WARNING_MESSAGE);
      }
    }//end if

    return config;
  }

  private void beforeClose()
  {
    messageServer.disconnect();

    // remember open dialogs
    List<Dockable> dockables =  frontend.listDockables();
    Set<String> dockablesAsstring = new HashSet<String>();
    for(Dockable d : dockables)
    {
      dockablesAsstring.add(d.getTitleText());
    }
    getConfig().put("dialogs", StringUtils.join(dockablesAsstring, ","));
    // save configuration to file
    try
    {
      new File(configlocation).mkdirs();
      getConfig().store(new FileWriter(fConfig), "");

      // save layout
      if(!doNotSaveLayoutOnClose)
      {
        frontend.save("naoth-robotcontrol-default");
        frontend.write(new DataOutputStream(new FileOutputStream(layoutFile)));
      }
    }
    catch (IOException ex)
    {
      Helper.handleException(ex);
    }
  }

  private void readLayoutFromFile()
  {
    try
    {
      frontend.read(new DataInputStream(new FileInputStream(layoutFile)));
    }
    catch (Exception ex)
    {
    }
  }
  private void loadLayout()
  {
    try
    { 
      frontend.load("naoth-robotcontrol-default");
    }
    catch(Exception ex)
    {
      //ex.printStackTrace();
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


  class ShowHelpAction extends AbstractAction
  {
    private String title = null;
    private Frame parent = null;
    private String text = null;
    private HelpDialog dlg = null;

    public ShowHelpAction(Frame parent, String title)
    {
      this.title = title;
      this.parent = parent;

      this.text = Helper.getResourceAsString("/de/hu_berlin/informatik/ki/nao/dialogs/"+title+".html");
      if(this.text == null)
        this.text = "For this dialog is no help avaliable.";

      this.dlg = new HelpDialog(parent, true, text);
      this.dlg.setDefaultCloseOperation(javax.swing.WindowConstants.HIDE_ON_CLOSE);
      this.dlg.setTitle(title);
      this.dlg.setModal(false);
      //this.dlg.setAlwaysOnTop(true);
      this.dlg.setVisible(false);
    }//end ShowHelpAction

    @Override
    public void actionPerformed(ActionEvent e) {
      // move only if the dialog is invisible
      if(!this.dlg.isVisible())
      {
        Point location = parent.getLocation();
        location.translate(100, 100);
        this.dlg.setLocation(location);
      }//end if

      this.dlg.setVisible(true);
    }//end actionPerformed
  }//end class ShowHelpAction

}
