package de.naoth.rc.dialogs.multiagentconfiguration;

import de.naoth.rc.dialogs.multiagentconfiguration.ui.RequestTreeItem;
import de.naoth.rc.messages.Messages;
import java.util.TreeMap;
import java.util.function.BiConsumer;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.util.stream.Collectors;
import javafx.beans.value.ChangeListener;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.collections.transformation.FilteredList;
import javafx.scene.control.CheckBoxTreeItem;
import javafx.scene.control.TreeItem;

/**
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class Utils
{
    public static TreeMap<String, TreeItem> global_debug_requests = new TreeMap<>();
    public static TreeMap<String, TreeItem> global_modules = new TreeMap<>();
    public static ObservableList<String> global_agent_list = FXCollections.observableArrayList();
    public static TreeMap<String, TreeItem<Parameter>> global_parameters = new TreeMap<>();
    
    
    public static void createDebugRequestTree(Messages.DebugRequest request, TreeItem root, BiConsumer<String, Boolean> debugRequest) {
        
        String root_name = (String) root.getValue();
        
        if(!global_debug_requests.containsKey(root_name)) {
            global_debug_requests.put(root_name, new CheckBoxTreeItem<>(root_name));
        }
        
        request.getRequestsList().forEach((r) -> {
            int pos = r.getName().lastIndexOf(':');
            String id = root_name + ":" + r.getName(); // the identifier of this debug request
            String name = r.getName().substring(pos+1);
            String path = r.getName().substring(0, pos);
            
            // retrieve (and if necessary create) global module item
            if(!global_debug_requests.containsKey(id)) {
                CheckBoxTreeItem<String> global_item = new CheckBoxTreeItem<>(name);
                getParentTreeItem(global_debug_requests.get(root_name), path, ":").getChildren().add(global_item);
                global_item.setSelected(r.getValue());
                global_debug_requests.put(id, global_item);
            }
            RequestTreeItem item = new RequestTreeItem(path, name);
            item.setRequest(r.getName());
            getParentTreeItem(root, path, ":").getChildren().add(item); // add this item to the module tree
            // bind to global module item
            // NOTE: can not use 'binding', because we're change the selected property and would causing a runtime exception otherwise
            ((CheckBoxTreeItem)global_debug_requests.get(id)).selectedProperty().addListener((observable, oldValue, newValue) -> {
                item.setSelected(newValue);
            });
            // set the selected state AFTER adding it to its parent
            item.setSelected(r.getValue());
            // set the callback for (de-)activating this module (after selecting it!)
            item.selectedProperty().addListener((ob, o, n) -> { debugRequest.accept(r.getName(), n); });
        });
    }
    
    public static TreeItem createDebugRequestCognition(Messages.DebugRequest request, BiConsumer<String, Boolean> debugRequest) {
        CheckBoxTreeItem<String> cognition_root = new CheckBoxTreeItem<>("Cognition");
        createDebugRequestTree(request, cognition_root, debugRequest);
        cognition_root.setExpanded(true);
        return cognition_root;
    }
    
    public static TreeItem createDebugRequestMotion(Messages.DebugRequest request, BiConsumer<String, Boolean> debugRequest) {
        CheckBoxTreeItem<String> motion_root = new CheckBoxTreeItem<>("Motion");
        createDebugRequestTree(request, motion_root, debugRequest);
        motion_root.setExpanded(true);
        return motion_root;
    }
    
    public static void createModulesTree(Messages.ModuleList list, CheckBoxTreeItem root, BiConsumer<String, Boolean> debugRequest) {
        
        String root_name = (String) root.getValue();
        Pattern p = Pattern.compile("([^\\s|]+).+/"+root.getValue()+"/(.+)\\/.+\\.h");
        /*
        list.getModulesList().stream().collect(Collectors.toMap(
            (m) -> { return m.getName(); }, 
            (m)->{ return m.getActive(); }
        )).entrySet().stream().s;
        */
        list.getModulesList().stream().map((m) -> {
            // create for each module a checkable tree item
            Matcher match = p.matcher(m.getName());
            if(match.matches()) {
                String name = match.group(1);
                String path = match.group(2);
                if(path.endsWith(name)) {
                    path = path.substring(0, path.length()-name.length()-1);
                }
                return new RequestTreeItem(path, name, m.getActive(), name);
            }
            return null;
        }).filter((m) -> {
            return m!=null; 
        }).sorted((m1, m2) -> {
            // sort the module tree items
            return m1.getPath().compareTo(m2.getPath());
        }).forEach((m) -> {
            // add module items and their respective tree path
            // the identifier of this module
            String id = root_name + "/" + m.getPath() + "/" + m.getValue();
            // retrieve (and if necessary create) global module item
            if(!global_modules.containsKey(id)) {
                CheckBoxTreeItem<String> global_item = new CheckBoxTreeItem<>(m.getValue());
                getParentTreeItem(global_modules.get(root_name), m.getPath(), "/").getChildren().add(global_item);
                global_item.setSelected(m.active);
                global_modules.put(id, global_item);
            }
            // bind to global module item
            // NOTE: can not use 'binding', because we're change the selected property and would causing a runtime exception otherwise
            ((CheckBoxTreeItem)global_modules.get(id)).selectedProperty().addListener((observable, oldValue, newValue) -> {
                m.setSelected(newValue);
            });
            // add this item to the module tree
            //getParentTreeItem(root, m.getPath(), "/").getChildren().add(m);
            addTreeItem(root, m, "/");
            // set the selected state AFTER adding it to its parent
            m.setSelected(m.active);
            // set the callback for (de-)activating this module
            m.selectedProperty().addListener((ob, o, n) -> { debugRequest.accept(m.getRequest(), n); });
        });
        
        expandSingleTreeNodes(root);
        expandSingleTreeNodes(global_modules.get((String) root.getValue()));
    }
    
    private static void addTreeItem(TreeItem<String> root, RequestTreeItem item, String sep) {
        TreeItem<String> current_root = root;
        for (String part : item.getPath().split(sep)) {
            FilteredList<TreeItem<String>> treePart = current_root.getChildren().filtered((m) -> { return m.getValue().equals(part); });
            if(treePart.isEmpty()) {
                CheckBoxTreeItem<String> treePartNew = new CheckBoxTreeItem<>(part);
                current_root.getChildren().add(treePartNew);
                current_root = treePartNew;
            } else {
                current_root = treePart.get(0);
            }
        }
        if (current_root instanceof RequestTreeItem) {
            TreeItem<String> current_root_parent = current_root.getParent();
            int current_root_index = current_root_parent.getChildren().indexOf(current_root);
            current_root_parent.getChildren().remove(current_root_index);
            CheckBoxTreeItem<String> current_root_new = new CheckBoxTreeItem<>(current_root.getValue());
            current_root_parent.getChildren().add(current_root_index, current_root_new);
            current_root_new.getChildren().add(current_root);
            current_root = current_root_new;
        }
        current_root.getChildren().add(item);
    }
    
    private static TreeItem<String> getParentTreeItem(TreeItem<String> root, String path, String sep) {
        TreeItem<String> current_root = root;
        for (String part : path.split(sep)) {
            FilteredList<TreeItem<String>> treePart = current_root.getChildren().filtered((m) -> { return m.getValue().equals(part); });
            if(treePart.isEmpty()) {
                CheckBoxTreeItem<String> treePartNew = new CheckBoxTreeItem<>(part);
                current_root.getChildren().add(treePartNew);
                current_root = treePartNew;
            } else {
                current_root = treePart.get(0);
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
        CheckBoxTreeItem<String> cognition_root = new CheckBoxTreeItem<>("Cognition");
        createModulesTree(list, cognition_root, moduleRequest);
        cognition_root.setExpanded(true);
        return cognition_root;
    }
    
    public static TreeItem createModulesMotion(Messages.ModuleList list, BiConsumer<String, Boolean> moduleRequest) {
        CheckBoxTreeItem<String> motion_root = new CheckBoxTreeItem<>("Motion");
        createModulesTree(list, motion_root, moduleRequest);
        motion_root.setExpanded(true);
        return motion_root;
    }
}
