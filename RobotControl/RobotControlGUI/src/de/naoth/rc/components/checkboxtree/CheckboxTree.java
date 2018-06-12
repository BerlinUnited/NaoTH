/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc.components.checkboxtree;

import java.util.ArrayList;
import java.util.Enumeration;
import java.util.LinkedList;
import java.util.List;
import javax.swing.JTree;
import javax.swing.ToolTipManager;
import javax.swing.event.CellEditorListener;
import javax.swing.event.ChangeEvent;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.TreePath;
import javax.swing.tree.TreeSelectionModel;

/**
 *
 * @author thomas
 */
public class CheckboxTree extends JTree
{

  private final SelectableTreeNode rootNode;
  private final DefaultTreeModel model;
  private boolean nonLeafNodesEditable;

  public CheckboxTree()
  {
    this.rootNode = new SelectableTreeNode("all debug requests", "", false);
    this.model = new DefaultTreeModel(rootNode);
    this.nonLeafNodesEditable = true;
    
    this.setModel(model);
    this.setCellRenderer(new CheckboxTreeCellRenderer());
    this.setCellEditor(new SelectableTreeCellEditor());
    
    this.getSelectionModel().setSelectionMode(TreeSelectionModel.SINGLE_TREE_SELECTION);
    this.setRootVisible(false);
    this.setEditable(true);
    this.setShowsRootHandles(true);
    this.setScrollsOnExpand(true);
    
    
    this.getCellEditor().addCellEditorListener(new CellEditorListener() {
        @Override
        public void editingStopped(ChangeEvent e) {
            repaint();
        }

        @Override
        public void editingCanceled(ChangeEvent e){}
    });
    
    ToolTipManager.sharedInstance().registerComponent(this);
  }

  public SelectableTreeNode getRootNode()
  {
    return rootNode;
  }

  public void insertPath(String path)
  {
    insertPath(path, '/');
  }

  public SelectableTreeNode insertPath(String path, char seperator)
  {
    String[] nodes = path.split("" + seperator);
    SelectableTreeNode current = rootNode;
    for(String n : nodes)
    {
      SelectableTreeNode matchingNode = null;
      Enumeration<SelectableTreeNode> childEnum = current.children();

      while(childEnum.hasMoreElements())
      {
        SelectableTreeNode child = childEnum.nextElement();

        if(n.equals(child.getText()))
        {
          matchingNode = child;
          break;
        }
      }

      if(matchingNode == null)
      {
        // add a new one
        matchingNode = new SelectableTreeNode(n, null, false);
        matchingNode.setParent(current);
        model.insertNodeInto(matchingNode, current, current.getChildCount());
        // current is not a leaf node
        current.setEnabled(this.nonLeafNodesEditable);
      }
      current = matchingNode;
    }//end for
    
    this.expandPath(new TreePath(rootNode));
    
    return current;
  }//end insertPath
  
  public void cleanTree()
  {
      cleanTree(rootNode);
  }
  
  /**
   * Remove all intermediate nodes which contain only one child with the same name.
   * @param current
   * @return 
   */
  private SelectableTreeNode cleanTree(SelectableTreeNode current)
  {
    if (current.getChildCount() == 0) {
        return current;
    }
    
    // only one leaf-child with the same name
    if(current.getChildCount() == 1 &&
       //current.getChildAt(0).isLeaf() && 
       current.getText().toLowerCase().equals(current.getChildAt(0).getText().toLowerCase())) {
        return current.getChildAt(0);
    }
    
    // clean recursive
    for(int i = 0; i < current.getChildCount(); ++i) {
        SelectableTreeNode node = cleanTree(current.getChildAt(i));
        if(node != current.getChildAt(i)) {
            current.insert(node, i);
            current.remove(i+1);
        }
    }

    return current;
  }//end cleanTree
  
  public void expandPath(String path, char seperator)
  {
    SelectableTreeNode node = getNode(path,seperator);
    this.expandPath(new TreePath(new Object[]{rootNode,node}));
  }
  
  public SelectableTreeNode getNode(String path)
  {
    return getNode(path, '/');
  }
  
  public SelectableTreeNode getNode(String path, char seperator)
  {
    String[] nodes = path.split("" + seperator);
    SelectableTreeNode current = rootNode;
    for(String n : nodes)
    {
      SelectableTreeNode matchingNode = null;
      Enumeration<SelectableTreeNode> childEnum = current.children();
      while(childEnum.hasMoreElements())
      {
        SelectableTreeNode child = childEnum.nextElement();

        if(n.equals(child.getText()))
        {
          matchingNode = child;
          break;
        }
      }

      if(matchingNode == null)
      {
        return null;
      }

      current = matchingNode;
    }
    return current;
  }
  
  private List<SelectableTreeNode> getNodeTreePath(String path, char seperator)
  {
    ArrayList<SelectableTreeNode> list = new ArrayList<>();
    String[] nodes = path.split("" + seperator);
    SelectableTreeNode current = rootNode;
    
    list.add(current);
    
    for(String n : nodes)
    {
      SelectableTreeNode matchingNode = null;
      Enumeration<SelectableTreeNode> childEnum = current.children();
      while(childEnum.hasMoreElements())
      {
        SelectableTreeNode child = childEnum.nextElement();

        if(n.equals(child.getText()))
        {
          matchingNode = child;
          break;
        }
      }

      if(matchingNode == null)
      {
        return null;
      }

      current = matchingNode;
      list.add(current);
    }
    
    return list;
  }
  
  
  public void selectNode(String path, char seperator)
  {
    List<SelectableTreeNode> nodepath = getNodeTreePath(path, seperator);
    if(nodepath != null) {
        super.setSelectionPath(new TreePath(nodepath.toArray()));
    }
  }

  public void clear()
  {
    List<SelectableTreeNode> nodes = new LinkedList<SelectableTreeNode>();
    Enumeration<SelectableTreeNode> enumChildren = rootNode.children();
    while(enumChildren.hasMoreElements())
    {
      nodes.add(enumChildren.nextElement());
    }
    for(SelectableTreeNode n : nodes)
    {
      model.removeNodeFromParent(n);
    }
  }

    public boolean isNonLeafNodesEditable() {
        return nonLeafNodesEditable;
    }

    public void setNonLeafNodesEditable(boolean nonLeafNodesEditable) {
        this.nonLeafNodesEditable = nonLeafNodesEditable;
    }
}
