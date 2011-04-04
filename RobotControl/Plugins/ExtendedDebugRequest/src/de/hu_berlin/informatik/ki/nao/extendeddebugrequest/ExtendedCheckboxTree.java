package de.hu_berlin.informatik.ki.nao.extendeddebugrequest;

/**
 * 
 */
import de.hu_berlin.informatik.ki.nao.dialogs.panels.NDTreeNode;
import it.cnr.imaa.essi.lablib.gui.checkboxtree.CheckboxTree;
import it.cnr.imaa.essi.lablib.gui.checkboxtree.DefaultCheckboxTreeCellRenderer;
import it.cnr.imaa.essi.lablib.gui.checkboxtree.TreeCheckingModel.CheckingMode;

import java.awt.Component;

import java.util.Enumeration;
import javax.swing.JTree;
import javax.swing.ToolTipManager;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.TreeNode;
import javax.swing.tree.TreePath;
import javax.swing.tree.TreeSelectionModel;

/**
 * @author Heinrich Mellmann
 * 
 */
public class ExtendedCheckboxTree extends CheckboxTree
{

  public ExtendedCheckboxTree()
  {
    super(new DefaultMutableTreeNode("root"));

    getSelectionModel().setSelectionMode(TreeSelectionModel.SINGLE_TREE_SELECTION);
    // set checking mode
    getCheckingModel().setCheckingMode(CheckingMode.PROPAGATE_PRESERVING_UNCHECK);
    setRootVisible(false);

    //Enable tool tips.
    ToolTipManager.sharedInstance().registerComponent(this);
    //ToolTipManager.sharedInstance().setInitialDelay(0);
        /*
     * dismissDelay
     * enabled
     * initialDelay
     * lightWeightPopupEnabled
     * reshowDelay
     */

    setCellRenderer(new NDRenderer());
    DefaultCheckboxTreeCellRenderer renderer = (DefaultCheckboxTreeCellRenderer) getCellRenderer();
    renderer.setOpenIcon(null);
    renderer.setClosedIcon(null);
    renderer.setLeafIcon(null);
    //BasicTreeUI ui = (BasicTreeUI) tree.getUI();
    //ui.setExpandedIcon(null);
    //ui.setCollapsedIcon(null);

  }

  public void print()
  {
    printNode((DefaultMutableTreeNode) getModel().getRoot());
  }//end print

  private void printNode(TreeNode parent)
  {
    System.out.println(parent);
    printNode(parent, "  ");
  }//end printNode

  private void printNode(TreeNode parent, String prefix)
  {
    Enumeration children = parent.children();
    while (children.hasMoreElements())
    {
      DefaultMutableTreeNode child = (DefaultMutableTreeNode) children.nextElement();
      System.out.println(prefix + "+-" + child);

      if (!children.hasMoreElements())
      {
        printNode(child, prefix + "   ");
      } else
      {
        printNode(child, prefix + "|  ");
      }
    }//end while
  }//end printNode

  public void clean()
  {
    DefaultMutableTreeNode root = (DefaultMutableTreeNode) getModel().getRoot();
    root.removeAllChildren();
    getCheckingModel().clearChecking();
  }//end clean

  /**
   * adds a path "node:node1:node2" to the tree
   * @param path athe path to be added (something like "node:node1:node2")
   * @param toolText the tool text shown for the leaf node (last node in the path)
   */
  public void addPath(String path, String toolText, boolean selected)
  {
    DefaultMutableTreeNode root = (DefaultMutableTreeNode) getModel().getRoot();
    DefaultMutableTreeNode leafNode = addPath(root, path, toolText);

    if (selected)
    {
      addCheckingPath(new TreePath(leafNode.getPath()));
    }

    ((DefaultTreeModel) getModel()).reload();
  }//end addPath

  /**
   * adds a path "node:node1:node2" to the node root
   * @param root tree node to add the path to
   * @param path "node:node1:node2"
   */
  private DefaultMutableTreeNode addPath(DefaultMutableTreeNode root, String path, String toolText)
  {
    String[] p = path.split(":");
    DefaultMutableTreeNode node = root;

    for (int i = 0; i < p.length; i++)
    {
      DefaultMutableTreeNode nextChild = findChild(node, p[i]);
      if (nextChild == null)
      {
        if (i == p.length - 1)
        {
          nextChild = new NDTreeNode(p[i], toolText);
        } else
        {
          nextChild = new DefaultMutableTreeNode(p[i]);
        }
        ((DefaultTreeModel) getModel()).insertNodeInto(nextChild, node, node.getChildCount());
        //node.add(nextChild);
      }//end if
      node = nextChild;
    }//end for

    return node;
  }//end addPath

  /**
   * searches for a child node with the label name
   * @param node the pasent tree node to search in
   * @param name label to search for
   * @return child node of node with the label name or null if no child were found
   */
  private DefaultMutableTreeNode findChild(DefaultMutableTreeNode node, String name)
  {
    Enumeration children = node.children();
    DefaultMutableTreeNode result = null;

    while (children.hasMoreElements())
    {
      DefaultMutableTreeNode child = (DefaultMutableTreeNode) children.nextElement();
      if (child.toString().compareTo(name) == 0)
      {
        result = child;
        break;
      }
    }//end while
    return result;
  }//end findChild


  private class NDRenderer extends DefaultCheckboxTreeCellRenderer
  {

    public NDRenderer()
    {
    }

    @Override
    public Component getTreeCellRendererComponent(
      JTree tree,
      Object value,
      boolean sel,
      boolean expanded,
      boolean leaf,
      int row,
      boolean hasFocus)
    {

      super.getTreeCellRendererComponent(
        tree, value, sel,
        expanded, leaf, row,
        hasFocus);

      if (leaf && value instanceof NDTreeNode)
      {
        NDTreeNode node = (NDTreeNode) value;
        setToolTipText("<html><b>" + value + "</b><br>" + node.getToolTipText() + "</html>");
        //this.checkBox.setSelected(node.isSelected());
      } else
      {
        setToolTipText(null); //no tool tip
      }
      return this;
    }//end getTreeCellRendererComponent
  }//end MyRenderer
}//end class CheckboxTreeApplet

