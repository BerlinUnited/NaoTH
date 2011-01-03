/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.hu_berlin.informatik.ki.nao.checkboxtree;

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
public class SelectableTreeCellEditor extends AbstractCellEditor 
  implements TreeCellEditor, ActionListener
{

  private SelectableTreeNode lastNode;

  @Override
  public Object getCellEditorValue()
  {
    return lastNode;
  }

  @Override
  public Component getTreeCellEditorComponent(JTree tree, Object value, boolean isSelected, boolean expanded, boolean leaf, int row)
  {
    JCheckBox cb = new JCheckBox();
    cb.addActionListener(this);
    
    cb.setBackground(tree.getBackground());

    if(value != null)
    {
      if( value instanceof SelectableTreeNode)
      {
        lastNode = (SelectableTreeNode) value;
        cb.setText(lastNode.getText());
        cb.setToolTipText(lastNode.getTooltip());
        cb.setSelected(lastNode.isSelected());
      }
      else
      {
        lastNode = null;
        cb.setText(value.toString());
      }
    }
    
    return cb;
  }

  @Override
  public void actionPerformed(ActionEvent e)
  {
    if(e.getSource() instanceof JCheckBox)
    {
      JCheckBox cb = (JCheckBox) e.getSource();
      if(lastNode != null)
      {
        lastNode.setSelected(cb.isSelected());
      }
      fireEditingStopped();
    }
  }

}
