/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.hu_berlin.informatik.ki.nao.checkboxtree;

import java.awt.Color;
import java.awt.Component;
import javax.swing.AbstractCellEditor;
import javax.swing.JCheckBox;
import javax.swing.JTree;
import javax.swing.tree.TreeCellEditor;

/**
 *
 * @author thomas
 */
public class SelectableTreeCellEditor extends AbstractCellEditor implements TreeCellEditor
{

  @Override
  public Object getCellEditorValue()
  {
    return new JCheckBox();
  }

  @Override
  public Component getTreeCellEditorComponent(JTree tree, Object value, boolean isSelected, boolean expanded, boolean leaf, int row)
  {
    JCheckBox cb = new JCheckBox();
    cb.setBackground(tree.getBackground());

    if(value != null)
    {
      if( value instanceof SelectableTreeNode)
      {
        SelectableTreeNode n = (SelectableTreeNode) value;
        cb.setText(n.getText());
        cb.setToolTipText(n.getTooltip());
        cb.setSelected(n.isSelected());
      }
      else
      {
        cb.setText(value.toString());
      }
    }

    return cb;
    //throw new UnsupportedOperationException("Not supported yet.");
  }

}
