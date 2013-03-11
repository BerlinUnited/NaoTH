/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.naoth.rc.checkboxtree;

import java.awt.Component;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.AbstractCellEditor;
import javax.swing.JCheckBox;
import javax.swing.JTree;
import javax.swing.tree.TreeCellEditor;

/**
 *
 * @author thomas
 */
public class SelectableTreeCellEditor 
    extends AbstractCellEditor
    implements TreeCellEditor, ActionListener
{
  private SelectableTreeNode lastNode;

  @Override
  public Component getTreeCellEditorComponent(
          JTree tree, Object value, boolean isSelected, 
          boolean expanded, boolean leaf, int row)
  {
    if(value != null)
    {
      if( value instanceof SelectableTreeNode)
      {
        lastNode = (SelectableTreeNode) value;
        JCheckBox cb = lastNode.getComponent();
        cb.addActionListener(this);
        return cb;
      }
    }
    return null;
  }
  
  @Override
  public Object getCellEditorValue()
  {
    return lastNode;
  }
  
  @Override
  public void actionPerformed(ActionEvent e)
  {
    JCheckBox cb = (JCheckBox) e.getSource();
    cb.removeActionListener(this);
    fireEditingStopped();
  }
}
