/*
 * 
 */

package de.naoth.rc.components.behaviorviewer;

import static de.naoth.rc.dialogs.BehaviorViewer.BOLD_FONT;
import static de.naoth.rc.dialogs.BehaviorViewer.DARK_GREEN;
import static de.naoth.rc.dialogs.BehaviorViewer.ITALIC_FONT;
import static de.naoth.rc.dialogs.BehaviorViewer.PLAIN_FONT;
import de.naoth.rc.messages.Messages;
import java.awt.Color;
import java.awt.Component;
import java.awt.Font;
import javax.swing.JLabel;
import javax.swing.JTree;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.TreeCellRenderer;

/**
 *
 * @author Heinrich Mellmann
 */
public class XABSLActionTreeCellRenderer implements TreeCellRenderer {
    @Override
    public Component getTreeCellRendererComponent(JTree tree, Object value, boolean selected, boolean expanded, boolean leaf, int row, boolean hasFocus)
    {
      if(value != null)
      {
        DefaultMutableTreeNode n = (DefaultMutableTreeNode) value;

        // default values for color and font
        Color color = Color.black;
        Font font = PLAIN_FONT;
        StringBuilder text = new StringBuilder();

        if(n.getUserObject() instanceof Messages.XABSLAction)
        {
          Messages.XABSLAction a = (Messages.XABSLAction) n.getUserObject();


          if(a.getType() == Messages.XABSLAction.ActionType.Option)
          {
            color = DARK_GREEN;
            font = BOLD_FONT;
            text.append(a.getName());
            text.append(" [");
            text.append(a.getTimeOfExecution());
            text.append(" ms] - ");
            text.append(a.getActiveState());
            text.append(" [");
            text.append(a.getStateTime());
            text.append(" ms]");
          }
          else if(a.getType() == Messages.XABSLAction.ActionType.BasicBehavior)
          {
            text.append(a.getName());
          }
          else if(a.getType() == Messages.XABSLAction.ActionType.BooleanOutputSymbol)
          {
            text.append(a.getName());
            text.append("=");
            text.append(a.getBoolValue());
          }
          else if(a.getType() == Messages.XABSLAction.ActionType.DecimalOutputSymbol)
          {
            text.append(a.getName());
            text.append("=");
            text.append(a.getDecimalValue());
          }
          else if(a.getType() == Messages.XABSLAction.ActionType.EnumOutputSymbol)
          {
            text.append(a.getName());
            text.append("=");
            String tmp_value = a.getEnumValue().replace(a.getName()+".", "");
            text.append(tmp_value);
          }
          
        }
        else if(n.getUserObject() instanceof Messages.XABSLParameter)
        {
          Messages.XABSLParameter p = (Messages.XABSLParameter) n.getUserObject();
          color = Color.DARK_GRAY;
          font = ITALIC_FONT;

          text.append("@");
          text.append(p.getName());
          if(p.getType() == Messages.XABSLParameter.ParamType.Boolean)
          {
            text.append("=");
            text.append(p.getBoolValue());
          }
          else if(p.getType() == Messages.XABSLParameter.ParamType.Decimal)
          {
            text.append("=");
            text.append(p.getDecimalValue());
          }
          if(p.getType() == Messages.XABSLParameter.ParamType.Enum)
          {
            text.append("=");
            String tmp_value = p.getEnumValue().replace(p.getName()+".", "");
            text.append(tmp_value);
          }
        }
        else
        {
          text.append(n.getUserObject().toString());
        }

        JLabel label = new JLabel(text.toString());
        label.setForeground(color);
        label.setFont(font);
        return label;
      }
      else
      {
        return new JLabel("null");
      }
    }
}
