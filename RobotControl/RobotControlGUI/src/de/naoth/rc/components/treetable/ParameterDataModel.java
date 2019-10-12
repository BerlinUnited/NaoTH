/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc.components.treetable;

import java.util.ArrayList;
import java.util.List;
 
public class ParameterDataModel extends AbstractTreeTableModel 
{
    static protected String[] columnNames = { "Name", "Value" };
    static protected Class<?>[] columnTypes = { TreeTableModel.class, Object.class };
 
    public ParameterDataModel() {
        super(new ParameterDataNode("Parameter"));
    }
    
    public ParameterDataModel(String rootName) {
        super(new ParameterDataNode(rootName));
    }
 
    @Override
    public Object getChild(Object parent, int index) {
        return ((ParameterDataNode) parent).getChildren().get(index);
    }
 
    @Override
    public int getChildCount(Object parent) {
        return ((ParameterDataNode) parent).getChildren().size();
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
    
    public ParameterDataNode insertPath(String path, String separator)
    {
        String[] nodes = path.split(separator);
        
        ParameterDataNode current = (ParameterDataNode)getRoot();
        ArrayList<Object> current_path = new ArrayList<>();
        current_path.add(current);
        
        for(String s: nodes)
        {
            ParameterDataNode matchingNode = null;
            List<ParameterDataNode> childList = current.getChildren();
            
            for(ParameterDataNode child: childList)
            {
                if(s.equals(child.getName()))
                {
                    matchingNode = child;
                    break;
                }
            }

            // add a new one
            if(matchingNode == null)
            {
                matchingNode = new ParameterDataNode(s);
                childList.add(matchingNode);
                fireTreeNodesInserted(this, current_path.toArray(), new int[]{childList.size()-1}, new Object[]{matchingNode});
            }
            
            current = matchingNode;
            current_path.add(current);
        }
        
        return current;
    }//end insertPath
    
    Object bb = null;
    
    @Override
    public Object getValueAt(Object node, int column) {
        switch (column) {
            case 0: return ((ParameterDataNode) node).getName();
            case 1: return ((ParameterDataNode) node).value;
            default: break;
        }
        return null;
    }

    @Override
    public void setValueAt(Object aValue, Object node, int column) {
        if(aValue == null) {
            return;
        }
        
        switch (column) {
            case 1: ((ParameterDataNode) node).setValue(aValue);
            break;
        default:
            break;
        }
    }
    
    public static interface ValueChangedListener
    {
        abstract void valueChanged(Object value);
    }
    
    static public class ParameterDataNode 
    {
        public Object value = null;
        
        private final String name;
        private final List<ParameterDataNode> children = new ArrayList<ParameterDataNode>();
        public ValueChangedListener listener;
 
        public ParameterDataNode(String name) {
            this.name = name;
        }
        
        public void setValue(Object v) {
            value = v;
            if(listener != null) {
                listener.valueChanged(value);
            }
        }
        
        public String getName() {
            return name;
        }
  
        public List<ParameterDataNode> getChildren() {
            return children;
        }
        
        @Override
        public String toString() {
            return name;
        }
    }
}
