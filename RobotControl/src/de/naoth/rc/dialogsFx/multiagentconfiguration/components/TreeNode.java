package de.naoth.rc.dialogsFx.multiagentconfiguration.components;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import javafx.collections.ListChangeListener;
import javafx.scene.control.CheckBoxTreeItem;
import javafx.scene.control.TreeItem;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 * @param <T>
 */
public class TreeNode<T extends Object> extends CheckBoxTreeItem<T>
{
    private final HashMap<T, TreeNode> children = new HashMap<>();

    public TreeNode() {
        this(null);
    }
    
    public TreeNode(T name) {
        super(name);
        
        getChildren().addListener((ListChangeListener.Change<? extends TreeItem<T>> c) -> {
             while (c.next()) {
                if (c.wasAdded()) {
                    // add the names of the added nodes
                    c.getAddedSubList().forEach((a) -> { children.put(a.getValue(), (TreeNode) a); });
                } else if(c.wasRemoved()) {
                    // remove the names of the removed nodes
                    c.getRemoved().forEach((r) -> { children.remove(r.getValue()); });
                }
            }
        });
    }

    public TreeNode getChildren(T name) {
        return children.get(name);
    }
    
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

                TreeNode current_root = this;
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
