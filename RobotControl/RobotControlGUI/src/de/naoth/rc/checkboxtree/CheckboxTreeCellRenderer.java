/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.naoth.rc.checkboxtree;

import java.awt.Color;
import java.awt.Component;
import javax.swing.JCheckBox;
import javax.swing.JTree;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.TreeCellRenderer;

/**
 *
 * @author thomas
 */
public class CheckboxTreeCellRenderer implements TreeCellRenderer
{

  @Override
  public Component getTreeCellRendererComponent(JTree tree, Object value, boolean selected, boolean expanded, boolean leaf, int row, boolean hasFocus)
  {
    if(value != null)
    {
      JCheckBox cb = new JCheckBox();

      cb.setBackground(tree.getBackground());

      if(value instanceof SelectableTreeNode)
      {
        SelectableTreeNode n = (SelectableTreeNode) value;

        
        cb.setText(n.getText());
        cb.setToolTipText(n.getTooltip());
        cb.setEnabled(true);

        cb.setSelected(n.isSelected());
      }
      else
      {
        cb.setText(value.toString());
        cb.setSelected(false);
      }
      return cb;
    }
    return null;
  }

}
