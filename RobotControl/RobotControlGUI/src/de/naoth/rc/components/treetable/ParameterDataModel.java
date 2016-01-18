/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc.components.treetable;

import java.util.ArrayList;
import java.util.List;
 
public class ParameterDataModel extends AbstractTreeTableModel 
{
    // Spalten Name.
    static protected String[] columnNames = { "Name", "Value" };
 
    // Spalten Typen.
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
        for(int i = 0; i < nodes.length; i++)
        {
            ParameterDataNode matchingNode = null;
            List<ParameterDataNode> childList = current.getChildren();
            
            for(ParameterDataNode child: childList)
            {
                if(nodes[i].equals(child.getName()))
                {
                    matchingNode = child;
                    break;
                }
            }

            if(matchingNode == null)
            {
                // add a new one
                matchingNode = new ParameterDataNode(nodes[i]);
                childList.add(matchingNode);
            }
            current = matchingNode;
        }
        
        return current;
    }//end insertPath
    
    
    @Override
    public Object getValueAt(Object node, int column) {
        switch (column) {
        case 0:
            return ((ParameterDataNode) node).getName();
        case 1:
            return ((ParameterDataNode) node).value;
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
        case 1:
            ((ParameterDataNode) node).value = aValue;
            break;
        default:
            break;
        }
    }
    
    
    public static interface ValueChangedListener
    {
        abstract void valueChanged(double value);
    }
    
    static public class ParameterDataNode 
    {
        public Object value = null;
        
        private final String name;
        private final List<ParameterDataNode> children;
        public ValueChangedListener enabledListener;
 
        public ParameterDataNode(String name) {
            this.name = name;
            this.children = new ArrayList<ParameterDataNode>();
        }
        
        public void setValue(double v) {
            value = new Double(v);
        }
        
        public void setValue(boolean v) {
            value = new Boolean(v);
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
