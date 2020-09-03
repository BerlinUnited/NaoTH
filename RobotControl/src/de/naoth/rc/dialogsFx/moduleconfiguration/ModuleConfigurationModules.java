package de.naoth.rc.dialogsFx.moduleconfiguration;

import de.naoth.rc.componentsFx.TreeNodeCell;
import de.naoth.rc.componentsFx.TreeNodeItem;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import javafx.animation.FadeTransition;
import javafx.collections.ListChangeListener;
import javafx.fxml.FXML;
import javafx.scene.control.Label;
import javafx.scene.control.TreeView;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyCodeCombination;
import javafx.scene.input.KeyCombination;
import javafx.scene.input.KeyEvent;
import javafx.util.Duration;

/**
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class ModuleConfigurationModules
{
    /** The current module configuration */
    private ModuleConfiguration mConfig;
    
    @FXML private TreeView<String> moduleTree;
    @FXML private Label notice;
    
    /** Some shortcuts */
    private final KeyCombination shortcutRefresh = new KeyCodeCombination(KeyCode.F5);
    private final KeyCombination shortcutEnableEnter = new KeyCodeCombination(KeyCode.ENTER);
    private final KeyCombination shortcutEnableSpace = new KeyCodeCombination(KeyCode.SPACE);
    
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
        moduleTree.setRoot(new TreeNodeItem<>());
        moduleTree.getRoot().setExpanded(true);
        
        // pre-set cognition and motion tree items
        TreeNodeItem cognition = new TreeNodeItem("Cognition");
        cognition.setExpanded(true);
        moduleTree.getRoot().getChildren().add(cognition);
        
        TreeNodeItem motion = new TreeNodeItem("Motion");
        motion.setExpanded(true);
        moduleTree.getRoot().getChildren().add(motion);

        // set the options for the fade-out animation
        fadeOut.setNode(notice);
        fadeOut.setFromValue(1.0);
        fadeOut.setToValue(0.0);
        fadeOut.setCycleCount(1);
        fadeOut.setAutoReverse(false);
        
        System.out.println(notice);
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
        }
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
            // clear the representation selection of the representation info list, if another module/representation is selected
            if (moduleTree.getSelectionModel().getSelectedItem() != null) {
                Object selection = moduleTree.getSelectionModel().getSelectedItem().getValue();
                if (selection != n) {
                    moduleTree.getSelectionModel().clearSelection();
                }
            }
        });
        
        // show flash message if it changed
        //notice.textProperty().bind(mConfig.getFlashMessageProperty());
        mConfig.getFlashMessageProperty().addListener((ob) -> {
            notice.setText(mConfig.getFlashMessageProperty().get());
            notice.setVisible(true);
            fadeOut.playFromStart();
        });
    }
    
    private void updateModulesTree() {
        // remove the old (sub-)tree
        moduleTree.getRoot().getChildren().forEach((t) -> {
            t.getChildren().clear();
        });
        
        // remember nodes that have the same name as its leaf node
        HashMap<String, TreeNodeItem> removeableNodes = new HashMap<>();
        
        // sort and create tree paths to the "module leafs"
        mConfig.getModulesProperty().stream().sorted((m1, m2) -> {
            return m1.getPath().compareToIgnoreCase(m2.getPath());
        }).forEachOrdered((m) -> {
            String path = m.getPath().substring(m.getPath().indexOf(m.getType()), m.getPath().lastIndexOf("/"));
            List<String> parts = Arrays.asList(path.split("/"));
            
            // retrieve the parent node for the leaf and create the path to it, if necessary
            TreeNodeItem parent = createTreePath(((TreeNodeItem)moduleTree.getRoot()).getChildren(m.getType()), parts);

            // remember this node as possible to remove
            if(parent.getValue().equals(m.getName())) { removeableNodes.put(m.getPath(), parent); }
            
            TreeNodeItem leaf = new TreeNodeItem(m);
            // add this item to the module tree
            parent.getChildren().add(leaf);
            // set the selected state AFTER adding it to its parent
            leaf.setSelected(m.isActive());
            // set the callback for (de-)activating this module
            leaf.selectedProperty().addListener((ob, o, n) -> { mConfig.enableModule(m, n); });
        });
        
        // remove node, where the leaf and its parent have the same name and the leaf is the only child
        removeableNodes.forEach((t, u) -> {
            if(u.getChildren().size() == 1) {
                TreeNodeItem parent = (TreeNodeItem) u.getParent();
                int idx = parent.getChildren().indexOf(u);
                parent.getChildren().remove(idx);
                parent.getChildren().add(idx, u.getChildren().get(0));
            }
        });
        
        // expand single tree paths (like "Cognition/Modules"
        moduleTree.getRoot().getChildren().forEach((t) -> {
            expandSingleTreeNodes((TreeNodeItem)t);
        });
        
        moduleTree.setDisable(false);
        // TODO: restore expanded paths
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
            TreeNodeItem current = parent.getChildren(path.get(i));
            if(current == null) {
                current = new TreeNodeItem(path.get(i));
                parent.getChildren().add(current);
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
        while ((current.getParent() == null || current.getParent().getValue() == null || current.getParent().getChildren().size() == 1) && current.getChildren().size() > 0) {
            current.setExpanded(true);
            current = (TreeNodeItem) current.getChildren().get(0);
        }
    }
}
