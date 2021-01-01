package de.naoth.rc.componentsFx;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.function.Predicate;
import javafx.beans.binding.Bindings;
import javafx.beans.binding.ObjectBinding;
import javafx.beans.property.ObjectProperty;
import javafx.beans.property.SimpleObjectProperty;
import javafx.collections.FXCollections;
import javafx.collections.ListChangeListener;
import javafx.collections.ObservableList;
import javafx.collections.transformation.FilteredList;
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
    /** The actual children of this TreeNodeItem */
    private final ObservableList<TreeItem<T>> sourceList = FXCollections.observableArrayList();
    /** The filtered children based on the predicate */
    private final FilteredList<TreeItem<T>> filteredList = new FilteredList<>(this.sourceList);
    /** The predicate to filter the source children list */
    private final ObjectProperty<TreeItemPredicate<T>> predicate = new SimpleObjectProperty<>();
    /** Container storing the references to the named children. */
    private final HashMap<T, TreeNodeItem> mapping = new HashMap<>();
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
       
        // sync the children: source -> filtered -> children
        Bindings.bindContent(getChildren(), filteredList);
        
        // keep track of added/removed children (callback)
        sourceList.addListener(createMappingListener());
        
        // update the predicate of the children and trigger the filter test, when
        // this (parent) predicate changes
        this.filteredList.predicateProperty().bind(createFilterPredicateBinding());
    }
    
    /**
     * Creates the listener to update the children names mapping.
     * 
     * @return the ListChangeListener mapping updater
     */
    private ListChangeListener<TreeItem<T>> createMappingListener() {
        return (ListChangeListener.Change<? extends TreeItem<T>> c) -> {
             while (c.next()) {
                if (c.wasAdded()) {
                    // add the names of the added nodes
                    c.getAddedSubList().forEach((a) -> { mapping.put(a.getValue(), (TreeNodeItem) a); });
                } else if(c.wasRemoved()) {
                    // remove the names of the removed nodes
                    c.getRemoved().forEach((r) -> { mapping.remove(r.getValue()); });
                }
            }
        };
    }
    
    /**
     * Returns the object binding to update and trigger the predicate of the children.
     * 
     * @return the object update binding
     */
    private ObjectBinding<Predicate<TreeItem<T>>> createFilterPredicateBinding()
    {
        return Bindings.createObjectBinding(() -> {
            Predicate<TreeItem<T>> p = child -> {
                // Set the predicate of child items to trigger filtering
                if (child instanceof TreeNodeItem) {
                    TreeNodeItem<T> filterableChild = (TreeNodeItem<T>) child;
                    filterableChild.setPredicate(this.predicate.get());
                }
                // If there is no predicate, keep this tree item
                if (this.predicate.get() == null) {
                    return true;
                }
                // If there are children, keep this tree item
                if (child.getChildren().size() > 0) {
                    return true;
                }
                // Intermediate nodes are not matched and hidden by default 
                // if they doesn't have visible children
                if (((TreeNodeItem)child).getSourceChildren().size() > 0) {
                    return false;
                }
                // Otherwise ask the TreeNodeItemPredicate
                return this.predicate.get().test(this, child.getValue());
            };
            return p;
        }, this.predicate);
    }

    /**
     * Returns the list of children that is backing the filtered list.
     *
     * @return underlying list of children
     */
    public ObservableList<TreeItem<T>> getSourceChildren() {
        return this.sourceList;
    }
    
    /**
     * An alias for "getChildren()" to better show the actual idea.
     * 
     * @return 
     */
    public ObservableList<TreeItem<T>> getViewChildren() {
        return getChildren();
    }

    /**
     * @return the predicate property
     */
    public final ObjectProperty<TreeItemPredicate<T>> predicateProperty() {
        return this.predicate;
    }

    /**
     * @return the predicate
     */
    public final TreeItemPredicate<T> getPredicate() {
        return this.predicate.get();
    }

    /**
     * Set the predicate
     *
     * @param predicate the predicate
     */
    public final void setPredicate(TreeItemPredicate<T> predicate) {
        this.predicate.set(predicate);
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
    public TreeNodeItem getChild(T name) {
        return mapping.get(name);
    }
    
    /**
     * Returns, whether a specific node with the name exists as child of this node.
     * @param name  child name to check
     * @return      true, if a child with the name exists, false otherwise
     */
    public boolean hasChild(T name) {
        return mapping.containsKey(name);
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
                    if(current_root.hasChild(part)) {
                        current_root = current_root.getChild(part);
                        current_root.setExpanded(true);
                    } else {
                        break; // got an unknown node
                    }
                }
            }
        });
    }
}
