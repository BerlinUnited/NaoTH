package de.naoth.rc.dialogs.multiagentconfiguration.ui;

import java.util.HashMap;
import javafx.collections.ListChangeListener;
import javafx.scene.control.CheckBoxTreeItem;
import javafx.scene.control.TreeItem;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class TreeNode extends CheckBoxTreeItem<String>
{
    private final HashMap<String, TreeNode> children = new HashMap<>();

    public TreeNode() {
        this("root");
    }
    
    public TreeNode(String name) {
        super(name);
        
        getChildren().addListener((ListChangeListener.Change<? extends TreeItem<String>> c) -> {
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

    public TreeNode getChildren(String name) {
        return children.get(name);
    }
    
    public boolean hasChildren(String name) {
        return children.containsKey(name);
    }
}
