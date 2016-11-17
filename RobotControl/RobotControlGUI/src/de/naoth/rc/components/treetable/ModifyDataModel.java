/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc.components.treetable;

import java.util.ArrayList;
import java.util.List;
 
public class ModifyDataModel extends AbstractTreeTableModel 
{
    // Spalten Name.
    static protected String[] columnNames = { "Modify", "Name", "Value" };
 
    // Spalten Typen.
    static protected Class<?>[] columnTypes = { Boolean.class, TreeTableModel.class, Double.class };
 
    public ModifyDataModel() {
        super(new ModifyDataNode("Modify"));
    }
 
    @Override
    public Object getChild(Object parent, int index) {
        return ((ModifyDataNode) parent).getChildren().get(index);
    }
 
    @Override
    public int getChildCount(Object parent) {
        return ((ModifyDataNode) parent).getChildren().size();
    }
 
    @Override
    public int getColumnCount() {
        return columnNames.length;
    }
 
    @Override
    public String getColumnName(int column) {
        return columnNames[column];
    }
 
    @Override
    public Class<?> getColumnClass(int column) {
        return columnTypes[column];
    }
 
    @Override
    public boolean isCellEditable(Object node, int column) {
        return true; // Important to activate TreeExpandListener
    }
    
    public ModifyDataNode insertPath(String path, char seperator)
    {
        String[] nodes = path.split("" + seperator);
        
        ModifyDataNode current = (ModifyDataNode)getRoot();
        ArrayList<Object> current_path = new ArrayList<>();
        current_path.add(current);
        
        for(String s: nodes)
        {
            ModifyDataNode matchingNode = null;
            List<ModifyDataNode> childList = current.getChildren();
            
            for(ModifyDataNode child: childList)
            {
                if(s.equals(child.getName()))
                {
                    matchingNode = child;
                    break;
                }
            }

            if(matchingNode == null)
            {
                // add a new one
                matchingNode = new ModifyDataNode(s);
                childList.add(matchingNode);
                fireTreeNodesInserted(this, current_path.toArray(), new int[]{childList.size()-1}, new Object[]{matchingNode});
            }
            current = matchingNode;
            current_path.add(current);
        }
        
        return current;
    }//end insertPath
    
    
    @Override
    public Object getValueAt(Object node, int column) {
        switch (column) {
        case 1:
            return ((ModifyDataNode) node).getName();
        case 0:
            return ((ModifyDataNode) node).enabled;
        case 2:
            return ((ModifyDataNode) node).value;
        default:
            break;
        }
        return null;
    }

    @Override
    public void setValueAt(Object aValue, Object node, int column) {
        if(aValue == null)
            return;
        
        switch (column) {
        case 0:
            ((ModifyDataNode) node).setEnabled((Boolean) aValue);
            break;
        case 2:
            ((ModifyDataNode) node).setValue((Double)aValue);
            break;
        default:
            break;
        }
    }
    
    
    public static interface ValueChangedListener
    {
        abstract void valueChanged(boolean enabled, double value);
    }
    
    static public class ModifyDataNode 
    {
        public Boolean enabled = null;
        public Double value = null;
        
        private final String name;
        private final List<ModifyDataNode> children;
        public ValueChangedListener enabledListener;
 
        public void setEnabled(boolean v)
        {
            enabled = v;
            if(enabledListener != null) 
                enabledListener.valueChanged(enabled, value);
        }
        
        public void setValue(double v)
        {
            enabled = true;
            value = v;
            if(enabledListener != null) 
                enabledListener.valueChanged(enabled, value);
        }
                
        
        public ModifyDataNode(String name) {
            this.name = name;
            this.children = new ArrayList<ModifyDataNode>();
        }
 
        public String getName() {
            return name;
        }

        public List<ModifyDataNode> getChildren() {
            return children;
        }
        
        @Override
        public String toString() {
            return name;
        }
    }
}
