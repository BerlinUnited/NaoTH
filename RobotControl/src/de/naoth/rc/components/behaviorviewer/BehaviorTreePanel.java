/*
 * 
 */
package de.naoth.rc.components.behaviorviewer;

import de.naoth.rc.components.behaviorviewer.model.Symbol;
import java.util.Enumeration;
import java.util.HashMap;
import javax.swing.JTree;
import javax.swing.event.TreeExpansionEvent;
import javax.swing.event.TreeExpansionListener;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.TreePath;

/**
 *
 * @author Heinrich Mellmann
 */
public class BehaviorTreePanel extends javax.swing.JScrollPane {

    private boolean showOptionsOnly = false;
    
    /**
     * Creates new form BehaviorTreePanel
     */
    public BehaviorTreePanel() {
        initComponents();
        
        this.setViewportView(newTree);
        createNewTree(null);
    }
    
    public void setFrame(XABSLBehaviorFrame frame, XABSLBehavior behavior)
    {
        if(frame == null || behavior == null
                || behavior.agents == null ||behavior.agents.isEmpty())
        {
            return;
        }
        DefaultMutableTreeNode treeRoot 
                = new DefaultMutableTreeNode("Behavior (" + behavior.agents.get(0).name + ")");
        for (XABSLAction a : frame.actions) {
            treeRoot.add(actionToNode(a));
        }
        createNewTree(treeRoot);
    }
    
    public DefaultMutableTreeNode actionToNode(XABSLAction a) {
        DefaultMutableTreeNode result = new DefaultMutableTreeNode(a);

        if (a instanceof XABSLAction.OptionExecution) {
            XABSLAction.OptionExecution oe = (XABSLAction.OptionExecution)a;

            // add parameters
            if(!showOptionsOnly) {
                for (Symbol p : oe.option.parameters) {
                    result.add(new DefaultMutableTreeNode(p));
                }
            }
            
            for (XABSLAction sub: oe.activeSubActions) {
                // skip symbols if only options should be shown
                if(showOptionsOnly && sub instanceof XABSLAction.SymbolAssignment) { continue; }
                result.add(actionToNode(sub));
            }
        }

        return result;
    }//end actionToNode
    
    private final JTree newTree = new JTree();
    private final TreeExpansionRecorder treeExpansionRecorder = new TreeExpansionRecorder();
    
    private void createNewTree(DefaultMutableTreeNode root) {
        if (root == null) {
            root = new DefaultMutableTreeNode("Behavior");
        }
        
        DefaultTreeModel model = new DefaultTreeModel(root);
        newTree.setModel(model);
        
        // restore collapsed paths
        newTree.removeTreeExpansionListener(treeExpansionRecorder);
        
        // expand all by default
        for (int i = 0; i < newTree.getRowCount(); i++) {
            newTree.expandRow(i);
        }
        // collapse all requested
        Enumeration e = root.depthFirstEnumeration();
        while (e.hasMoreElements()) {
            Object o = e.nextElement();
            if (o instanceof DefaultMutableTreeNode) {
                DefaultMutableTreeNode n = (DefaultMutableTreeNode) o;
                
                if(n.getUserObject() instanceof XABSLAction.OptionExecution) {
                    XABSLAction.OptionExecution oe = (XABSLAction.OptionExecution) n.getUserObject();
                    if (treeExpansionRecorder.isCollapsed(oe.option.name)) {
                        newTree.collapsePath(new TreePath(n.getPath()));
                    }
                }
            }
        }//end while
        newTree.addTreeExpansionListener(treeExpansionRecorder);
        
        
        newTree.setDoubleBuffered(false);
        newTree.setCellRenderer(new XABSLActionSparseTreeCellRenderer(getFont().getSize()));
        newTree.setVisible(true);
        
        //TODO: this are preparation for jumping to the sourse, when an option is clicked
        /*
        newTree.addTreeSelectionListener(new TreeSelectionListener() {
            @Override
            public void valueChanged(TreeSelectionEvent e) {
                Object[] path = e.getPath().getPath();

                // get the leafe
                Object userObject = ((DefaultMutableTreeNode) path[path.length - 1]).getUserObject();
                if (userObject instanceof Messages.XABSLAction) {
                    Messages.XABSLAction action = (Messages.XABSLAction) userObject;

                    StringBuilder sb = new StringBuilder();
                    sb.append(action.getName())
                            .append(':')
                            .append(action.getActiveState());

                    System.out.println(sb.toString());

                    if (!action.hasActiveState()) {
                        // invole listeners here
                    }
                }
            }
        });*/

        //scrollTree.setViewportView(newTree);
        //this.removeAll();
        //this.setViewportView(newTree);
        this.validate();
    }//end createNewTree

    
    class TreeExpansionRecorder implements TreeExpansionListener
    {
        private final HashMap<String, Boolean> actionExpanded = new HashMap<>();
        
        public boolean isCollapsed(String name) {
            return Boolean.FALSE.equals(actionExpanded.get(name));
        }
        
        @Override
        public void treeExpanded(TreeExpansionEvent event) {
            if (event.getPath().getLastPathComponent() instanceof DefaultMutableTreeNode) {
                DefaultMutableTreeNode n = (DefaultMutableTreeNode) event.getPath().getLastPathComponent();
                if(n.getUserObject() instanceof XABSLAction.OptionExecution) {
                    actionExpanded.put(((XABSLAction.OptionExecution) n.getUserObject()).option.name,
                            Boolean.TRUE);
                }
            }
        }

        @Override
        public void treeCollapsed(TreeExpansionEvent event) 
        {
                if (event.getPath().getLastPathComponent() instanceof DefaultMutableTreeNode) {
                DefaultMutableTreeNode n = (DefaultMutableTreeNode) event.getPath().getLastPathComponent();
                if(n.getUserObject() instanceof XABSLAction.OptionExecution) {
                    actionExpanded.put(((XABSLAction.OptionExecution) n.getUserObject()).option.name,
                            Boolean.FALSE);
                }
            }
        }
    }

    public void setShowOptionsOnly(boolean showOptionsOnly) {
        this.showOptionsOnly = showOptionsOnly;
    }

    /**
     * This method is called from within the constructor to initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is always
     * regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        setBackground(new java.awt.Color(255, 255, 255));
    }// </editor-fold>//GEN-END:initComponents


    // Variables declaration - do not modify//GEN-BEGIN:variables
    // End of variables declaration//GEN-END:variables
}
