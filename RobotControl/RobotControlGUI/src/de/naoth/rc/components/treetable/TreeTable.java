/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc.components.treetable;

import java.awt.Dimension;
 
import javax.swing.JTable;
import javax.swing.JTree;
import javax.swing.table.TableCellEditor;
import javax.swing.table.TableCellRenderer;
import javax.swing.tree.TreePath;
 
public class TreeTable extends JTable {
 
    private final TreeTableCellRenderer tree;
    private final AbstractTreeTableModel model;
    
    public JTree getTree() {
        return this.tree;
    }
    
    public TreeTable(AbstractTreeTableModel treeTableModel) {
        super();
 
        // JTree erstellen.
        model = treeTableModel;
        tree = new TreeTableCellRenderer(this, treeTableModel);
        tree.setShowsRootHandles(true);
        tree.setRootVisible(false);
        //tree.setEditable(true);
        tree.setScrollsOnExpand(true);
        
        // Modell setzen.
        super.setModel(new TreeTableModelAdapter(treeTableModel, tree));
         
        // Gleichzeitiges Selektieren fuer Tree und Table.
        TreeTableSelectionModel treeSelectionModel = new TreeTableSelectionModel();
        tree.setSelectionModel(treeSelectionModel); //For the tree
        setSelectionModel(treeSelectionModel.getListSelectionModel()); //For the table
 
         
        // Renderer fuer den Tree.
        setDefaultRenderer(TreeTableModel.class, tree);
        // Editor fuer die TreeTable
        setDefaultEditor(TreeTableModel.class, new TreeTableCellEditor(tree, this));
        
        //setDefaultRenderer(Boolean.class, new MyTableCellRenderer());
        //setDefaultEditor(Boolean.class, new MyTableCellEditor());
        
        // Kein Grid anzeigen.
        setShowGrid(false);
 
        // Keine Abstaende.
        setIntercellSpacing(new Dimension(0, 0));
    }
    
    public void expandRoot() {
        tree.expandPath(new TreePath(model.getRoot()));
        tree.revalidate();
    }
    
    public void setModel(AbstractTreeTableModel treeTableModel)
    {
        super.setModel(new TreeTableModelAdapter(treeTableModel, tree));
    }
    
    @Override
    public TableCellEditor getCellEditor(int row, int column)  
    {  
        Object value = super.getValueAt(row, column);
        if (value instanceof Boolean) {  
            return getDefaultEditor(Boolean.class);  
        }
        if (value instanceof Double) {  
            return getDefaultEditor(Double.class);  
        }  
        if (value instanceof Integer) {  
             return getDefaultEditor(Integer.class);  
        }  
        // no special case  
        return super.getCellEditor(row, column);  
    }  
 
 
    @Override
    public TableCellRenderer getCellRenderer(int row, int column)
    {  
        Object value = super.getValueAt(row, column);
        if (value instanceof Boolean) {  
            return getDefaultRenderer(Boolean.class);  
        }  
        if (value instanceof Double) {  
            return getDefaultRenderer(Double.class);  
        }  
        if (value instanceof Integer) {  
             return getDefaultRenderer(Integer.class);  
        }  
        // no special case  
        return super.getCellRenderer(row, column);  
    }
}
