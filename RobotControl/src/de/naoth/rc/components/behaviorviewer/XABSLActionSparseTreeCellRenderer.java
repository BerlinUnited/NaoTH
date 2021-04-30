/*
 * 
 */
package de.naoth.rc.components.behaviorviewer;

import de.naoth.rc.components.behaviorviewer.model.EnumType;
import de.naoth.rc.components.behaviorviewer.model.Symbol;
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

    JLabel label = new JLabel("null");
    StringBuilder text = new StringBuilder();
    
    private static final Color DARK_GREEN = new Color(0, 128, 0);
    
    private final Font PLAIN_FONT;
    private final Font BOLD_FONT;
    private final Font ITALIC_FONT;
    
    public XABSLActionSparseTreeCellRenderer(int fontSize) {
        this.PLAIN_FONT = new Font("Sans Serif", Font.PLAIN, fontSize);
        this.BOLD_FONT = new Font("Sans Serif", Font.BOLD, fontSize);
        this.ITALIC_FONT = new Font("Sans Serif", Font.ITALIC, fontSize);
    }
    
    private Component makeCellRenderer(Color color, Font font, String text) {
        label.setText(text);
        label.setForeground(color);
        label.setFont(font);
        return label;
    }
    
    @Override
    public Component getTreeCellRendererComponent(JTree tree, Object value, boolean selected, boolean expanded, boolean leaf, int row, boolean hasFocus) {
        if (value != null) {
            DefaultMutableTreeNode n = (DefaultMutableTreeNode) value;

            // default values for color and font
            Color color = Color.black;
            Font font = PLAIN_FONT;
            text.setLength(0);
            
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
                    if(oe.activeState < oe.option.states.size()) { // to prevent errors on new/old states!
                        text.append(oe.option.states.get(oe.activeState).name);
                    } else {
                        text.append("[UNKNOWN_STATE]");
                    }
                    text.append(" [");
                    text.append(oe.stateTime);
                    text.append(" ms]");
                } else if (a instanceof XABSLAction.BasicBehaviorExecution) {
                    text.append(((XABSLAction.BasicBehaviorExecution) a).name);
                } else if (a instanceof XABSLAction.SymbolAssignment) {
                    Symbol s = ((XABSLAction.SymbolAssignment) a).symbol;

                    text.append(s.name);
                    text.append('=');
                    if (s instanceof Symbol.Boolean) {
                        text.append(((Symbol.Boolean) s).value);
                    } else if (s instanceof Symbol.Decimal) {
                        text.append(((Symbol.Decimal) s).value);
                    } else if (s instanceof Symbol.Enum) {
                        Symbol.Enum e = (Symbol.Enum) s;
                        EnumType.Element etypeElement =  e.enumType.elements.get(e.value);
                        text.append(etypeElement.name);
                    }
                }

            } else if (n.getUserObject() instanceof Symbol) {
                Symbol s = (Symbol) n.getUserObject();
                color = Color.DARK_GRAY;
                font = ITALIC_FONT;

                text.append('@');
                text.append(s.name);
                text.append('=');
                if (s instanceof Symbol.Boolean) {
                    text.append(((Symbol.Boolean) s).value);
                } else if (s instanceof Symbol.Decimal) {
                    text.append(((Symbol.Decimal) s).value);
                } else if (s instanceof Symbol) {
                    text.append(((Symbol.Enum) s).value);
                }
            } else {
                text.append(n.getUserObject().toString());
            }

            return makeCellRenderer(color, font, text.toString());
        } else {
            return makeCellRenderer(Color.red, PLAIN_FONT, "null");
        }
    }
}
