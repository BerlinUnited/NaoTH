/*
 * Main.java
 *
 * Created on 28. Februar 2008, 11:01
 */
package de.hu_berlin.informatik.ki.nao;

import de.hu_berlin.informatik.ki.nao.dialogs.Console;
import de.hu_berlin.informatik.ki.nao.dialogs.DebugRequestPanel;
import de.hu_berlin.informatik.ki.nao.manager.GenericManager;
import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.event.ActionEvent;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.JPanel;
import javax.swing.UIManager;
import javax.swing.border.LineBorder;
import org.flexdock.docking.Dockable;
import org.flexdock.docking.DockingConstants;
import org.flexdock.docking.DockingManager;
import org.flexdock.view.View;
import org.flexdock.view.Viewport;

import de.hu_berlin.informatik.ki.nao.manager.UnrequestedOutputManager;
import de.hu_berlin.informatik.ki.nao.server.Command;
import de.hu_berlin.informatik.ki.nao.server.ConnectionDialog;
import de.hu_berlin.informatik.ki.nao.server.IMessageServerParent;
import de.hu_berlin.informatik.ki.nao.server.MessageServer;
import java.awt.Frame;
import java.awt.Image;
import java.awt.Point;
import java.awt.Toolkit;
import java.awt.event.ActionListener;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.net.URI;
import java.net.URISyntaxException;
import java.net.URL;
import java.security.CodeSource;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Properties;
import java.util.Set;
import javax.swing.AbstractAction;
import javax.swing.Action;
import javax.swing.JComponent;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.KeyStroke;
import net.xeoh.plugins.base.PluginManager;
import net.xeoh.plugins.base.impl.PluginManagerFactory;
import net.xeoh.plugins.base.util.PluginManagerUtil;
import net.xeoh.plugins.base.util.uri.ClassURI;
import org.flexdock.docking.drag.effects.EffectsManager;
import org.flexdock.docking.drag.preview.GhostPreview;
import org.flexdock.docking.state.PersistenceException;
import org.flexdock.perspective.PerspectiveManager;
import org.flexdock.perspective.persist.FilePersistenceHandler;
import org.flexdock.perspective.persist.PersistenceHandler;
import org.flexdock.perspective.persist.xml.XMLPersister;
import org.flexdock.view.actions.DefaultCloseAction;

/**
 *
 * @author  thomas
 */
public class Main extends javax.swing.JFrame implements IMessageServerParent
{
  private PluginManager pluginManager;
  private DialogManager dialogManager;

  // manager
  private MessageServer messageServer;
  private UnrequestedOutputManager unrequestedOutputManager;

  private HashMap<Command, GenericManager> mapOfGenericManager;
  
  // dialogs
  private LinkedList<View> lastViews;
  private HashMap<String, Integer> dialogCounter;
  private View drcView;
  
  private ConnectionDialog connectionDialog;

  // there are always some special ones...
  private Dialog debugRequestCenter;

  // Propertes
  private File fConfig;
  private Properties config;

  private File layoutFile =
    new File(System.getProperty("user.home") + "/.RobotControlyLayout.xml");

 
  private File mainDirectory;

  // show the connection dialog if not connected
  private boolean triggerConnect = true;

  /** Creates new form Main */
  public Main()
  {
    initComponents();
   
    pluginManager = PluginManagerFactory.createPluginManager();
    try
    {
      pluginManager.addPluginsFrom(new URI("classpath://*"));
    }
    catch (URISyntaxException ex)
    {
      Logger.getLogger(Main.class.getName()).log(Level.SEVERE, null, ex);
    }

    dialogManager = pluginManager.getPlugin(DialogManager.class);


    CodeSource source = Main.class.getProtectionDomain().getCodeSource();
    if(source != null)
    {
      URL url = source.getLocation();
      try
      {
        mainDirectory = new File(url.toURI()).getParentFile().getParentFile();
      } catch (URISyntaxException ex)
      {
        Logger.getLogger(Main.class.getName()).log(Level.SEVERE, null, ex);
        // fallback to working directory
        mainDirectory = new File(".");
      }
    }

    // icon
    Image icon = Toolkit.getDefaultToolkit().getImage(
      this.getClass().getResource("res/RobotControlLogo128.png"));
    setIconImage(icon);

    // managers and server
    messageServer = new MessageServer(this);
    unrequestedOutputManager = new UnrequestedOutputManager(messageServer);

    mapOfGenericManager = new HashMap<Command, GenericManager>();

    connectionDialog = new ConnectionDialog(this, this);

    setupDefaultLayout();
    configureDocking();

    showConnected(false);


    // HACK: implement MagagerChangedEvent concept
    Thread t = new Thread(new Runnable() {
      public void run()
      {
        while(true)
        {
          String str = "Running Manager " +messageServer.getListeners().size();

          btManager.setText(str);
          try
          {
              Thread.sleep(100);
          }catch(InterruptedException e)
          {
              System.out.println(e);
          }//end catch
        }//end while
      }//end run
    });

    Thread t2 = new Thread(new Runnable() {
      long oldRecivedSize = 0;
      long oldSentSize = 0;

      public void run()
      {
        while(true)
        {
          double recivedKB = ((double)(messageServer.getReceivedBytes()-oldRecivedSize))/1024.0;
          double sentKB = ((double)(messageServer.getSentBytes()-oldSentSize))/1024.0;

          String recivedStr = String.format("Recived KB/s: %4.2f", recivedKB);
          String sentStr = String.format("Sent KB/s: %4.2f", sentKB);
          oldRecivedSize = messageServer.getReceivedBytes();
          oldSentSize = messageServer.getSentBytes();

          lblRecivedBytesS.setText(recivedStr);
          lblSentBytesS.setText(sentStr);

          try
          {
              Thread.sleep(1000);
          }catch(InterruptedException e)
          {
              System.out.println(e);
          }//end catch
        }//end while
      }//end run
    });
    
    t.start();
    t2.start();
  }//end constructor Main



  private void setupDefaultLayout()
  {
    Viewport port = new Viewport();

    this.getContentPane().add(port, BorderLayout.CENTER);

    lastViews = new LinkedList<View>();

    // add debug request center
    debugRequestCenter = new DebugRequestPanel();
    debugRequestCenter.init(this);
    drcView =
      createView("debugRequestCenter", "Debug Request Center", debugRequestCenter.getPanel());

    // attach the same action to several panels to avoid producing several
    // help dialogs
    ShowHelpAction helpAction = new ShowHelpAction(this, debugRequestCenter.getClass().getSimpleName());
    attachHelpDialog(debugRequestCenter.getPanel(), helpAction);
    attachHelpDialog(drcView,                       helpAction);

    //drcView.addAction(DockingConstants.PIN_ACTION);
    port.dock((Dockable) drcView, Viewport.EAST_REGION);
    
    // loading all dialogs
    dialogCounter = new HashMap<String, Integer>();

    LinkedList<JMenuItem> dialogMenuEntries = new LinkedList<JMenuItem>();

    for(final String caption : dialogManager.getDialogs().keySet())
    {
      final JMenuItem newItem = new JMenuItem(caption + " (0)");

      newItem.addActionListener(new ActionListener()
      {

        public void actionPerformed(ActionEvent e)
        {
          loadDynamicDialog(dialogManager.getDialogs().get(caption), caption, newItem);
        }
      });
      dialogMenuEntries.add(newItem);
    }

    Collections.sort(dialogMenuEntries, new Comparator<JMenuItem>()
    {
      public int compare(JMenuItem o1, JMenuItem o2)
      {
        return o1.getText().compareTo(o2.getText());
      }
    });

    for(JMenuItem item : dialogMenuEntries)
    {
      dialogsMenu.add(item);
    }

//      String openDialogsString = getConfig().getProperty("dialogs");
//      if(openDialogsString != null)
//      {
//        String[] splitted = openDialogsString.split(",");
//        for(String s : splitted)
//        {
//          loadDynamicDialog(s, propDialogs.getProperty(s), classNameToItem.get(s));
//        }
//      }


  }//end setupDefaultLayout



  private void configureDocking()
  {
    DockingManager.setFloatingEnabled(true);
    EffectsManager.setPreview(new GhostPreview());

    PerspectiveManager.setRestoreFloatingOnLoad(true);
    //PerspectiveManager mgr = PerspectiveManager.getInstance();
    //System.out.println(FilePersistenceHandler.DEFAULT_PERSPECTIVE_DIR);
    PersistenceHandler persister =
      new FilePersistenceHandler(layoutFile, XMLPersister.newDefaultInstance());
    PerspectiveManager.setPersistenceHandler(persister);

    try
    {
      DockingManager.loadLayoutModel();
      DockingManager.restoreLayout(true);
    }
    catch(IOException ex)
    {
      Helper.handleException(ex);
    }
    catch(PersistenceException ex)
    {
      Helper.handleException(ex);
    }
    DockingManager.setAutoPersist(true);
  }//end configureDocking



  /** 
   * Returns the configuration wich will be saved to a file if the application
   is closed
   */
  public Properties getConfig()
  {
    if(fConfig == null || config == null)
    {
      fConfig = new File(System.getProperty("user.home") + "/.robotcontrol");
      config = new Properties();
      try
      {
        config.load(new FileReader(fConfig));
      }
      catch(IOException ex)
      {
        // ignore
      }
    }//end if

    return config;
  }//end getConfig

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
  }
  
  private void loadDynamicDialog(
          final Dialog dialog,
          final String caption,
          final JMenuItem menuItem)
  {
    if(dialog != null && caption != null && !caption.equals(""))
    {
      try
      {
        final String className = dialog.getClass().getSimpleName();
        // update counter
        int oldVal = dialogCounter.containsKey(className) ?
          dialogCounter.get(className) : 0;
        oldVal++;
        dialogCounter.put(className,new Integer(oldVal));
        menuItem.setText(caption + " (" + oldVal + ")");
        
        dialog.init(this);

        String tabCaption = caption;
        if(oldVal > 1)
        {
          tabCaption = caption + " (" + oldVal + ")";
        }
        final View tmpView = createView(className + "_" + oldVal, tabCaption , dialog.getPanel());

        String region = Viewport.CENTER_REGION;

        if(lastViews.size() == 0)
        {
          region = Viewport.WEST_REGION;
          drcView.dock((Dockable) tmpView, region, 0.83f);
        }
        else
        {
          lastViews.getLast().dock((Dockable) tmpView, region);
        }

        ShowHelpAction helpAction = new ShowHelpAction(this, dialog.getClass().getSimpleName());
        attachHelpDialog(tmpView,           helpAction);
        attachHelpDialog(dialog.getPanel(), helpAction);

        
        DefaultCloseAction closeAction = new DefaultCloseAction()
        {

          @Override
          public void actionPerformed(ActionEvent arg0)
          {
            super.actionPerformed(arg0);
            lastViews.remove(tmpView);

            dialog.dispose();
                    
            int oldVal2 = dialogCounter.containsKey(className) ? 
            dialogCounter.get(className) : 1;
            oldVal2--;
            dialogCounter.put(className,new Integer(oldVal2));
            menuItem.setText(caption + " (" + oldVal2 + ")");
          }

        };
        closeAction.putValue(Action.NAME, DockingConstants.CLOSE_ACTION);
        tmpView.addAction(closeAction);
        tmpView.addAction(DockingConstants.PIN_ACTION);

        lastViews.add(tmpView);
        
        
      }
      catch(Exception ex)
      {
        // ups...
        Helper.handleException(ex);
      }
    }
  }

  private View createView(String id, String text, JPanel panel)
  {
    final View result = new View(id, text);
    panel.setBorder(new LineBorder(Color.GRAY, 1));
    result.setContentPane(panel);

    return result;
  }


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
  // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
  private void initComponents() {

    statusPanel = new javax.swing.JPanel();
    lblConnect = new javax.swing.JLabel();
    btManager = new javax.swing.JButton();
    lblRecivedBytesS = new javax.swing.JLabel();
    lblSentBytesS = new javax.swing.JLabel();
    menuBar = new javax.swing.JMenuBar();
    mainControlMenu = new javax.swing.JMenu();
    connectMenuItem = new javax.swing.JMenuItem();
    disconnectMenuItem = new javax.swing.JMenuItem();
    jSeparator1 = new javax.swing.JSeparator();
    resetLayoutMenuItem = new javax.swing.JMenuItem();
    exitMenuItem = new javax.swing.JMenuItem();
    dialogsMenu = new javax.swing.JMenu();
    helpMenu = new javax.swing.JMenu();
    aboutMenuItem = new javax.swing.JMenuItem();

    setDefaultCloseOperation(javax.swing.WindowConstants.EXIT_ON_CLOSE);
    setTitle("RobotControl for Nao");
    setName("Robot Control"); // NOI18N
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

    lblRecivedBytesS.setText("Recived byte/s: ");

    lblSentBytesS.setText("Sent byte/s: ");

    javax.swing.GroupLayout statusPanelLayout = new javax.swing.GroupLayout(statusPanel);
    statusPanel.setLayout(statusPanelLayout);
    statusPanelLayout.setHorizontalGroup(
      statusPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(statusPanelLayout.createSequentialGroup()
        .addComponent(btManager, javax.swing.GroupLayout.PREFERRED_SIZE, 121, javax.swing.GroupLayout.PREFERRED_SIZE)
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
        .addComponent(lblRecivedBytesS, javax.swing.GroupLayout.PREFERRED_SIZE, 173, javax.swing.GroupLayout.PREFERRED_SIZE)
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
        .addComponent(lblSentBytesS, javax.swing.GroupLayout.PREFERRED_SIZE, 164, javax.swing.GroupLayout.PREFERRED_SIZE)
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 399, Short.MAX_VALUE)
        .addComponent(lblConnect)
        .addContainerGap())
    );
    statusPanelLayout.setVerticalGroup(
      statusPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, statusPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
        .addComponent(btManager, javax.swing.GroupLayout.PREFERRED_SIZE, 25, javax.swing.GroupLayout.PREFERRED_SIZE)
        .addComponent(lblConnect, javax.swing.GroupLayout.DEFAULT_SIZE, 23, Short.MAX_VALUE)
        .addComponent(lblRecivedBytesS)
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
    mainControlMenu.add(jSeparator1);

    resetLayoutMenuItem.setMnemonic('l');
    resetLayoutMenuItem.setText("Reset Layout");
    resetLayoutMenuItem.setToolTipText("Resets all layout information and closes all dialogs");
    resetLayoutMenuItem.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        resetLayoutMenuItemActionPerformed(evt);
      }
    });
    mainControlMenu.add(resetLayoutMenuItem);

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

  public MessageServer getMessageServer()
  {
    return messageServer;
  }

  public UnrequestedOutputManager getUnrequestedOutputManager()
  {
    return unrequestedOutputManager;
  }

  public GenericManager getGenericManager(Command command)
  {
    if(mapOfGenericManager.get(command) == null)
      mapOfGenericManager.put(command, new GenericManager(messageServer, command));
    return mapOfGenericManager.get(command);
  }

  public File getMainDirectory()
  {
    return mainDirectory;
  }

  /**
   * check if the server is connected,
   * if not, give error message to the user and show the connection dialog 
   */
  public boolean checkConnected()
  {
    if(!isTriggerConnect())
    {
      return true;
    }
    
    if (Helper.checkConnected(messageServer))
    {
      return true;
    } else
    {
      showConnectionDialog();
    }
    return messageServer.isConnected();
  }//end checkConnected

  public void connect(String host, int port)
  {
    try
    {
      messageServer.connect(host, port);
    } catch (IOException ex)
    {
      Helper.handleException(ex);
    }
  }

  public void disconnect()
  {
    messageServer.disconnect();
  }

  private void aboutMenuItemActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_aboutMenuItemActionPerformed

    AboutDialog dlg = new AboutDialog(this, true);
    Point location = this.getLocation();
    location.translate(100, 100);
    dlg.setLocation(location);
    dlg.setVisible(true);

  }//GEN-LAST:event_aboutMenuItemActionPerformed

  private void beforeClose()
  {
    // save configuration //
  
  // get all open dialogs
  StringBuilder openDialogs = new StringBuilder("");
  Set<String> set = dialogCounter.keySet();
 
  List<String> list = new LinkedList<String>();
  for(String className : set)
  {
    int val = dialogCounter.get(className);
    for(int i=0; i < val; i++)
    {
      list.add(className);
    }
  }
  int i=0;
  for(String d : list)
  {
    openDialogs.append(d);
    i++;
    if(i < list.size())
    {
      openDialogs.append(",");
    }
  }
  getConfig().put("dialogs", openDialogs.toString());

  // save to file
  try
  {
    getConfig().store(new FileWriter(fConfig), "");
  }
  catch(IOException ex)
  {
    Helper.handleException(ex);
  }

  messageServer.disconnect();

  if(pluginManager != null)
  {
    pluginManager.shutdown();
  }

}

private void formWindowClosing(java.awt.event.WindowEvent evt) {//GEN-FIRST:event_formWindowClosing

  beforeClose();
  
}//GEN-LAST:event_formWindowClosing

private void exitMenuItemActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_exitMenuItemActionPerformed
{//GEN-HEADEREND:event_exitMenuItemActionPerformed

  beforeClose();
  System.exit(0);

}//GEN-LAST:event_exitMenuItemActionPerformed

private void resetLayoutMenuItemActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_resetLayoutMenuItemActionPerformed
{//GEN-HEADEREND:event_resetLayoutMenuItemActionPerformed

  if(layoutFile.exists() && layoutFile.isFile() && layoutFile.canWrite())
  {
    layoutFile.delete();
    DockingManager.setAutoPersist(false);
    JOptionPane.showMessageDialog(null, "You need to restart RobotControl now.");
  }//end if

}//GEN-LAST:event_resetLayoutMenuItemActionPerformed

private void connectMenuItemActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_connectMenuItemActionPerformed
{//GEN-HEADEREND:event_connectMenuItemActionPerformed
  showConnectionDialog();
}//GEN-LAST:event_connectMenuItemActionPerformed

private void showConnectionDialog()
{
    connectionDialog.setVisible(true);
}

private void disconnectMenuItemActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_disconnectMenuItemActionPerformed
{//GEN-HEADEREND:event_disconnectMenuItemActionPerformed

  messageServer.disconnect();

}//GEN-LAST:event_disconnectMenuItemActionPerformed

private void btManagerActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btManagerActionPerformed

  String str = "Currently registeres Manager:\n\n";
  for(int i = 0; i < messageServer.getListeners().size(); i++)
  {
    str += messageServer.getListeners().get(i).getClass().getSimpleName() + "\n";
  }//end for
  str += "\n";
  
  JOptionPane.showMessageDialog(this, str);
}//GEN-LAST:event_btManagerActionPerformed


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


  /**
   * @param args the command line arguments
   */
  public static void main(String args[]) {
    java.awt.EventQueue.invokeLater(new Runnable() {
      public void run() 
      {
        Main m = new Main();
        m.setVisible(true);
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
  private javax.swing.JLabel lblRecivedBytesS;
  private javax.swing.JLabel lblSentBytesS;
  private javax.swing.JMenu mainControlMenu;
  private javax.swing.JMenuBar menuBar;
  private javax.swing.JMenuItem resetLayoutMenuItem;
  private javax.swing.JPanel statusPanel;
  // End of variables declaration//GEN-END:variables

  public void setTriggerConnect(boolean triggerConnect) {
    this.triggerConnect = triggerConnect;
  }

  public boolean isTriggerConnect() {
    return triggerConnect;
  }

  @Override
  public void dispose()
  {
    if(pluginManager != null)
    {
      pluginManager.shutdown();
    }
  }


  
}//end Main
