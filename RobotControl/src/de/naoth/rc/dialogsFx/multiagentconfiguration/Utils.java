package de.naoth.rc.dialogsFx.multiagentconfiguration;

import de.naoth.rc.dialogsFx.multiagentconfiguration.components.RequestTreeItem;
import de.naoth.rc.dialogsFx.multiagentconfiguration.components.TreeNode;
import de.naoth.rc.core.messages.Messages;
import java.io.File;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.TreeMap;
import java.util.function.BiConsumer;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.scene.control.TreeItem;
import javafx.stage.FileChooser;
import javafx.stage.Window;

/**
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class Utils
{
    public static TreeMap<String, TreeNode> global_debug_requests = new TreeMap<>();
    public static TreeMap<String, TreeNode> global_modules = new TreeMap<>();
    public static ObservableList<String> global_agent_list = FXCollections.observableArrayList();
    public static TreeMap<String, TreeItem<Parameter>> global_parameters = new TreeMap<>();
    public static ObservableList<String> global_representations_list = FXCollections.observableArrayList();
    private static FileChooser behaviorFileDialog;
    
    public static void createDebugRequestTree(Messages.DebugRequest request, TreeNode<String> root, BiConsumer<String, Boolean> debugRequest)
    {
        String root_name = root.getValue();
        
        if(!global_debug_requests.containsKey(root_name)) {
            global_debug_requests.put(root_name, new TreeNode(root_name));
        }
        
        request.getRequestsList().forEach((r) -> {
            int pos = r.getName().lastIndexOf(':');
            String id = root_name + ":" + r.getName(); // the identifier of this debug request
            String name = r.getName().substring(pos+1);
            String path = r.getName().substring(0, pos);
            
            // retrieve (and if necessary create) global module item
            if(!global_debug_requests.containsKey(id)) {
                TreeNode global_item = new TreeNode(name);
                getParentTreeItem(global_debug_requests.get(root_name), path, ":").getChildren().add(global_item);
                global_item.setSelected(r.getValue());
                global_debug_requests.put(id, global_item);
            }
            RequestTreeItem item = new RequestTreeItem(path, name);
            item.setRequest(r.getName());
            getParentTreeItem(root, path, ":").getChildren().add(item); // add this item to the module tree
            // bind to global module item
            // NOTE: can not use 'binding', because we're change the selected property and would causing a runtime exception otherwise
            global_debug_requests.get(id).selectedProperty().addListener((observable, oldValue, newValue) -> {
                item.setSelected(newValue);
            });
            // set the selected state AFTER adding it to its parent
            item.setSelected(r.getValue());
            // set the callback for (de-)activating this module (after selecting it!)
            item.selectedProperty().addListener((ob, o, n) -> { debugRequest.accept(r.getName(), n); });
        });
    }
    
    /**
     * Creates the module tree for the given modules list and appends it to the root node.
     * The debug request is used, when the node is selected.
     * 
     * @param list  modules list
     * @param root  the root node (Cognition, Motion)
     * @param debugRequest the debug request which should be used if a leaf node is selected
     */
    public static void createModulesTree(Messages.ModuleList list, TreeNode root, BiConsumer<String, Boolean> debugRequest)
    {
        Pattern p = Pattern.compile("([^\\s|]+).+/"+root.getValue()+"/(.+)\\/.+\\.h");
        
        // remember nodes that have the same name as its leaf node
        HashMap<String, TreeNode> removeableNodes = new HashMap<>();
        HashMap<String, TreeNode> removeableNodesGlobal = new HashMap<>();
        
        list.getModulesList().stream().map((m) -> {
            // create for each module a checkable tree item
            Matcher match = p.matcher(m.getName());
            if(match.matches()) {
                return new RequestTreeItem(match.group(2), match.group(1), m.getActive(), match.group(1));
            }
            return null;
        }).filter((m) -> {
            return m!=null; 
        }).sorted((m1, m2) -> {
            // sort the module tree items
            return m1.getPath().compareTo(m2.getPath());
        }).forEach((leaf) -> {
            List<String> parts = Arrays.asList(leaf.getPath().split("/"));
            
            // retrieve the parent node for the leaf and create the path to it, if necessary
            TreeNode parent = createTreePath(root, parts);
            TreeNode parentGlobal = createTreePath(global_modules.get(root.getValue()), parts);
            
            // remember this node as possible to remove
            if(parent.getValue().equals(leaf.getValue())) { removeableNodes.put(leaf.getPath(), parent); }
            if(parentGlobal.getValue().equals(leaf.getValue())) { removeableNodesGlobal.put(leaf.getPath(), parentGlobal); }

            // retrieve the global leaf or create a new, if necessary
            TreeNode leafGlobal = parentGlobal.getChildren(leaf.getValue());
            if(leafGlobal == null) {
                leafGlobal = new TreeNode(leaf.getValue());
                parentGlobal.getChildren().add(leafGlobal);
                leafGlobal.setSelected(leaf.active);
            }
            
            // bind to global module item
            // NOTE: can not use 'binding', because we're change the selected property and would causing a runtime exception otherwise
            leafGlobal.selectedProperty().addListener((observable, oldValue, newValue) -> {
                leaf.setSelected(newValue);
            });
            
            // add this item to the module tree
            parent.getChildren().add(leaf);
            // set the selected state AFTER adding it to its parent
            leaf.setSelected(leaf.active);
            // set the callback for (de-)activating this module
            leaf.selectedProperty().addListener((ob, o, n) -> { debugRequest.accept(leaf.getRequest(), n); });
        });
        
        // remove node, where the leaf and its parent have the same name and the leaf is the only child
        removeableNodes.forEach((t, u) -> {
            if(u.getChildren().size() == 1) {
                TreeNode parent = (TreeNode) u.getParent();
                int idx = parent.getChildren().indexOf(u);
                parent.getChildren().remove(idx);
                parent.getChildren().add(idx, u.getChildren().get(0));
            }
        });
        
        // remove node, where the leaf and its parent have the same name and the leaf is the only child
        removeableNodesGlobal.forEach((t, u) -> {
            if(u.getChildren().size() == 1) {
                TreeNode parent = (TreeNode) u.getParent();
                int idx = parent.getChildren().indexOf(u);
                parent.getChildren().remove(idx);
                parent.getChildren().add(idx, u.getChildren().get(0));
            }
        });
        
        expandSingleTreeNodes(root);
        expandSingleTreeNodes(global_modules.get((String) root.getValue()));
    }
    
    /**
     * Iterates from the root through the given path and returns the last node. If an intermediate
     * node doesn't exists, it is created.
     * 
     * @param root  the starting node
     * @param path  the path which should be iterated through
     * @return      the last node in in path
     */
    private static TreeNode createTreePath(TreeNode root, List<String> path) {
        TreeNode parent = root;
        for (int i = 0; i < path.size(); i++) {
            TreeNode current = parent.getChildren(path.get(i));
            if(current == null) {
                current = new TreeNode(path.get(i));
                parent.getChildren().add(current);
            }
            parent = current;
        }
        return parent;
    }
    
    private static TreeItem<String> getParentTreeItem(TreeNode root, String path, String sep) {
        TreeNode current_root = root;
        for (String part : path.split(sep)) {
            if(!current_root.hasChildren(part)) {
                TreeNode treePartNew = new TreeNode(part);
                current_root.getChildren().add(treePartNew);
                current_root = treePartNew;
            } else {
                current_root = current_root.getChildren(part);
            }
        }
        return current_root;
    }

    private static void expandSingleTreeNodes(TreeItem root) {
        TreeItem current = root;
        // expand root nodes or nodes with children and no siblings
        while ((current.getParent() == null || current.getParent().getValue() == null || current.getParent().getChildren().size() == 1) && current.getChildren().size() > 0) {
            current.setExpanded(true);
            current = (TreeItem) current.getChildren().get(0);
        }
    }
    
    public static TreeItem createModulesCognition(Messages.ModuleList list, BiConsumer<String, Boolean> moduleRequest) {
        TreeNode cognition_root = new TreeNode("Cognition");
        createModulesTree(list, cognition_root, moduleRequest);
        cognition_root.setExpanded(true);
        return cognition_root;
    }
    
    public static TreeItem createModulesMotion(Messages.ModuleList list, BiConsumer<String, Boolean> moduleRequest) {
        TreeNode motion_root = new TreeNode("Motion");
        createModulesTree(list, motion_root, moduleRequest);
        motion_root.setExpanded(true);
        return motion_root;
    }

    public static File showBehaviorFileDialog(Window owner) {
        if(behaviorFileDialog == null) {
            File defaultDirectory = new File("../../NaoTHSoccer/Config/");
            if(!defaultDirectory.isDirectory()) { defaultDirectory = new File(System.getProperty("user.home")); }
            
            behaviorFileDialog = new FileChooser();
            behaviorFileDialog.setTitle("Open Behavior File");
            behaviorFileDialog.setInitialFileName("behavior-ic.dat");
            behaviorFileDialog.setInitialDirectory(defaultDirectory);
            behaviorFileDialog.getExtensionFilters().addAll(
                    new FileChooser.ExtensionFilter("Behavior Files", "*.dat"),
                    new FileChooser.ExtensionFilter("All Files", "*.*"));
        }
        return behaviorFileDialog.showOpenDialog(owner);
    }
    
    /**
     * Creates a list of expanded nodes starting from t.
     * 
     * @param t the starting tree node
     * @return  a list of expanded nodes, starting from node t
     */
    public static List<String> getExpandedNodes(TreeItem<String> t) {
        return getExpandedNodes(t, "");
    }
    
    /**
     * Creates a list of expanded nodes
     * 
     * @param t      the starting tree node
     * @param parent the path to the starting tree node
     * @return       a list of expanded nodes, starting from node t
     */
    private static List<String> getExpandedNodes(TreeItem<String> t, String parent) {
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
     * Expands all nodes on the path starting from the root node.
     * 
     * @param root  the root node to start from
     * @param paths the paths, which should be expended
     */
    public static void expandNodes(TreeNode root, List<String> paths) {
        paths.forEach((p) -> {
            String[] parts = p.split("/");
            TreeNode current_root = root;
            for (String part : parts) {
                if(current_root.hasChildren(part)) {
                    current_root = current_root.getChildren(part);
                    current_root.setExpanded(true);
                } else {
                    break; // got an unknown node
                }
            }
        });
    }
}
