/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.naoth.rc.components.checkboxtree;

import java.awt.Component;
import javax.swing.JCheckBox;
import javax.swing.JTree;
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
      if(value instanceof SelectableTreeNode)
      {
        SelectableTreeNode n = (SelectableTreeNode) value;
        JCheckBox cb = n.getComponent();
        cb.setBackground(tree.getBackground());
        return cb;
      }
    }
    return null;
  }
}
