/*
 * 
 */

/*
 * DebugRequestPanel.java
 *
 * Created on 29.10.2010, 15:15:34
 */
package de.naoth.rc.dialogs;

import com.google.protobuf.InvalidProtocolBufferException;
import de.naoth.rc.core.dialog.AbstractDialog;
import de.naoth.rc.core.dialog.DialogPlugin;
import de.naoth.rc.RobotControl;
import de.naoth.rc.components.checkboxtree.SelectableTreeNode;
import de.naoth.rc.core.dialog.RCDialog;
import de.naoth.rc.core.manager.ObjectListener;
import de.naoth.rc.core.manager.SwingCommandExecutor;
import de.naoth.rc.core.manager.SwingCommandListener;
import de.naoth.rc.core.messages.Messages.DebugRequest;
import de.naoth.rc.core.server.Command;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.ToolTipManager;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 *
 * @author thomas
 */
public class DebugRequestPanel extends AbstractDialog
{

    @RCDialog(category = RCDialog.Category.Debug, name = "DebugRequest")
    @PluginImplementation
    public static class Plugin extends DialogPlugin<DebugRequestPanel>
    {
        @InjectPlugin
        public static RobotControl parent;
        @InjectPlugin
        public static SwingCommandExecutor commandExecutor;
    }
    
  DebugRequestUpdater debugRequestUpdaterCognition = new DebugRequestUpdater("[Cognition]", "Cognition:representation:set");
  DebugRequestUpdater debugRequestUpdaterMotion = new DebugRequestUpdater("[Motion]", "Motion:representation:set");
    
  /** Creates new form DebugRequestPanel */
  public DebugRequestPanel()
  {
    initComponents();
    
    ToolTipManager.sharedInstance().setDismissDelay(60000);
    this.debugRequestTree.setNonLeafNodesEditable(false);
    
    // NOTE: experimental auto update on connect
    /*
    Plugin.parent.getMessageServer().addConnectionStatusListener(new ConnectionStatusListener() 
    {
        @Override
        public void connected(ConnectionStatusEvent event) {
            Plugin.dbgRequestManager.addListener(DebugRequestPanel.this);
        }
        @Override
        public void disconnected(ConnectionStatusEvent event) {
        }
    });
    */
  }

  /** This method is called from within the constructor to
   * initialize the form.
   * WARNING: Do NOT modify this code. The content of this method is
   * always regenerated by the Form Editor.
   */
  @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        toolbarMain = new javax.swing.JToolBar();
        btUpdate = new javax.swing.JToggleButton();
        btRefresh = new javax.swing.JToggleButton();
        jScrollPane = new javax.swing.JScrollPane();
        debugRequestTree = new de.naoth.rc.components.checkboxtree.CheckboxTree();

        setLayout(new java.awt.BorderLayout());

        toolbarMain.setFloatable(false);
        toolbarMain.setRollover(true);

        btUpdate.setText("Update");
        btUpdate.setFocusable(false);
        btUpdate.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btUpdate.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btUpdate.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btUpdateActionPerformed(evt);
            }
        });
        toolbarMain.add(btUpdate);

        btRefresh.setText("Reload");
        btRefresh.setFocusable(false);
        btRefresh.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btRefresh.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btRefresh.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btRefreshActionPerformed(evt);
            }
        });
        toolbarMain.add(btRefresh);

        add(toolbarMain, java.awt.BorderLayout.PAGE_START);

        jScrollPane.setBorder(null);
        jScrollPane.setViewportView(debugRequestTree);

        add(jScrollPane, java.awt.BorderLayout.CENTER);
    }// </editor-fold>//GEN-END:initComponents

  private void btRefreshActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_btRefreshActionPerformed
  {//GEN-HEADEREND:event_btRefreshActionPerformed

    this.debugRequestTree.clear();
            
    if (btRefresh.isSelected())
    {
      if (Plugin.parent.checkConnected()) {
        Plugin.commandExecutor.executeCommand(debugRequestUpdaterCognition, new Command("Cognition:representation:get").addArg("DebugRequest"));
        Plugin.commandExecutor.executeCommand(debugRequestUpdaterMotion, new Command("Motion:representation:get").addArg("DebugRequest"));
      } else {
        btRefresh.setSelected(false);
        btUpdate.setSelected(false);
      }
    }
  }//GEN-LAST:event_btRefreshActionPerformed

    private void btUpdateActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btUpdateActionPerformed
        if (btUpdate.isSelected())
        {
          if (Plugin.parent.checkConnected()) {
            Plugin.commandExecutor.executeCommand(debugRequestUpdaterCognition, new Command("Cognition:representation:get").addArg("DebugRequest"));
            Plugin.commandExecutor.executeCommand(debugRequestUpdaterMotion, new Command("Motion:representation:get").addArg("DebugRequest"));
          } else {
            btRefresh.setSelected(false);
            btUpdate.setSelected(false);
          }
        }
    }//GEN-LAST:event_btUpdateActionPerformed

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JToggleButton btRefresh;
    private javax.swing.JToggleButton btUpdate;
    private de.naoth.rc.components.checkboxtree.CheckboxTree debugRequestTree;
    private javax.swing.JScrollPane jScrollPane;
    private javax.swing.JToolBar toolbarMain;
    // End of variables declaration//GEN-END:variables
  
  private class DebugRequestAction 
    implements ActionListener, ObjectListener<byte[]>
  {
    SelectableTreeNode node;
    String path;
    String command;
    
    DebugRequestAction(String command, SelectableTreeNode node, String path) {
        this.node = node;
        this.path = path;
        this.command = command;
    }
    
    @Override
    public void actionPerformed(ActionEvent e) {
        Command cmd = new Command(command);

        cmd.addArg("DebugRequest",
            DebugRequest.newBuilder().addRequests(
                DebugRequest.Item.newBuilder().setName(path).setValue(node.isSelected())
            ).build().toByteArray()
        );
        
        Plugin.parent.getMessageServer().executeCommand(new SwingCommandListener(this), cmd);
    }
      
    @Override
    public void newObjectReceived(byte[] object)
    {
        System.out.println(new String(object));
    }
    
    @Override
    public void errorOccured(String cause)
    {
        System.err.println(cause);
    }
  }
  
  @Override
  public void dispose() {
    this.debugRequestTree.clear();
  }
  
  class DebugRequestUpdater implements ObjectListener<byte[]>
  {
    String rootName;
    String command;
    
    public DebugRequestUpdater(String rootName, String command) {
        this.rootName = rootName;
        this.command = command;
    }
      
    @Override
    public void newObjectReceived(byte[] object)
    {
      try 
      {
        DebugRequest request = DebugRequest.parseFrom(object);
        
        for(DebugRequest.Item item : request.getRequestsList())
        {
            final String path = item.getName();
            boolean selected = item.getValue();
            
            String tooltip = "NO COMMENT";
            if(item.hasDescription() && item.getDescription().length() > 0) {
                tooltip = item.getDescription();
            }
            
            final SelectableTreeNode node = debugRequestTree.insertPath(rootName + ':' + path, ':');
            node.setSelected(selected);
            node.setTooltip(tooltip);
            //NOTE: add the listener only once
            if(node.getComponent().getActionListeners().length == 0) {
                node.getComponent().addActionListener(new DebugRequestAction(command,node,path));
            }
        }
        
        debugRequestTree.repaint();
        btRefresh.setSelected(false);
        btUpdate.setSelected(false);
        
      } catch(InvalidProtocolBufferException ex) 
      {
          ex.printStackTrace();
          System.out.println(new String(object));
      }
    }

    @Override
    public void errorOccured(String cause)
    {
      btRefresh.setSelected(false);
      btUpdate.setSelected(false);
      dispose();
    }
  }
}
