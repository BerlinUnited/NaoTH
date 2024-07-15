/*
 * 
 */
package de.naoth.rc.components.behaviorviewer;

import de.naoth.rc.components.behaviorviewer.model.Symbol;
import java.awt.Dimension;
import java.awt.Point;
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
    
    // two trees for "double buffering" on update
    private final JTree treeOne = new JTree();
    private final JTree treeTwo = new JTree();
    
    // currently shown tree
    private JTree currentTree   = treeOne;
    
    private final TreeExpansionRecorder treeExpansionRecorder = new TreeExpansionRecorder();
    
    
    /**
     * Creates new form BehaviorTreePanel
     */
    public BehaviorTreePanel() {
        initComponents();
        
        treeOne.setDoubleBuffered(false);
        treeOne.setCellRenderer(new XABSLActionSparseTreeCellRenderer(getFont().getSize()));
        
        treeTwo.setDoubleBuffered(false);
        treeTwo.setCellRenderer(new XABSLActionSparseTreeCellRenderer(getFont().getSize()));
        
        updateTreeView(null);
    }
    
    public void setFrame(XABSLBehaviorFrame frame, XABSLBehavior behavior)
    {
        if( frame == null || behavior == null ||
            behavior.agents == null || behavior.agents.isEmpty())
        {
            return;
        }
        
        DefaultMutableTreeNode treeRoot 
                = new DefaultMutableTreeNode("Behavior (" + behavior.agents.get(0).name + ")");
        
        for (XABSLAction a : frame.actions) {
            treeRoot.add(actionToNode(a));
        }
        updateTreeView(treeRoot);
    }
    
    public DefaultMutableTreeNode actionToNode(XABSLAction a) 
    {
        DefaultMutableTreeNode result = new DefaultMutableTreeNode(a);

        if (a instanceof XABSLAction.OptionExecution) 
        {
            XABSLAction.OptionExecution oe = (XABSLAction.OptionExecution)a;

            // add option parameters
            if(!showOptionsOnly) {
                for (Symbol p : oe.option.parameters) {
                    result.add(new DefaultMutableTreeNode(p));
                }
            }
            
            for (XABSLAction sub: oe.activeSubActions) {
                // skip symbols if only options should be shown
                if(showOptionsOnly && sub instanceof XABSLAction.SymbolAssignment) { 
                    continue; 
                }
                result.add(actionToNode(sub));
            }
        }

        return result;
    }//end actionToNode
    
    private void expandAllNodes(JTree tree, int startingIndex, int rowCount){
        for(int i = startingIndex; i < rowCount; ++i) {
            tree.expandRow(i);
        }

        if(tree.getRowCount()!=rowCount){
            expandAllNodes(tree, rowCount, tree.getRowCount());
        }
    }
    
    private void expandAllNodes(JTree tree) {
        expandAllNodes(tree, 0, tree.getRowCount());
    }
    
    private void updateTreeView(DefaultMutableTreeNode root) 
    {
        if (root == null) {
            root = new DefaultMutableTreeNode("Behavior");
        }
        
        // the expansion listener is only attached to the currently visible tree
        currentTree.removeTreeExpansionListener(treeExpansionRecorder);
        
        // switch between buffers and update the not visible tree in the background
        if(currentTree == treeOne) {
            currentTree = treeTwo;
        } else {
            currentTree = treeOne;
        }
        
        currentTree.setModel(new DefaultTreeModel(root));
        
        expandAllNodes(currentTree);
        
        // restore collapsed paths
        // collapse all requested
        Enumeration e = root.depthFirstEnumeration();
        while (e.hasMoreElements()) {
            Object o = e.nextElement();
            if (o instanceof DefaultMutableTreeNode) {
                DefaultMutableTreeNode n = (DefaultMutableTreeNode) o;
                
                if(n.getUserObject() instanceof XABSLAction.OptionExecution) {
                    XABSLAction.OptionExecution oe = (XABSLAction.OptionExecution) n.getUserObject();
                    if (treeExpansionRecorder.isCollapsed(oe.option.name)) {
                        currentTree.collapsePath(new TreePath(n.getPath()));
                    }
                }
            }
        }//end while

        currentTree.addTreeExpansionListener(treeExpansionRecorder);
        
        
        Point p = this.getViewport().getViewPosition();
        this.setViewportView(currentTree);
        currentTree.setVisible(true);
        this.getViewport().setViewPosition(p);
        
        
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
