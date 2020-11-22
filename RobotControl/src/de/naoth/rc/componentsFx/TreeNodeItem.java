package de.naoth.rc.componentsFx;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import javafx.collections.ListChangeListener;
import javafx.scene.control.CheckBoxTreeItem;
import javafx.scene.control.TreeItem;

/**
 * Extended CheckBoxTreeItem class.
 * It keeps track of all added (and removed) children and adds the ability to
 * check and retrieve a specific child node based on its name.
 * In addition a list of expanded node paths can be created and also restored.
 * 
 * @author Philipp Strobel <philippstrobel@posteo.de>
 * @param <T>
 */
public class TreeNodeItem<T extends Object> extends CheckBoxTreeItem<T>
{
    /** Container storing the references to the named children. */
    private final HashMap<T, TreeNodeItem> children = new HashMap<>();
    /** The tooltip for this tree node. */
    private final String tooltip;

    public TreeNodeItem() {
        this(null, null);
    }
    
    public TreeNodeItem(T name) {
        this(name, null);
    }
    
    public TreeNodeItem(T name, String tooltip) {
        super(name);
        
        this.tooltip = tooltip;
        
        // keep track of added/removed children (callback)
        getChildren().addListener((ListChangeListener.Change<? extends TreeItem<T>> c) -> {
             while (c.next()) {
                if (c.wasAdded()) {
                    // add the names of the added nodes
                    c.getAddedSubList().forEach((a) -> { children.put(a.getValue(), (TreeNodeItem) a); });
                } else if(c.wasRemoved()) {
                    // remove the names of the removed nodes
                    c.getRemoved().forEach((r) -> { children.remove(r.getValue()); });
                }
            }
        });
    }
    
    /**
     * Returns the tooltip of this node
     * @return the tooltip of this node
     */
    public String getTooltip() {
        return tooltip;
    }

    /**
     * Returns the child node of the given name.
     * @param name  the name of the child
     * @return      the child node or null, if the name couldn't be found
     */
    public TreeNodeItem getChildren(T name) {
        return children.get(name);
    }
    
    /**
     * Returns, whether a specific node with the name exists as child of this node.
     * @param name  child name to check
     * @return      true, if a child with the name exists, false otherwise
     */
    public boolean hasChildren(T name) {
        return children.containsKey(name);
    }
    
    /**
     * Creates a list of expanded nodes starting from this node.
     * The nodes are separated by a '/'.
     * 
     * @return  a list of expanded nodes
     */
    public List<String> getExpandedNodes() {
        return getExpandedNodes(this, "");
    }

    /**
     * Creates a list of expanded nodes
     * 
     * @param t      the starting tree node
     * @param parent the path to the starting tree node
     * @return       a list of expanded nodes, starting from node t
     */
    private List<String> getExpandedNodes(TreeItem<T> t, String parent) {
        List<String> expanded = new ArrayList<>();
        if(t.isExpanded()) {
            String path = parent + (parent.isEmpty()?"":"/") + t.getValue();
            expanded.add(path);
            if(!t.isLeaf()) {
                t.getChildren().forEach((child) -> {
                    expanded.addAll(getExpandedNodes(child, path));
                });
            }
        }
        return expanded;
    }

    /**
     * Expands all nodes on the path starting from the this node.
     * 
     * @param paths the paths, which should be expended
     */
    public void expandNodes(List<String> paths) {
        paths.forEach((p) -> {
            String root = String.valueOf(this.getValue());
            if(p.startsWith(root)) {
                String[] parts = p.replaceFirst(root+"/?", "").split("/");

                TreeNodeItem current_root = this;
                current_root.setExpanded(true);

                for (String part : parts) {
                    if(current_root.hasChildren(part)) {
                        current_root = current_root.getChildren(part);
                        current_root.setExpanded(true);
                    } else {
                        break; // got an unknown node
                    }
                }
            }
        });
    }
}
