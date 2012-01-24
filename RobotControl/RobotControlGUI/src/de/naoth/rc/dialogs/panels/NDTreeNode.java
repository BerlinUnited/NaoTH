package de.naoth.rc.dialogs.panels;

import javax.swing.tree.DefaultMutableTreeNode;

public class NDTreeNode extends DefaultMutableTreeNode
{

  private String toolTipText;

  public NDTreeNode(String str, String toolTipText)
  {
    super(str);
    this.toolTipText = toolTipText;
  }

  public String getToolTipText()
  {
    return toolTipText;
  }
}
