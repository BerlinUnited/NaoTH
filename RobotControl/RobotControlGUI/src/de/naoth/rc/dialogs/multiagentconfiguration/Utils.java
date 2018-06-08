package de.naoth.rc.dialogs.multiagentconfiguration;

import de.naoth.rc.dialogs.multiagentconfiguration.ui.RequestTreeItem;
import de.naoth.rc.messages.Messages;
import de.naoth.rc.messages.Messages.Module;
import java.util.TreeMap;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import javafx.beans.value.ChangeListener;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
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
                    RequestTreeItem item = new RequestTreeItem(path[i]);
                    
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
    
    public static void createModulesTree(Messages.ModuleList list, TreeItem root, ChangeListener debugRequest) {
        
        String root_name = (String) root.getValue();
        Pattern p = Pattern.compile("([^\\s|]+).+/"+root.getValue()+"/(.+)\\/.+\\.h");

        TreeMap<String, TreeItem> items = new TreeMap<>();
        items.put(root_name, root);
        
        for (Module m : list.getModulesList()) {
            Matcher match = p.matcher(m.getName());
            if(match.matches()) {
                String path_name =  match.group(2) + "/" + match.group(1);
                
                System.out.println(m.getName() + " | " + path_name);
                
                String[] path = path_name.split("/");
                String str_path = root_name;
                for (int i = 0; i < path.length; i++) {
                    TreeItem parent = items.get(str_path);
                    TreeItem globalParent = global_modules.get(str_path);

                    if(m.getActive() && !((CheckBoxTreeItem)parent).isIndeterminate()) {
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

                    if(m.getActive() && !((CheckBoxTreeItem)globalParent).isIndeterminate()) {
                        ((CheckBoxTreeItem)globalParent).setIndeterminate(true);
                    }

                    str_path += ":" + path[i];

                    if (!global_modules.containsKey(str_path)) {
                        CheckBoxTreeItem<String> item = new CheckBoxTreeItem<>(path[i]);

                        if(i == path.length - 1) {
                            item.setSelected(m.getActive());
                        }
                        globalParent.getChildren().add(item);
                        global_modules.put(str_path, item);
                    }

                    if (!items.containsKey(str_path)) {
                        RequestTreeItem item = new RequestTreeItem(path[i]);

                        if(i == path.length - 1) {
                            item.setSelected(m.getActive());
                            item.setRequest(match.group(1)); // processName + ":" + commandStringSetModules

                            item.selectedProperty().addListener(debugRequest);
                            // NOTE: can not use 'binding', because we're change the selected property and would causing a runtime exception otherwise
                            ((CheckBoxTreeItem)global_modules.get(str_path)).selectedProperty().addListener((observable, oldValue, newValue) -> {
                                item.setSelected(newValue);
                            });
                        }
                        parent.getChildren().add(item);
                        items.put(str_path, item);
                    }
                }
            }
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
    
//    public static TreeItem createParameterTree()
}
