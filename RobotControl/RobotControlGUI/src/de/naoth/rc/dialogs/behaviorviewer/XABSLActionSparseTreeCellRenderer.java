/*
 * 
 */
package de.naoth.rc.dialogs.behaviorviewer;

import static de.naoth.rc.dialogs.BehaviorViewer.BOLD_FONT;
import static de.naoth.rc.dialogs.BehaviorViewer.DARK_GREEN;
import static de.naoth.rc.dialogs.BehaviorViewer.ITALIC_FONT;
import static de.naoth.rc.dialogs.BehaviorViewer.PLAIN_FONT;
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
public class XABSLActionSparseTreeCellRenderer implements TreeCellRenderer {

    @Override
    public Component getTreeCellRendererComponent(JTree tree, Object value, boolean selected, boolean expanded, boolean leaf, int row, boolean hasFocus) {
        if (value != null) {
            DefaultMutableTreeNode n = (DefaultMutableTreeNode) value;

            // default values for color and font
            Color color = Color.black;
            Font font = PLAIN_FONT;
            StringBuilder text = new StringBuilder();

            if (n.getUserObject() instanceof XABSLAction) {
                XABSLAction a = (XABSLAction) n.getUserObject();

                if (a instanceof XABSLAction.OptionExecution) {
                    XABSLAction.OptionExecution oe = (XABSLAction.OptionExecution) a;
                    color = DARK_GREEN;
                    font = BOLD_FONT;
                    text.append(oe.option.name);
                    text.append(" [");
                    text.append(oe.timeOfExecution);
                    text.append(" ms] - ");
                    text.append(oe.option.states.get(oe.activeState).name);
                    text.append(" [");
                    text.append(oe.stateTime);
                    text.append(" ms]");
                } else if (a instanceof XABSLAction.BasicBehaviorExecution) {
                    text.append(((XABSLAction.BasicBehaviorExecution) a).name);
                } else if (a instanceof XABSLAction.SymbolAssignement) {
                    XABSLBehavior.Symbol s = ((XABSLAction.SymbolAssignement) a).symbol;

                    text.append(s.name);
                    text.append("=");
                    if (s instanceof XABSLBehavior.Symbol.Boolean) {
                        text.append(((XABSLBehavior.Symbol.Boolean) s).value);
                    } else if (s instanceof XABSLBehavior.Symbol.Decimal) {
                        text.append(((XABSLBehavior.Symbol.Decimal) s).value);
                    } else if (s instanceof XABSLBehavior.Symbol.Enum) {
                        text.append(((XABSLBehavior.Symbol.Enum) s).value);
                    }
                }

            } else if (n.getUserObject() instanceof XABSLBehavior.Symbol) {
                XABSLBehavior.Symbol s = (XABSLBehavior.Symbol) n.getUserObject();
                color = Color.DARK_GRAY;
                font = ITALIC_FONT;

                text.append("@");
                text.append(s.name);
                text.append("=");
                if (s instanceof XABSLBehavior.Symbol.Boolean) {
                    text.append(((XABSLBehavior.Symbol.Boolean) s).value);
                } else if (s instanceof XABSLBehavior.Symbol.Decimal) {
                    text.append(((XABSLBehavior.Symbol.Decimal) s).value);
                } else if (s instanceof XABSLBehavior.Symbol) {
                    text.append(((XABSLBehavior.Symbol.Enum) s).value);
                }
            } else {
                text.append(n.getUserObject().toString());
            }

            JLabel label = new JLabel(text.toString());
            label.setForeground(color);
            label.setFont(font);
            return label;
        } else {
            return new JLabel("null");
        }
    }
}
