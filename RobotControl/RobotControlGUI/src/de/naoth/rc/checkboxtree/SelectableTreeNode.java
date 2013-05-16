/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc.checkboxtree;

import java.awt.Color;
import java.awt.Component;
import java.awt.Graphics;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Enumeration;
import java.util.Iterator;
import java.util.List;
import javax.swing.Icon;
import javax.swing.JCheckBox;
import javax.swing.UIManager;
import javax.swing.tree.MutableTreeNode;
import javax.swing.tree.TreeNode;

/**
 * This is  a threadsafe implementation of a MutableTreeNode which is able
 * to store a caption, a tooltip and a toggle state.
 * @author thomas
 */
public class SelectableTreeNode implements MutableTreeNode, ItemListener
{

  private SelectableTreeNode parent;
  private final List<SelectableTreeNode> childNodes;
  private JCheckBox cb = new JCheckBox();
  
  private boolean semiselected = false;
  
  public SelectableTreeNode()
  {
    this("",null,false);
  }

  public SelectableTreeNode(String text, String tooltip, boolean selected)
  {
    childNodes = Collections.synchronizedList(new ArrayList<SelectableTreeNode>());
    parent = null;
    
    this.cb.setText(text);
    this.cb.setToolTipText(tooltip);
    //this.cb.setIcon(new TristateCheckBoxIcon(Color.gray));
    this.cb.setSelected(selected);
    this.cb.addItemListener(this);
  }

  @Override
    public void itemStateChanged(ItemEvent e)
    {
        setSemiselected(false);
        
        for(SelectableTreeNode c: childNodes) {
            c.setSelectedQuietChildren(e.getStateChange() == ItemEvent.SELECTED);
        }

        if(parent != null) {
            parent.updateState();
        }
    }

  public JCheckBox getComponent()
  {
      return this.cb;
  }

  @Override
  public void insert(MutableTreeNode child, int index)
  {
    insert((SelectableTreeNode) child, index);
  }

  public void insert(SelectableTreeNode child, int index)
  {
    childNodes.add(index, (SelectableTreeNode) child);
    updateState();
  }

  public boolean isSemiselected()
  {
      return this.semiselected;
  }
  
  
  private void setSemiselected(boolean value)
  {
      if(value) {
          this.cb.setIcon(new TristateCheckBoxIcon(new Color(32,32,128)));
      } else {
          this.cb.setIcon(null);
      }
      this.semiselected = value;
  }
  
  public void updateState()
  {
    if(childNodes.isEmpty()){
        return;
    }
   
    this.cb.removeItemListener(this);
    
    int selectedChildren = 0;
    int semiSelectedChildren = 0;
    
    for(SelectableTreeNode c: childNodes) {
        if(c.isSelected()){
            selectedChildren++;
        } else if(c.isSemiselected()) {
            semiSelectedChildren++;
        }
    }

    if(selectedChildren == childNodes.size()) {
        setSelected(true);
        setSemiselected(false);
        this.cb.setIcon(null);
    } else if(selectedChildren > 0 || semiSelectedChildren > 0) {
        setSelected(false);
        setSemiselected(true);
        this.cb.setIcon(new TristateCheckBoxIcon(new Color(32,32,128)));
    } else {
        setSelected(false);
        setSemiselected(false);
        this.cb.setIcon(null);
    }
    
    this.cb.addItemListener(this);
    
    if(parent != null) {
        parent.updateState();
    }
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
    return this.cb.isSelected();
  }

  
  private void setSelectedQuietChildren(boolean selected)
  {
    this.cb.removeItemListener(this);
    this.cb.setSelected(selected);
    for(SelectableTreeNode c: childNodes) {
        c.setSelectedQuietChildren(selected);
    }
    this.cb.addItemListener(this);
  }
  
  public void setSelected(boolean selected)
  {
    this.cb.setIcon(null);
    this.cb.setSelected(selected);
  }

  public String getText()
  {
    return this.cb.getText();
  }

  public void setText(String text)
  {
    this.cb.setText(text);
  }

  public String getTooltip()
  {
    return this.cb.getToolTipText();
  }

  public void setTooltip(String tooltip)
  {
    this.cb.setToolTipText(tooltip);
  }

  @Override
  public String toString()
  {
    return getText();
  }

  
    public class TristateCheckBoxIcon implements Icon {
        private Color color;
        private int width, height;
        private Icon icon;
        
        TristateCheckBoxIcon(Color iconColor) {
            color = iconColor;
            icon = UIManager.getIcon("CheckBox.icon");
            width = icon.getIconWidth()/2;
            height = icon.getIconHeight()/2;
        }

        @Override
        public void paintIcon(Component c, Graphics g, int x, int y) {
            icon.paintIcon(c, g, x, y);
            g.setColor(new Color(32,32,64,200));
            g.fillRect(x + width - 4, y+height-4, 8, 8);
            g.setColor(new Color(32,32,64,128));
            g.fillRect(x + width - 3, y+height-3, 6, 6);
            
        }

        @Override
        public int getIconWidth() {
            return icon.getIconWidth();
        }

        @Override
        public int getIconHeight() {
            return icon.getIconHeight();
        }
    }
}
