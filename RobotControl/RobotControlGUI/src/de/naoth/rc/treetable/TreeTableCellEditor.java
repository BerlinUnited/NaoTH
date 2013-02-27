/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc.treetable;

import java.awt.Component;
import java.awt.event.MouseEvent;
import java.util.EventObject;
 
import javax.swing.AbstractCellEditor;
import javax.swing.JTable;
import javax.swing.JTree;
import javax.swing.table.TableCellEditor;
 
public class TreeTableCellEditor extends AbstractCellEditor implements TableCellEditor {
 
    private JTree tree;
    private JTable table;
 
    public TreeTableCellEditor(JTree tree, JTable table) {
        this.tree = tree;
        this.table = table;
    }
 
    public Component getTableCellEditorComponent(JTable table, Object value, boolean isSelected, int r, int c) {
        return tree;
    }
 
    public boolean isCellEditable(EventObject e) {
        if (e instanceof MouseEvent) 
        {
            MouseEvent me = (MouseEvent) e;
            int doubleClick = 2;
            for(int i = 0; i < table.getColumnCount(); i++)
            {
                if(table.getColumnClass(i).equals(TreeTableModel.class))
                {
                    // delegate the event to the tree in its own coordinates
                    MouseEvent newME = new MouseEvent(
                            tree, me.getID(), me.getWhen(),
                            me.getModifiers(), me.getX() - table.getCellRect(0, i, true).x, 
                            me.getY(), doubleClick, me.isPopupTrigger());
                    tree.dispatchEvent(newME);
                }
            }
        }
        return false;
    }
 
    @Override
    public Object getCellEditorValue() {
        return null;
    }
 
}
