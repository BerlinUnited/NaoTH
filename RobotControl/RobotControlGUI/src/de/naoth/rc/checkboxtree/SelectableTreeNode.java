/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc.checkboxtree;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Enumeration;
import java.util.Iterator;
import java.util.List;
import javax.swing.tree.MutableTreeNode;
import javax.swing.tree.TreeNode;

/**
 * This is  a threadsafe implementation of a MutableTreeNode which is able
 * to store a caption, a tooltip and a toggle state.
 * @author thomas
 */
public class SelectableTreeNode implements MutableTreeNode
{

  private SelectableTreeNode parent;
  private final List<SelectableTreeNode> childNodes;
  private String text;
  private String tooltip;
  private boolean selected;

  public SelectableTreeNode()
  {
    this("","",false);
  }

  public SelectableTreeNode(String text, String tooltip, boolean selected)
  {
    childNodes = Collections.synchronizedList(new ArrayList<SelectableTreeNode>());
    parent = null;
    
    this.text = text;
    this.tooltip = tooltip;
    this.selected = selected;
  }



  @Override
  public void insert(MutableTreeNode child, int index)
  {
    insert((SelectableTreeNode) child, index);
  }

  public void insert(SelectableTreeNode child, int index)
  {
    childNodes.add(index, (SelectableTreeNode) child);
  }

  @Override
  public void remove(int index)
  {
    childNodes.remove(index);
  }

  @Override
  public void remove(MutableTreeNode node)
  {
    remove((SelectableTreeNode) node);
  }

  public void remove(SelectableTreeNode node)
  {
    childNodes.remove(node);
  }

  @Override
  public void setUserObject(Object object)
  {
    // do nothing
  }

  @Override
  public void removeFromParent()
  {
    if (parent != null)
    {
      parent.remove(this);
    }
    parent = null;
  }

  public void setParent(SelectableTreeNode newParent)
  {
    this.parent = (SelectableTreeNode) newParent;
  }

  @Override
  public void setParent(MutableTreeNode newParent)
  {
    setParent((SelectableTreeNode) parent);
  }

  @Override
  public SelectableTreeNode getChildAt(int childIndex)
  {
    return childNodes.get(childIndex);
  }

  @Override
  public int getChildCount()
  {
    return childNodes.size();
  }

  @Override
  public SelectableTreeNode getParent()
  {
    return parent;
  }

  @Override
  public int getIndex(TreeNode node)
  {
    return childNodes.indexOf(node);
  }

  @Override
  public boolean getAllowsChildren()
  {
    return true;
  }

  @Override
  public boolean isLeaf()
  {
    return childNodes.isEmpty();
  }

  @Override
  public Enumeration<SelectableTreeNode> children()
  {
    synchronized (childNodes)
    {
      final Iterator<SelectableTreeNode> it = childNodes.iterator();
      return new Enumeration()
      {

        @Override
        public boolean hasMoreElements()
        {
          return it.hasNext();
        }

        @Override
        public SelectableTreeNode nextElement()
        {
          return it.next();
        }
      };
    }
  }

  public boolean isSelected()
  {
    return selected;
  }

  public void setSelected(boolean selected)
  {
    this.selected = selected;
  }

  public String getText()
  {
    return text;
  }

  public void setText(String text)
  {
    this.text = text;
  }

  public String getTooltip()
  {
    return tooltip;
  }

  public void setTooltip(String tooltip)
  {
    this.tooltip = tooltip;
  }

  @Override
  public String toString()
  {
    return text;
  }


}
