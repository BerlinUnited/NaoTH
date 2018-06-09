package de.naoth.rc.dialogs.multiagentconfiguration.ui;

import de.naoth.rc.dialogs.multiagentconfiguration.Parameter;
import de.naoth.rc.dialogs.multiagentconfiguration.Utils;
import javafx.scene.control.CheckBoxTreeItem;
import javafx.scene.control.TreeItem;

/**
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class AgentTabGlobal extends AgentTab
{
    public AgentTabGlobal() {
        super();

        setText("All");

        CheckBoxTreeItem<String> motion_root = new CheckBoxTreeItem<>("Motion");
        motion_root.setExpanded(true);
        debugTree.getRoot().getChildren().add(motion_root);
        Utils.global_debug_requests.put(motion_root.getValue(), motion_root);

        CheckBoxTreeItem<String> cognition_root = new CheckBoxTreeItem<>("Cognition");
        cognition_root.setExpanded(true);
        debugTree.getRoot().getChildren().add(cognition_root);
        Utils.global_debug_requests.put(cognition_root.getValue(), cognition_root);

        CheckBoxTreeItem<String> motion_modules_root = new CheckBoxTreeItem<>("Motion");
        motion_modules_root.setExpanded(true);
        System.out.println(moduleTree);
        moduleTree.getRoot().getChildren().add(motion_modules_root);
        Utils.global_modules.put(motion_modules_root.getValue(), motion_modules_root);

        CheckBoxTreeItem<String> cognition_modules_root = new CheckBoxTreeItem<>("Cognition");
        cognition_modules_root.setExpanded(true);
        moduleTree.getRoot().getChildren().add(cognition_modules_root);
        Utils.global_modules.put(cognition_modules_root.getValue(), cognition_modules_root);

        agentList.setItems(Utils.global_agent_list);
        TreeItem<Parameter> motion_params = new TreeItem<>(new Parameter("Motion", null));
        motion_params.setExpanded(true);
        parameterTree.getRoot().getChildren().add(motion_params);
        Utils.global_parameters.put(motion_params.getValue().getName(), motion_params);
        
        TreeItem<Parameter> cognition_params = new TreeItem<>(new Parameter("Cognition", null));
        cognition_params.setExpanded(true);
        parameterTree.getRoot().getChildren().add(cognition_params);
        Utils.global_parameters.put(cognition_params.getValue().getName(), cognition_params);
        
    }

    @Override
    protected void updateParameters() {
        // the global tab doesn't update any parameters directly!
        System.out.println("updateParameters");
    }

    @Override
    protected void updateDebugRequests() {
        // the global tab doesn't update any parameters directly!
        System.out.println("updateDebugRequests");
    }

    @Override
    protected void updateModules() {
        // the global tab doesn't update any parameters directly!
        System.out.println("updateModules");
    }

    @Override
    protected void saveModules() {
        // the global tab doesn't update any parameters directly!
    }

    @Override
    protected void saveParameters() {
        // the global tab doesn't update any parameters directly!
    }

    @Override
    protected void sendBehaviorFile() {
        // the global tab doesn't update any parameters directly!
    }
}
