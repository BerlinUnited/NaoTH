/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc.checkboxtree;

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

  private SelectableTreeNode rootNode;
  private DefaultTreeModel model;

  public CheckboxTree()
  {
    rootNode = new SelectableTreeNode("all debug requests", "", false);
    model = new DefaultTreeModel(rootNode);
    
    this.setModel(model);
    this.setCellRenderer(new CheckboxTreeCellRenderer());
    this.setCellEditor(new SelectableTreeCellEditor());
    
    this.getSelectionModel().setSelectionMode(TreeSelectionModel.SINGLE_TREE_SELECTION);
    this.setRootVisible(false);
    this.setEditable(true);
    this.setShowsRootHandles(true);
    
    
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
      }
      current = matchingNode;
    }
    
    this.expandPath(new TreePath(rootNode));
    
    return current;
  }//end insertPath
  
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
}
