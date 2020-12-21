package de.naoth.rc.dialogsFx.moduleconfiguration;

import de.naoth.rc.componentsFx.TreeNodeCell;
import de.naoth.rc.componentsFx.TreeNodeItem;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import javafx.animation.FadeTransition;
import javafx.beans.binding.Bindings;
import javafx.collections.ListChangeListener;
import javafx.fxml.FXML;
import javafx.scene.control.Label;
import javafx.scene.control.TextField;
import javafx.scene.control.TreeItem;
import javafx.scene.control.TreeView;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyCodeCombination;
import javafx.scene.input.KeyCombination;
import javafx.scene.input.KeyEvent;
import javafx.util.Duration;
import de.naoth.rc.componentsFx.TreeItemPredicate;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class ModuleConfigurationModules
{
    /** The current module configuration */
    private ModuleConfiguration mConfig;
    
    @FXML private TreeView<String> moduleTree;
    @FXML private Label notice;
    @FXML private TextField search;
    
    /** Some shortcuts */
    private final KeyCombination shortcutRefresh = new KeyCodeCombination(KeyCode.F5);
    private final KeyCombination shortcutEnableEnter = new KeyCodeCombination(KeyCode.ENTER);
    private final KeyCombination shortcutEnableSpace = new KeyCodeCombination(KeyCode.SPACE);
    private final KeyCombination shortcutDisableSearch = new KeyCodeCombination(KeyCode.ESCAPE);
    
    /** The animation for the flash messages */
    private final FadeTransition fadeOut = new FadeTransition(Duration.millis(2000));
    
    /**
     * Default constructor for the FXML loader.
     */
    public ModuleConfigurationModules() {}

    /**
     * Constructor for custom initialization.
     * 
     * @param config the current module configuration
     */
    public ModuleConfigurationModules(ModuleConfiguration config) {
        setModuleConfiguration(config);
    }
    
    /**
     * Sets the current module configuration and updates the internal binding.
     * 
     * @param config the new module configuration
     */
    public void setModuleConfiguration(ModuleConfiguration config) {
        this.mConfig = config;
        
        // The bindings can only be installed, when a module configuration object is present.
        updateBindings();
    }

    /**
     * Gets called, after the FXML file was loaded.
     */
    @FXML
    private void initialize() 
    {
        moduleTree.setCellFactory((p) -> new TreeNodeCell<>());
        moduleTree.setRoot(createModulesTree());
        moduleTree.getRoot().setExpanded(true);
        ((TreeNodeItem)moduleTree.getRoot()).predicateProperty().bind(Bindings.createObjectBinding(() -> {
            if (search.getText() == null || search.getText().isEmpty()) { return null; }
            return TreeItemPredicate.create(item -> item.toString().toLowerCase().contains(search.getText().toLowerCase()));
        }, search.textProperty()));

        // set the options for the fade-out animation
        fadeOut.setNode(notice);
        fadeOut.setFromValue(1.0);
        fadeOut.setToValue(0.0);
        fadeOut.setCycleCount(1);
        fadeOut.setAutoReverse(false);
    }
    
    /**
     * Is called, if the key pressed event is triggered by the tree view.
     * 
     * @param k the key (event), which triggered the event
     */
    @FXML
    private void fxListShortcuts(KeyEvent k) {
        // handle space/enter key press and toggle selected state
        if (shortcutEnableEnter.match(k) || shortcutEnableSpace.match(k)) {
            // HACK: make sure that a focused item is also selected
            //       especially the space key toggles the selected state, but
            //       keeps the focus on the item and therefore requires two keystrokes!
            TreeNodeItem i = (TreeNodeItem) moduleTree.getSelectionModel().getSelectedItem();
            if (i == null && moduleTree.getFocusModel().getFocusedItem() != null) {
                i = (TreeNodeItem) moduleTree.getFocusModel().getFocusedItem();
                moduleTree.getSelectionModel().select(i);
            }

            if (i != null && i.isLeaf()) {
                i.setSelected(!i.isSelected());
            }
        } else if (shortcutRefresh.match(k)) {
            this.mConfig.updateModules();
        } else if (shortcutDisableSearch.match(k)) {
            disableSearch();
        } else {
            TreeItem<String> selection = moduleTree.getSelectionModel().getSelectedItem();
            if (k.getCode() == KeyCode.BACK_SPACE) {
                // handle backspace key
                search.deletePreviousChar();
            } else if (!k.getText().isEmpty()) {
                // add - non-empty - character to the search field
                search.setVisible(true);
                search.appendText(k.getText());
            }
            moduleTree.getSelectionModel().select(selection);
        }
    }
    
    /**
     * Disables (hide) the search field and keep the current selection on the tree.
     */
    private void disableSearch() {
        TreeItem<String> selection = moduleTree.getSelectionModel().getSelectedItem();
        search.setVisible(false);
        search.clear();
        moduleTree.getSelectionModel().select(selection);
        moduleTree.scrollTo(moduleTree.getSelectionModel().getSelectedIndex());
    }

    /**
     * Installs the bindings to properties of the module configuration object.
     */
    private void updateBindings() {
        // update the module tree, if the modules list changes
        mConfig.getModulesProperty().addListener(new ListChangeListener() {
            @Override
            public void onChanged(ListChangeListener.Change change) {
                updateModulesTree();
            }
        });
        
        // set the active module (selection) and show the dependencies
        moduleTree.getSelectionModel().selectedItemProperty().addListener((ob, o, n) -> {
            // HACKY: java thinks the value is a string, so we must 'down cast' it
            //        to an object before we can compare it with the module type
            if (n != null && (Object)n.getValue() instanceof Module) {
                mConfig.getActiveProperty().set((Module)(Object)n.getValue());
            }
        });
        
        // set binding to the active module/representation
        mConfig.getActiveProperty().addListener((ob, o, n) -> {
            if (n instanceof Module) {
                TreeNodeItem node = findModuleInTree((Module) n);
                if (node != null && moduleTree.getSelectionModel().getSelectedItem() != node) {
                    moduleTree.getSelectionModel().select(node);
                    moduleTree.scrollTo(moduleTree.getRow(node.getParent()));
                }
            } else {
                moduleTree.getSelectionModel().clearSelection();
            }
        });
        
        // show flash message if it changed
        mConfig.getFlashMessageProperty().addListener((ob) -> {
            notice.setText(mConfig.getFlashMessageProperty().get());
            notice.setVisible(true);
            fadeOut.playFromStart();
        });
    }
    
    /**
     * Handles the update of the modules (sub-)tree.
     */
    private void updateModulesTree() {
        // do nothing, if the current modules list is empty
        if (mConfig.getModulesProperty().isEmpty()) {
            return;
        }
        
        // create the new tree, but update only cognition/motion subtrees
        TreeNodeItem root = createModulesTree();
        updateModulesSubtree("Cognition", root);
        updateModulesSubtree("Motion", root);

        moduleTree.setDisable(false);
    }
    
    /**
     * Updates a specific subtree of the modules tree.
     * 
     * @param type  the subtree to update
     * @param root  the root of the new tree
     */
    private void updateModulesSubtree(String type, TreeNodeItem root) {
        // check, if the subtree of the type should be updated
        if (root.getChild(type).getSourceChildren().size() > 0) {
            TreeNodeItem subtree = ((TreeNodeItem)moduleTree.getRoot()).getChild(type);
            // remember the expanded nodes of the subtree
            List<String> expanded = subtree.getExpandedNodes();
            // update subtree
            subtree.getSourceChildren().setAll(root.getChild(type).getSourceChildren());
            // restore expanded nodes
            subtree.expandNodes(expanded);
            // expand single tree paths (like "Cognition/Modules"
            expandSingleTreeNodes(subtree);
        }
    }
    
    /**
     * Creates the modules tree based on the module configuration list and returns
     * the root element of the tree.
     * 
     * @return the root of the (new) modules tree
     */
    private TreeNodeItem createModulesTree() {
        TreeNodeItem root = new TreeNodeItem();
        
        // pre-set cognition and motion tree items
        createRootTypeNodes(root, "Cognition");
        createRootTypeNodes(root, "Motion");
        
        // if set, use the module configuration
        if (mConfig != null) {
            // remember nodes that have the same name as its leaf node
            HashMap<String, TreeNodeItem> removeableNodes = new HashMap<>();

            // sort and create tree paths to the "module leafs"
            mConfig.getModulesProperty().stream().sorted((m1, m2) -> {
                return m1.getPath().compareToIgnoreCase(m2.getPath());
            }).forEachOrdered((m) -> {
                
                List<String> parts = splitPathIntoParts(m);
                if (parts == null) { return; }

                // retrieve the parent node for the leaf and create the path to it, if necessary
                TreeNodeItem parent = createTreePath(root.getChild(m.getType()), parts);

                // remember this node as possible to remove
                if(parent.getValue().equals(m.getName())) { removeableNodes.put(m.getPath(), parent); }

                TreeNodeItem leaf = new TreeNodeItem(m);
                // add this item to the module tree
                parent.getSourceChildren().add(leaf);
                // bind the modules active property to the checkbox for (de-)activating this module
                leaf.selectedProperty().bindBidirectional(m.getActiveProperty());
            });

            // remove node, where the leaf and its parent have the same name and the leaf is the only child
            removeableNodes.forEach((t, u) -> {
                if(u.getSourceChildren().size() == 1) {
                    TreeNodeItem parent = (TreeNodeItem) u.getParent();
                    int idx = parent.getSourceChildren().indexOf(u);
                    parent.getSourceChildren().remove(idx);
                    parent.getSourceChildren().add(idx, u.getSourceChildren().get(0));
                }
            });
        }
        
        return root;
    }
    
    /**
     * Creates the root node for a type (Cognition/Motion).
     * 
     * @param root  the actual root node
     * @param type  the name of the type (Cognition/Motion)
     */
    private void createRootTypeNodes(TreeNodeItem root, String type)
    {
        TreeNodeItem typeNode = new TreeNodeItem(type);
        typeNode.setExpanded(true);
        root.getSourceChildren().add(typeNode);
    }
    
    /**
     * Splits the modules path into the directory paths.
     * 
     * @param m the module
     * @return  the parts of the modules path
     */
    private List<String> splitPathIntoParts(Module m) 
    {
        // normalize path for windows and linux
        String normalizedPath = m.getPath().replaceAll("\\\\|/", ":");
        
        // retrieve the start/end of the path we want to show in the module tree
        int typeStart = normalizedPath.indexOf(m.getType());
        int fileStart = normalizedPath.lastIndexOf(":");

        // make sure we got valid start/end points
        if (typeStart == -1 || fileStart == -1 || fileStart <= typeStart) {
            System.err.println("Invalid module path: " + m.getPath());
            return null;
        }

        // retrieve the path we want to show in the module tree
        String path = normalizedPath.substring(typeStart, fileStart);
        return Arrays.asList(path.split(":"));
    }

    /**
     * Iterates from the root through the given path and returns the last node. If an intermediate
     * node doesn't exists, it is created.
     * 
     * @param root  the starting node
     * @param path  the path which should be iterated through
     * @return      the last node in in path
     */
    private static TreeNodeItem createTreePath(TreeNodeItem root, List<String> path) {
        TreeNodeItem parent = root;
        for (int i = 1; i < path.size(); i++) {
            TreeNodeItem current = parent.getChild(path.get(i));
            if(current == null) {
                current = new TreeNodeItem(path.get(i));
                parent.getSourceChildren().add(current);
            }
            parent = current;
        }
        return parent;
    }
    
    /**
     * Expands root nodes or nodes with children and no siblings.
     * 
     * @param root 
     */
    private void expandSingleTreeNodes(TreeNodeItem root) {
        TreeNodeItem current = root;
        while ((current.getParent() == null 
                || current.getParent().getValue() == null 
                || ((TreeNodeItem) current.getParent()).getSourceChildren().size() == 1)
                && current.getSourceChildren().size() > 0) {
            current.setExpanded(true);
            current = (TreeNodeItem) current.getSourceChildren().get(0);
        }
    }
 
    /**
     * Finds the given module in the module tree and returns the corresponding
     * TreeNodeItem, or null if the module was't found.
     * 
     * @param module the module to search for
     * @return the corresponding TreeNodeItem, or null if not found
     */
    private TreeNodeItem findModuleInTree(Module module)
    {
        // use the modules path to find the correct TreeNodeItem
        List<String> path = splitPathIntoParts(module);
        
        // find the parent tree item of the module
        TreeNodeItem current = (TreeNodeItem) moduleTree.getRoot();
        for (String p : path) {
            if (!current.hasChild(p)) {
                break;
            }
            current = current.getChild(p);
        }
        // return the actual TreeNodeItem of the module
        return current.getChild(module);
    }
}
