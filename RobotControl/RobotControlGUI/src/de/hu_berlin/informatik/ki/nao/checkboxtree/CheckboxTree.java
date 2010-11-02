/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package de.hu_berlin.informatik.ki.nao.checkboxtree;

import java.util.Enumeration;
import javax.swing.JTree;
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
    setModel(model);
    this.getSelectionModel().setSelectionMode(TreeSelectionModel.SINGLE_TREE_SELECTION);
    this.setCellRenderer(new CheckboxTreeCellRenderer());
    this.setModel(treeModel);
    this.setRootVisible(true);
    this.setEditable(true);
    this.setCellEditor(new SelectableTreeCellEditor());
    this.setShowsRootHandles(true);

  }

  public SelectableTreeNode getRootNode()
  {
    return rootNode;
  }

  public void insertPath(String path)
  {
    insertPath(path, '/');
  }

  public void insertPath(String path, char seperator)
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
        matchingNode = new SelectableTreeNode(n, "", false);
        model.insertNodeInto(matchingNode, current, current.getChildCount());
      }

      current = matchingNode;

    }
    
  }
}
