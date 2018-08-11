package de.naoth.rc.dialogs.multiagentconfiguration;

import de.naoth.rc.dialogs.multiagentconfiguration.ui.RequestTreeItem;
import de.naoth.rc.messages.Messages;
import java.util.TreeMap;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
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
    
    
    public static void createDebugRequestTree(Messages.DebugRequest request, TreeItem root, ChangeListener debugRequest) {
        
        String root_name = (String) root.getValue();
        
        TreeMap<String, TreeItem> items = new TreeMap<>();
        items.put(root_name, root);
        
        if(!global_debug_requests.containsKey(root_name)) {
            global_debug_requests.put(root_name, new CheckBoxTreeItem<>(root_name));
        }
        
        request.getRequestsList().forEach((t) -> {
//            System.out.println(t.getName() + " = " + t.getValue());
            String[] path = t.getName().split(":");
            String str_path = root_name;
            for (int i = 0; i < path.length; i++) {
                TreeItem parent = items.get(str_path);
                TreeItem globalParent = global_debug_requests.get(str_path);
                
                if(t.getValue() && !((CheckBoxTreeItem)parent).isIndeterminate()) {
                    try {
                        if(!((CheckBoxTreeItem)parent).selectedProperty().isBound()) {
//                            System.out.println(((CheckBoxTreeItem)parent).getValue() + ", " + str_path);
                            ((CheckBoxTreeItem)parent).setIndeterminate(true);
                        } else {
                            System.out.println(((CheckBoxTreeItem)parent).getValue());
                        }
                    } catch (Exception e) {
                        System.out.println(((CheckBoxTreeItem)parent).getValue());
                    }
                    
                }

                if(t.getValue() && !((CheckBoxTreeItem)globalParent).isIndeterminate()) {
                    ((CheckBoxTreeItem)globalParent).setIndeterminate(true);
                }

                str_path += ":" + path[i];
                
                if (!global_debug_requests.containsKey(str_path)) {
                    CheckBoxTreeItem<String> item = new CheckBoxTreeItem<>(path[i]);
                    
                    if(i == path.length - 1) {
                        item.setSelected(t.getValue());
                    }
                    globalParent.getChildren().add(item);
                    global_debug_requests.put(str_path, item);
                }
                
                if (!items.containsKey(str_path)) {
                    RequestTreeItem item = new RequestTreeItem("", path[i]);
                    
                    if(i == path.length - 1) {
                        item.setSelected(t.getValue());
                        item.setRequest(t.getName());
                        
                        item.selectedProperty().addListener(debugRequest);
                        // NOTE: can not use 'binding', because we're change the selected property and would causing a runtime exception otherwise
                        ((CheckBoxTreeItem)global_debug_requests.get(str_path)).selectedProperty().addListener((observable, oldValue, newValue) -> {
                            item.setSelected(newValue);
                        });
                    }
                    parent.getChildren().add(item);
                    items.put(str_path, item);
                }

            }
        });
    }
    
    public static TreeItem createDebugRequestCognition(Messages.DebugRequest request, ChangeListener debugRequest) {
        CheckBoxTreeItem<String> cognition_root = new CheckBoxTreeItem<>("Cognition");
        createDebugRequestTree(request, cognition_root, debugRequest);
        cognition_root.setExpanded(true);
        return cognition_root;
    }
    
    public static TreeItem createDebugRequestMotion(Messages.DebugRequest request, ChangeListener debugRequest) {
        CheckBoxTreeItem<String> motion_root = new CheckBoxTreeItem<>("Motion");
        createDebugRequestTree(request, motion_root, debugRequest);
        motion_root.setExpanded(true);
        return motion_root;
    }
    
    public static void createModulesTree(Messages.ModuleList list, CheckBoxTreeItem root, ChangeListener debugRequest) {
        
        String root_name = (String) root.getValue();
        Pattern p = Pattern.compile("([^\\s|]+).+/"+root.getValue()+"/(.+)\\/.+\\.h");

        list.getModulesList().stream().map((m) -> {
            // create for each module a checkable tree item
            Matcher match = p.matcher(m.getName());
            if(match.matches()) {
                return new RequestTreeItem(match.group(2), match.group(1), m.getActive(), match.group(1));
            }
            return null;
        }).filter((m) -> { return m!=null; }).sorted((m1, m2) -> {
            // sort the module tree items
            return m1.getPath().compareTo(m2.getPath());
        }).forEach((m) -> {
            // add module items and their respective tree path
            // the identifier of this module
            String id = root_name + "/" + m.getPath() + "/" + m.getValue();
            // retrieve (and if necessary create) global module item
            if(!global_modules.containsKey(id)) {
                CheckBoxTreeItem<String> global_item = new CheckBoxTreeItem<>(m.getValue());
                getParentTreeItem(global_modules.get(root_name), m.getPath()).getChildren().add(global_item);
                global_item.setSelected(m.active);
                global_modules.put(id, global_item);
            }
            // set the callback for (de-)activating this module
            m.selectedProperty().addListener(debugRequest);
            // bind to global module item
            // NOTE: can not use 'binding', because we're change the selected property and would causing a runtime exception otherwise
            ((CheckBoxTreeItem)global_modules.get(id)).selectedProperty().addListener((observable, oldValue, newValue) -> {
                m.setSelected(newValue);
            });
            // add this item to the module tree
            getParentTreeItem(root, m.getPath()).getChildren().add(m);
            // set the selected state AFTER adding it to its parent
            m.setSelected(m.active);
        });
        
        expandSingleTreeNodes(root);
        expandSingleTreeNodes(global_modules.get((String) root.getValue()));
    }
    
    private static TreeItem<String> getParentTreeItem(TreeItem<String> root, String path) {
        TreeItem<String> current_root = root;
        for (String part : path.split("/")) {
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
    
    public static TreeItem createModulesCognition(Messages.ModuleList list, ChangeListener moduleRequest) {
        CheckBoxTreeItem<String> cognition_root = new CheckBoxTreeItem<>("Cognition");
        createModulesTree(list, cognition_root, moduleRequest);
        cognition_root.setExpanded(true);
        return cognition_root;
    }
    
    public static TreeItem createModulesMotion(Messages.ModuleList list, ChangeListener moduleRequest) {
        CheckBoxTreeItem<String> motion_root = new CheckBoxTreeItem<>("Motion");
        createModulesTree(list, motion_root, moduleRequest);
        motion_root.setExpanded(true);
        return motion_root;
    }
}
