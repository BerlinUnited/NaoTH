/*
 * DebugRequestPanel.java
 *
 * Created on 9. Mai 2008, 11:55
 */
package de.hu_berlin.informatik.ki.nao.extendeddebugrequest;
import de.hu_berlin.informatik.ki.nao.Dialog;
import de.hu_berlin.informatik.ki.nao.RobotControl;
import de.hu_berlin.informatik.ki.nao.dialogs.panels.NDTreeNode;
import de.hu_berlin.informatik.ki.nao.manager.DebugRequestManager;
import de.hu_berlin.informatik.ki.nao.manager.ObjectListener;
import de.hu_berlin.informatik.ki.nao.server.Command;
import de.hu_berlin.informatik.ki.nao.server.CommandSender;
import de.hu_berlin.informatik.ki.nao.server.MessageServer;
import it.cnr.imaa.essi.lablib.gui.checkboxtree.TreeCheckingEvent;
import it.cnr.imaa.essi.lablib.gui.checkboxtree.TreeCheckingListener;
import it.cnr.imaa.essi.lablib.gui.checkboxtree.TreeCheckingModel.CheckingMode;
import java.util.Enumeration;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.MutableTreeNode;
import javax.swing.tree.TreePath;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.events.Init;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 *
 * @author  Heinrich Mellmann
 */
@PluginImplementation
public class DebugRequestPanel extends javax.swing.JPanel
  implements CommandSender, ObjectListener<String[]>, Dialog
{

  @InjectPlugin
  public RobotControl parent;

  @InjectPlugin
  public DebugRequestManager debugRequestManager;

  private MessageServer server;

  /** Creates new form DebugRequestPanel */
  public DebugRequestPanel()
  {
    initComponents();
  }

  @Init
  public void init()
  {
    server = parent.getMessageServer();

    TreeCheckingListener treeListenerMain = new TreeCheckingListener()
    {

      public void valueChanged(TreeCheckingEvent e)
      {
        Object node = e.getPath().getLastPathComponent();
        if (node instanceof DefaultMutableTreeNode)
        {
          sendCommand((DefaultMutableTreeNode) node, e.isCheckedPath());
          //System.out.println(extendedCheckboxTree.getCheckingModel());
        }
      }

      private void sendCommand(DefaultMutableTreeNode node, boolean enable)
      {
        if (node.isLeaf())
        {
          sendLeafeCommand(node, enable);
        }

        Enumeration children = node.children();
        while (children.hasMoreElements())
        {
          DefaultMutableTreeNode child = (DefaultMutableTreeNode) children.nextElement();
          sendCommand(child, enable);
        }//end while
      }//end sendCommand

      private void sendLeafeCommand(DefaultMutableTreeNode node, boolean enable)
      {
        Object[] path = node.getPath();
        if (path.length < 2)
        {
          return;
        }
        String debugRequestName = path[1].toString();

        for (int i = 2; i < path.length; i++)
        {
          debugRequestName += ":" + path[i];
        }//end for

        //Command command = new Command().setName(debugRequestName).addArgs(CMDArg.newBuilder().setName(enable ? "on" : "off")).build();
        String param = enable ? "on" : "off";
        Command command = new Command(debugRequestName).addArg(param);

        System.err.println(debugRequestName + " " + param);
        send(command);
      }
    };

    TreeCheckingListener treeListenerFav = new TreeCheckingListener()
    {

      public void valueChanged(TreeCheckingEvent e)
      {
        boolean enable = e.isCheckedPath();
        String debugRequestName = joinPath(e.getPath());

        String param = enable ? "on" : "off";
        Command command = new Command(debugRequestName).addArg(param);

        System.err.println(debugRequestName + " " + param);
        send(command);
      }
    };

    extendedCheckboxTree.addTreeCheckingListener(treeListenerMain);
    favCheckboxTree.addTreeCheckingListener(treeListenerFav);

  }

  public JPanel getPanel()
  {
    return this;
  }

  /** This method is called from within the constructor to
   * initialize the form.
   * WARNING: Do NOT modify this code. The content of this method is
   * always regenerated by the Form Editor.
   */
  // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
  private void initComponents() {

    jToolBar1 = new javax.swing.JToolBar();
    btRefresh = new javax.swing.JToggleButton();
    splitPane = new javax.swing.JSplitPane();
    scrollPaneContent = new javax.swing.JScrollPane();
    extendedCheckboxTree = new de.hu_berlin.informatik.ki.nao.extendeddebugrequest.ExtendedCheckboxTree();
    panelFav = new javax.swing.JPanel();
    btAdd = new javax.swing.JButton();
    btRemove = new javax.swing.JButton();
    scrollPaneFavs = new javax.swing.JScrollPane();
    favCheckboxTree = new de.hu_berlin.informatik.ki.nao.extendeddebugrequest.ExtendedCheckboxTree();

    jToolBar1.setRollover(true);

    btRefresh.setText("Refresh");
    btRefresh.setFocusable(false);
    btRefresh.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
    btRefresh.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
    btRefresh.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        btRefreshActionPerformed(evt);
      }
    });
    jToolBar1.add(btRefresh);

    splitPane.setDividerSize(10);
    splitPane.setOrientation(javax.swing.JSplitPane.VERTICAL_SPLIT);
    splitPane.setResizeWeight(1.0);
    splitPane.setOneTouchExpandable(true);

    scrollPaneContent.setViewportView(extendedCheckboxTree);

    splitPane.setTopComponent(scrollPaneContent);

    btAdd.setIcon(new javax.swing.ImageIcon(getClass().getResource("/de/hu_berlin/informatik/ki/nao/res/arrowdown.png"))); // NOI18N
    btAdd.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        btAddActionPerformed(evt);
      }
    });

    btRemove.setIcon(new javax.swing.ImageIcon(getClass().getResource("/de/hu_berlin/informatik/ki/nao/res/arrowup.png"))); // NOI18N
    btRemove.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        btRemoveActionPerformed(evt);
      }
    });

    scrollPaneFavs.setViewportView(favCheckboxTree);

    javax.swing.GroupLayout panelFavLayout = new javax.swing.GroupLayout(panelFav);
    panelFav.setLayout(panelFavLayout);
    panelFavLayout.setHorizontalGroup(
      panelFavLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(panelFavLayout.createSequentialGroup()
        .addComponent(btAdd)
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
        .addComponent(btRemove)
        .addContainerGap(62, Short.MAX_VALUE))
      .addComponent(scrollPaneFavs, javax.swing.GroupLayout.DEFAULT_SIZE, 124, Short.MAX_VALUE)
    );
    panelFavLayout.setVerticalGroup(
      panelFavLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(panelFavLayout.createSequentialGroup()
        .addGroup(panelFavLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
          .addComponent(btAdd)
          .addComponent(btRemove))
        .addGap(0, 0, 0)
        .addComponent(scrollPaneFavs, javax.swing.GroupLayout.DEFAULT_SIZE, 23, Short.MAX_VALUE))
    );

    splitPane.setRightComponent(panelFav);

    javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
    this.setLayout(layout);
    layout.setHorizontalGroup(
      layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addComponent(jToolBar1, javax.swing.GroupLayout.DEFAULT_SIZE, 124, Short.MAX_VALUE)
      .addComponent(splitPane, javax.swing.GroupLayout.DEFAULT_SIZE, 124, Short.MAX_VALUE)
    );
    layout.setVerticalGroup(
      layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(layout.createSequentialGroup()
        .addComponent(jToolBar1, javax.swing.GroupLayout.PREFERRED_SIZE, 25, javax.swing.GroupLayout.PREFERRED_SIZE)
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
        .addComponent(splitPane, javax.swing.GroupLayout.DEFAULT_SIZE, 118, Short.MAX_VALUE))
    );
  }// </editor-fold>//GEN-END:initComponents
    private void btRefreshActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btRefreshActionPerformed
      if (btRefresh.isSelected())
      {
        if (parent.checkConnected())
        {
          debugRequestManager.addListener(this);
          this.extendedCheckboxTree.clean();
        } else
        {
          btRefresh.setSelected(false);
        }
      } else
      {
        debugRequestManager.removeListener(this);
      }
    }//GEN-LAST:event_btRefreshActionPerformed

    private void btAddActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_btAddActionPerformed
    {//GEN-HEADEREND:event_btAddActionPerformed

      TreePath selectedPath = extendedCheckboxTree.getSelectionPath();

      DefaultTreeModel model = (DefaultTreeModel) favCheckboxTree.getModel();
      DefaultMutableTreeNode root = (DefaultMutableTreeNode) model.getRoot();

      if(selectedPath != null && model.getChildCount(selectedPath.getLastPathComponent()) == 0)
      {

        String fullPath = joinPath(selectedPath);
        NDTreeNode newNode = new NDTreeNode(fullPath, fullPath);

        model.insertNodeInto(newNode, root, 0);
        model.reload();
      }
      else
      {
        JOptionPane.showMessageDialog(this, "Only leafs can be added to the favourite list");
      }
    }//GEN-LAST:event_btAddActionPerformed

    private void btRemoveActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_btRemoveActionPerformed
    {//GEN-HEADEREND:event_btRemoveActionPerformed

      TreePath selectedPath = favCheckboxTree.getSelectionPath();
      if(selectedPath != null)
      {
        MutableTreeNode toRemove = (MutableTreeNode) selectedPath.getLastPathComponent();
        DefaultTreeModel model = (DefaultTreeModel) favCheckboxTree.getModel();
        model.removeNodeFromParent(toRemove);
        model.reload();
      }
    }//GEN-LAST:event_btRemoveActionPerformed

  public void errorOccured(String cause)
  {
    btRefresh.setSelected(false);
    debugRequestManager.removeListener(this);
  }

  public void newObjectReceived(String[] object)
  {

    extendedCheckboxTree.getCheckingModel().setCheckingMode(CheckingMode.SIMPLE);
    for (String str : object)
    {
      String[] tokens = str.split("\\|");

      String tooltip = "NO COMMENT";
      if (tokens.length >= 3)
      {
        tooltip = tokens[2];

      }
      if (tokens.length >= 2)
      {
        boolean selected = tokens[1].equals("1");
        extendedCheckboxTree.addPath(tokens[0], tooltip, selected);

        DefaultTreeModel model = (DefaultTreeModel) favCheckboxTree.getModel();

        DefaultMutableTreeNode root =  (DefaultMutableTreeNode) model.getRoot();

        for(int i=0; i < root.getChildCount(); i++)
        {
          DefaultMutableTreeNode node = (DefaultMutableTreeNode) root.getChildAt(i);
          if(node.getUserObject().toString().equals(tokens[0]))
          {
            TreePath path = new TreePath(node.getPath());
            if(selected)
            {
              favCheckboxTree.getCheckingModel().addCheckingPath(path);
            }
            else
            {
              favCheckboxTree.getCheckingModel().removeCheckingPath(path);
            }
          }
        }

      }
    }//end for
    extendedCheckboxTree.getCheckingModel().setCheckingMode(CheckingMode.PROPAGATE_PRESERVING_UNCHECK);

    btRefresh.setSelected(false);
    debugRequestManager.removeListener(this);
  }//end newObjectReceived

  private void send(Command command)
  {
    if (parent.checkConnected())
    {
      server.executeSingleCommand(this, command);
    }
  }

  public void handleResponse(byte[] result, Command originalCommand)
  {
    // TODO: do something
  }

  public void handleError(int code)
  {

  }//end handleError

  public Command getCurrentCommand()
  {
    return new Command("ping");
  }

  private String joinPath(TreePath path)
  {
    StringBuilder builder = new StringBuilder();

    Object[] asArray = path.getPath();

    // skip "root", start at 1
    for (int i = 1; i < asArray.length; i++)
    {
      if (i > 1)
      {
        builder.append(":");
      }
      builder.append(asArray[i].toString());
    }

    return builder.toString();
  }

  public void dispose()
  {
    System.out.println("Dispose is not implemented for: " + this.getClass().getName());
  }//end dispose
    
  // Variables declaration - do not modify//GEN-BEGIN:variables
  private javax.swing.JButton btAdd;
  private javax.swing.JToggleButton btRefresh;
  private javax.swing.JButton btRemove;
  private de.hu_berlin.informatik.ki.nao.extendeddebugrequest.ExtendedCheckboxTree extendedCheckboxTree;
  private de.hu_berlin.informatik.ki.nao.extendeddebugrequest.ExtendedCheckboxTree favCheckboxTree;
  private javax.swing.JToolBar jToolBar1;
  private javax.swing.JPanel panelFav;
  private javax.swing.JScrollPane scrollPaneContent;
  private javax.swing.JScrollPane scrollPaneFavs;
  private javax.swing.JSplitPane splitPane;
  // End of variables declaration//GEN-END:variables
}
