package de.naoth.rc.dialogsFx.multiagentconfiguration.ui;

import de.naoth.rc.dialogsFx.multiagentconfiguration.components.TreeNode;
import de.naoth.rc.dialogsFx.multiagentconfiguration.components.CheckableTreeCell;
import com.google.protobuf.InvalidProtocolBufferException;
import de.naoth.rc.dialogsFx.multiagentconfiguration.Parameter;
import de.naoth.rc.dialogsFx.multiagentconfiguration.Utils;
import de.naoth.rc.core.messages.Messages;
import de.naoth.rc.core.messages.Messages.DebugRequest;
import de.naoth.rc.core.server.Command;
import de.naoth.rc.core.server.ResponseListener;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;
import java.util.function.BiConsumer;
import java.util.function.Consumer;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import javafx.application.Platform;
import javafx.beans.property.SimpleObjectProperty;
import javafx.fxml.FXML;
import javafx.scene.control.Button;
import javafx.scene.control.ComboBox;
import javafx.scene.control.Label;
import javafx.scene.control.SplitMenuButton;
import javafx.scene.control.SplitPane;
import javafx.scene.control.Tab;
import javafx.scene.control.TreeItem;
import javafx.scene.control.TreeTableColumn;
import javafx.scene.control.TreeTableView;
import javafx.scene.control.TreeView;
import javafx.scene.control.cell.TreeItemPropertyValueFactory;
import javafx.stage.DirectoryChooser;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class ConfigurationsTab implements ResponseListener
{
    private AgentTab parent;
    
    private final Command cmd_agent_set = new Command("Cognition:behavior:set_agent");
    private final Command cmd_parameter_cognition = new Command("Cognition:ParameterList:list");
    private final Command cmd_parameter_motion = new Command("Motion:ParameterList:list");
    private final Command cmd_modules_cognition = new Command("Cognition:modules:list");
    private final Command cmd_modules_cognition_store = new Command("Cognition:modules:store");
    private final Command cmd_modules_motion = new Command("Motion:modules:list");
    private final Command cmd_modules_motion_store = new Command("Motion:modules:store");
    private final Command cmd_debug_cognition = new Command("Cognition:representation:get").addArg("DebugRequest");
    private final Command cmd_debug_motion = new Command("Motion:representation:get").addArg("DebugRequest");
    private final Command cmd_agent_list = new Command("Cognition:behavior:list_agents");
    private final Command cmd_agent = new Command("Cognition:behavior:get_agent");
    private final Command cmd_playerinfo_scheme = new Command("Cognition:representation:print").addArg("PlayerInfo");
    private final String cmd_write_behavior = "Cognition:file::write";
    private final String cmd_parameter_cognition_get = "Cognition:ParameterList:get";
    private final String cmd_parameter_motion_get = "Motion:ParameterList:get";
    
    
    private String agent = "";
    private List<String> agent_list = new ArrayList<>();
    private final TreeMap<String, TreeItem<Parameter>> parameter = new TreeMap<>();
    
    private final BiConsumer<String, Boolean> cognitionDebugRequest = (r, b) -> {
        Command request = new Command("Cognition:representation:set").addArg("DebugRequest",
            DebugRequest.newBuilder().addRequests(
                DebugRequest.Item.newBuilder().setName(r).setValue(b)
            ).build().toByteArray());
        parent.sendCommand(request, this);
    };
    private final BiConsumer<String, Boolean> motionDebugRequest = (r, b) -> {
        Command request = new Command("Motion:representation:set").addArg("DebugRequest",
            DebugRequest.newBuilder().addRequests(
                DebugRequest.Item.newBuilder().setName(r).setValue(b)
            ).build().toByteArray());
        parent.sendCommand(request, this);
    };
    
    private final BiConsumer<String, Boolean> cognitionModuleRequest = (r, b) -> {
        parent.sendCommand(new Command("Cognition:modules:set").addArg(r, b ? "on" : "off"), this);
    };
    
    private final BiConsumer<String, Boolean> motionModuleRequest = (r, b) -> {
        parent.sendCommand(new Command("Motion:modules:set").addArg(r, b ? "on" : "off"), this);
    };
    
    private final Consumer<Parameter> parameterRequest = (p) -> {
        parent.sendCommand(new Command(p.getType()+":ParameterList:set").addArg("<name>", p.getModule()).addArg(p.getName(), p.getValue()), this);
    };
    
    private final String behaviorPath = "Config/behavior-ic.dat";
    protected final SimpleObjectProperty<File> behaviorFile = new SimpleObjectProperty<>(null);
    
    @FXML protected TreeView<String> debugTree;
    @FXML protected TreeView<String> moduleTree;
    @FXML protected TreeTableView<Parameter> parameterTree;
    @FXML protected SplitMenuButton btnSaveParameters;
    @FXML protected Button btnSaveModules;
    @FXML protected SplitPane splitPane;
    @FXML protected Button btnUpdateDebug;
    @FXML protected Button btnUpdateModules;
    @FXML protected Button btnUpdateParameters;
    @FXML protected Button btnSendBehavior;
    @FXML protected ComboBox<String> agentList;
    @FXML protected Label lblScheme;
    
    @FXML public void initialize() {
        
        // setup ui
        debugTree.setCellFactory(e -> new CheckableTreeCell());
        debugTree.setRoot(new TreeNode());
        debugTree.getRoot().setExpanded(true);
        
        moduleTree.setCellFactory(e -> new CheckableTreeCell());
        moduleTree.setRoot(new TreeItem());
        moduleTree.getRoot().setExpanded(true);
        
        parameterTree.setShowRoot(false);
        
        TreeItem<Parameter> parameterTreeRoot = new TreeItem<>(new Parameter("root", null));
        
        parameterTree.setRoot(parameterTreeRoot);
        parameterTree.getRoot().setExpanded(true);
        
        TreeTableColumn<Parameter, String> col_name = (TreeTableColumn<Parameter, String>) parameterTree.getColumns().get(0);
        TreeTableColumn<Parameter, String> col_value = (TreeTableColumn<Parameter, String>) parameterTree.getColumns().get(1);
        col_name.setCellValueFactory(new TreeItemPropertyValueFactory("name"));
        col_value.setCellValueFactory(new TreeItemPropertyValueFactory("value"));
        col_value.setCellFactory(list -> new ParameterTreeTableCell());
        
        col_value.setOnEditCommit((evt) -> {
            if(!evt.getRowValue().isLeaf()) {
                return;
            }
            evt.getRowValue().getValue().setValue(evt.getNewValue());
            evt.getTreeTableView().requestFocus();
        });
        
        agentList.getSelectionModel().selectedItemProperty().addListener((observable, oldValue, newValue) -> {
            if(newValue != null && !newValue.isEmpty()) {
                if(cmd_agent_set.getArguments() != null) {
                    cmd_agent_set.getArguments().clear();
                }
                cmd_agent_set.addArg("agent", newValue);
                parent.sendCommand(cmd_agent_set, this);
            }
        });
        
        behaviorFile.addListener((o, v, f) -> { sendBehaviorFile(); });
    }
    
    @FXML
    private void selectBehaviorFile() {
        File selectedFile = Utils.showBehaviorFileDialog(parent.getTabPane().getScene().getWindow());
        if (selectedFile != null) {
            behaviorFile.set(selectedFile);
        }
    }
    
    @FXML
    private void saveParameters() {
        File selectedDirectory = selectParameterDirectory();
        if(selectedDirectory != null) {
            // write configs
            writeConfigFiles(parameterTree.getRoot().getChildren().get(0), selectedDirectory.getPath());
            writeConfigFiles(parameterTree.getRoot().getChildren().get(1), selectedDirectory.getPath());
        }
    }
    
    @FXML
    public void updateParameters() {
        parameterTree.getRoot().getChildren().clear();
        parent.sendCommand(cmd_parameter_cognition, this);
        parent.sendCommand(cmd_parameter_motion, this);
    }
    
    @FXML
    public void updateModules() {
        moduleTree.getRoot().getChildren().clear();
        parent.sendCommand(cmd_modules_cognition, this);
        parent.sendCommand(cmd_modules_motion, this);
    }
    
    @FXML
    public void updateDebugRequests() {
        parent.sendCommand(cmd_debug_cognition, this);
        parent.sendCommand(cmd_debug_motion, this);
    }
    
    @FXML
    private void saveParametersMotion() {
        File selectedDirectory = selectParameterDirectory();
        if(selectedDirectory != null) {
            // write configs
            writeConfigFiles(parameterTree.getRoot().getChildren().get(0), selectedDirectory.getPath());
        }
    }
    
    @FXML
    private void saveParametersCognition() {
        File selectedDirectory = selectParameterDirectory();
        if(selectedDirectory != null) {
            // write configs
            writeConfigFiles(parameterTree.getRoot().getChildren().get(1), selectedDirectory.getPath());
        }
    }
    
    @FXML
    public void saveModules() {
        parent.sendCommand(cmd_modules_cognition_store, this);
        parent.sendCommand(cmd_modules_motion_store, this);
    }
    
    public void setParent(AgentTab t) {
        parent = t;
    }
    
    public void connected() {
        updateModules();
        updateDebugRequests();
        updateParameters();
        parent.sendCommand(cmd_agent_list, this);
        parent.sendCommand(cmd_agent, this);
        parent.sendCommand(cmd_playerinfo_scheme, this);
    }
    
    private File selectParameterDirectory() {
        DirectoryChooser dirChooser = new DirectoryChooser();
        dirChooser.setTitle("Save all parameters");
        dirChooser.setInitialDirectory(new File("../../NaoTHSoccer/Config/"));
        File selectedFile = dirChooser.showDialog(parent.getTabPane().getScene().getWindow());
        if (selectedFile != null && selectedFile.isDirectory()) {
            // TODO: ask, whether overwriting file is "ok"
            if(true) {
                // trigger exception (if couldn't write)
                try {
                    File f = File.createTempFile("tmp_"+Math.random(), ".tmp", selectedFile);
                    f.delete();
                } catch (IOException ex) {
                    // TODO: show hint & exit
                    Logger.getLogger(AgentTab.class.getName()).log(Level.SEVERE, null, ex);
                    return null;
                }
                return selectedFile;
            }
        }
        return null;
    }
    
    private void writeConfigFiles(TreeItem<Parameter> type, String path) {
        type.getChildren().forEach((p) -> {
            try {
                File cfg = new File(path + File.separator + type.getValue().getName() + "_" + p.getValue().getName() + ".cfg");
                BufferedWriter bf = new BufferedWriter(new FileWriter(cfg));
                bf.write("[" + p.getValue().getName() + "]\n");
                for (TreeItem<Parameter> v : p.getChildren()) {
                    bf.write(v.getValue().getName() + "=" + v.getValue().getValue() + "\n");
                }
                bf.close();
            } catch (IOException ex) {
                Logger.getLogger(AgentTab.class.getName()).log(Level.SEVERE, null, ex);
            }
        });
    }
    
    protected void sendBehaviorFile() {
        if (behaviorFile.get() != null) {
            try (FileReader fileReader = new FileReader(behaviorFile.get())) {
                StringBuilder buffer = new StringBuilder();
                int c = fileReader.read();
                while (c != -1) {
                    buffer.append((char) c);
                    c = fileReader.read();
                }//end while
                HashMap<String, byte[]> args = new HashMap<>();
                args.put("path", behaviorPath.getBytes());
                args.put("content", buffer.toString().getBytes());
                parent.sendCommand(cmd_write_behavior, args, this);
            } catch (FileNotFoundException ex) {
                Logger.getLogger(AgentTab.class.getName()).log(Level.SEVERE, null, ex);
            } catch (IOException ex) {
                Logger.getLogger(AgentTab.class.getName()).log(Level.SEVERE, null, ex);
            }
            // reset behavior file, in order to be able to select file again
            // NOTE: this triggers the event handler again!
            behaviorFile.set(null);
        }
    }
    
    public void updateConnectionStatusUI(boolean connected) {
        if(connected) {
            splitPane.setDisable(false);
        } else {
            splitPane.setDisable(true);
        }
    }

    @Override
    public void handleResponse(byte[] result, Command command) {
        Platform.runLater(() -> {
            if(command.equals(cmd_debug_cognition)) {
                handleCognitionDebugResponse(result);
            } else if(command.equals(cmd_debug_motion)) {
                handleMotionDebugResponse(result);
            } else if(command.equals(cmd_modules_cognition)) {
                handleCognitionModulesResponse(result);
            } else if(command.equals(cmd_modules_motion)) {
                handleMotionModulesResponse(result);
            } else if(command.equals(cmd_agent_list)) {
                handleAgentListResponse(result);
            } else if(command.equals(cmd_agent)) {
                agent = new String(result);

                if(agent.isEmpty()) {
                    agentList.getSelectionModel().select(null);
                } else {
                    agentList.getSelectionModel().select(agent);
                }
            } else if(command.equals(cmd_parameter_cognition)) {
                handleCognitionParameterResponse(result);
            } else if(command.equals(cmd_parameter_motion)) {
                handleMotionParameterResponse(result);
            } else if(command.getName().equals(cmd_parameter_cognition_get)) {
                handleCognitionParameterGetResponse(new String(command.getArguments().get("<name>")), result);
            } else if(command.getName().equals(cmd_parameter_motion_get)) {
                handleMotionParameterGetResponse(new String(command.getArguments().get("<name>")), result);
            } else if(command.equals(cmd_modules_cognition_store) || command.equals(cmd_modules_motion_store)) {
                // currently do nothing ...
                System.out.println(command.getName() + ": " + new String(result));
            } else if(command.equals(cmd_playerinfo_scheme)) {
                handlePlayerInforResponse(result);
            } else {
                System.out.println(command.getName() + ": " + new String(result));
            }
        });
    }

    @Override
    public void handleError(int code) {
        System.err.println("ERROR: " + code);
    }
    
    private void handleCognitionDebugResponse(byte[] result) {
        try {
            // parse data
            Messages.DebugRequest request = Messages.DebugRequest.parseFrom(result);
            // get the cognition debug request root item or create a new, if it doesn't exists
            TreeNode cognition_root;
            if(((TreeNode)debugTree.getRoot()).hasChildren("Cognition")) {
                cognition_root = ((TreeNode)debugTree.getRoot()).getChildren("Cognition");
            } else {
                cognition_root = new TreeNode("Cognition");
                cognition_root.setExpanded(true);
                debugTree.getRoot().getChildren().add(cognition_root);
            }
            List<String> t = Utils.getExpandedNodes(cognition_root);
            // remove the old subtree
            cognition_root.getChildren().clear();
            // create the new subtree
            Utils.createDebugRequestTree(request, cognition_root, cognitionDebugRequest);
            Utils.expandNodes((TreeNode) cognition_root.getParent(), t);
        } catch (InvalidProtocolBufferException ex) {
            Logger.getLogger(Tab.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
    
    private void handleMotionDebugResponse(byte[] result) {
        try {
            // parse data
            Messages.DebugRequest request = Messages.DebugRequest.parseFrom(result);
            // get the motion debug request root item or create a new, if it doesn't exists
            TreeNode motion_root;
            if(((TreeNode)debugTree.getRoot()).hasChildren("Motion")) {
                motion_root = (TreeNode) debugTree.getRoot().getChildren().get(0);
            } else {
                motion_root = new TreeNode("Motion");
                debugTree.getRoot().getChildren().add(0, motion_root);
                motion_root.setExpanded(true);
            }
            List<String> t = Utils.getExpandedNodes(motion_root);
            // remove the old subtree
            motion_root.getChildren().clear();
            // create the new subtree
            Utils.createDebugRequestTree(request, motion_root, motionDebugRequest);
            Utils.expandNodes((TreeNode) motion_root.getParent(), t);
        } catch (InvalidProtocolBufferException ex) {
            Logger.getLogger(Tab.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
    
    private void handleCognitionModulesResponse(byte[] result) {
        try {
            Messages.ModuleList list = Messages.ModuleList.parseFrom(result);
            moduleTree.getRoot().getChildren().add(Utils.createModulesCognition(list, cognitionModuleRequest));
        } catch (InvalidProtocolBufferException ex) {
            Logger.getLogger(Tab.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
    
    private void handleMotionModulesResponse(byte[] result) {
        try {
            Messages.ModuleList list = Messages.ModuleList.parseFrom(result);
            moduleTree.getRoot().getChildren().add(0, Utils.createModulesMotion(list, motionModuleRequest));
        } catch (InvalidProtocolBufferException ex) {
            Logger.getLogger(Tab.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
    
    private void handleAgentListResponse(byte[] result) {
        agent_list = Arrays.asList(new String(result).split("\n"));
        agentList.getItems().addAll(agent_list);
        // TODO: check duplicates!
        Utils.global_agent_list.addAll(agent_list);

        if(agent.isEmpty()) {
            agentList.getSelectionModel().select(null);
        } else {
            agentList.getSelectionModel().select(agent);
        }
    }
    
    private void handleCognitionParameterResponse(byte[] result) {
        TreeItem<Parameter> root_item = new TreeItem<>(new Parameter("Cognition", null));
        root_item.setExpanded(true);
        parameterTree.getRoot().getChildren().add(root_item);
        
        handleParameterResponse(root_item, cmd_parameter_cognition_get, result);
    }
    
    private void handleMotionParameterResponse(byte[] result) {
        TreeItem<Parameter> root_item = new TreeItem<>(new Parameter("Motion", null));
        root_item.setExpanded(true);
        parameterTree.getRoot().getChildren().add(0, root_item);
        
        handleParameterResponse(root_item, cmd_parameter_motion_get, result);
    }
    
    private void handleParameterResponse(TreeItem<Parameter> root_item, String cmd, byte[] result) {
        
        TreeItem<Parameter> root_global = Utils.global_parameters.get(root_item.getValue().getName());
        
        Arrays.asList(new String(result).split("\n")).forEach((p) -> {
            // an identifier for the parameter
            String parameterId = root_item.getValue().getName()+":"+p;
            // check if the global parameter list contains the parameter
            if(!Utils.global_parameters.containsKey(parameterId)) {
                TreeItem<Parameter> global_parameter = new TreeItem<>(new Parameter(p, null));
                root_global.getChildren().add(global_parameter);
                Utils.global_parameters.put(parameterId, global_parameter);
            }
            
            TreeItem<Parameter> parameter = new TreeItem<>(new Parameter(p, null));
            this.parameter.put(parameterId, parameter);
            root_item.getChildren().add(parameter);
            
            parent.sendCommand(new Command(cmd).addArg("<name>", p), this);
        });
    }
    
    private void handleCognitionParameterGetResponse(String param, byte[] result) {
        handleParameterGetResponse("Cognition", param, result);
    }
    
    private void handleMotionParameterGetResponse(String param, byte[] result) {
        handleParameterGetResponse("Motion", param, result);
    }
    
    private void handleParameterGetResponse(String type, String param, byte[] result) {
        String parameterId = type + ":" + param;
        TreeItem<Parameter> treeItem = this.parameter.get(parameterId);
        TreeItem<Parameter> treeItem_global = Utils.global_parameters.get(parameterId);
        if(treeItem != null) {
            Arrays.asList(new String(result).split("\n")).forEach((p) -> {
                String[] parts = p.split("=");
                if(parts.length == 2) {
                    // find existing parameter in global tree
                    TreeItem<Parameter> globalParameter = treeItem_global.getChildren().stream().filter((TreeItem<Parameter> t) -> {
                        return t.getValue().getName().equals(parts[0]);
                    }).findAny().orElse(null);
                    // create new item in global tree, if it wasn't found
                    if(globalParameter == null) {
                        globalParameter = new TreeItem<>(new Parameter(type, param, parts[0], parts[1]));
                        treeItem_global.getChildren().add(globalParameter);
                    }
                    
                    // create parameter objects (local, global)
                    Parameter pl = new Parameter(type, param, parts[0], parts[1]);
                    // add listener to global parameter (changes the local)
                    globalParameter.getValue().valueProperty().addListener((o, v1, v2) -> { pl.setValue(v2); });
                    pl.valueProperty().addListener((o) -> { parameterRequest.accept(pl); });
                    // add to respective parameter tree
                    treeItem.getChildren().add(new TreeItem<>(pl));
                } else {
                    System.err.println("Invalid motion parameter: " + Arrays.asList(parts));
                }
            });
        }
        
    }
    
    private void handlePlayerInforResponse(byte[] result) {
        // extract representation from string
        Pattern keyValue = Pattern.compile("^(?<key>.+)=(?<value>.*)$", Pattern.MULTILINE);
        Map<String, String> repr = new HashMap<>();
        Matcher m = keyValue.matcher(new String(result));
        while (m.find()) {
            repr.put(m.group("key").trim(), m.group("value").trim());
        }
        String scheme = repr.getOrDefault("active scheme", "?");
        lblScheme.setText((scheme.equals("-")?"default parameter":scheme));
    }
}
