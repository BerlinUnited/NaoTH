package de.naoth.rc.dialogs.multiagentconfiguration.ui;

import com.google.protobuf.InvalidProtocolBufferException;
import com.sun.javafx.scene.control.behavior.TabPaneBehavior;
import com.sun.javafx.scene.control.skin.TabPaneSkin;
import de.naoth.rc.messages.Messages;
import de.naoth.rc.messages.Messages.ModuleList;
import de.naoth.rc.server.Command;
import de.naoth.rc.server.ConnectionStatusEvent;
import de.naoth.rc.server.ConnectionStatusListener;
import de.naoth.rc.server.MessageServer;
import de.naoth.rc.server.ResponseListener;
import de.naoth.rc.dialogs.multiagentconfiguration.Parameter;
import de.naoth.rc.dialogs.multiagentconfiguration.Utils;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.application.Platform;
import javafx.beans.property.BooleanProperty;
import javafx.beans.property.SimpleObjectProperty;
import javafx.beans.property.StringProperty;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.control.Button;
import javafx.scene.control.ComboBox;
import javafx.scene.control.SplitPane;
import javafx.scene.control.Tab;
import javafx.scene.control.Tooltip;
import javafx.scene.control.TreeItem;
import javafx.scene.control.TreeTableColumn;
import javafx.scene.control.TreeTableView;
import javafx.scene.control.TreeView;
import javafx.scene.control.cell.TreeItemPropertyValueFactory;
import javafx.stage.FileChooser;
import javafx.stage.FileChooser.ExtensionFilter;

/**
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class AgentTab extends Tab implements ConnectionStatusListener, ResponseListener
{
    private MessageServer server = new MessageServer();
    
    private final Command cmd_debug_cognition = new Command("Cognition:representation:get").addArg("DebugRequest");
    
    private final Command cmd_debug_motion = new Command("Motion:representation:get").addArg("DebugRequest");
    
    private final Command cmd_modules_cognition = new Command("Cognition:modules:list");
    private final Command cmd_modules_cognition_store = new Command("Cognition:modules:store");
    private final Command cmd_modules_motion = new Command("Motion:modules:list");
    private final Command cmd_modules_motion_store = new Command("Motion:modules:store");
    
    private final Command cmd_agent_list = new Command("Cognition:behavior:list_agents");
    
    private final Command cmd_agent = new Command("Cognition:behavior:get_agent");
    
    private final Command cmd_agent_set = new Command("Cognition:behavior:set_agent");
    
    private final Command cmd_parameter_cognition = new Command("Cognition:ParameterList:list");
    
    private final Command cmd_parameter_motion = new Command("Motion:ParameterList:list");
    
    private final String cmd_parameter_cognition_get = "Cognition:ParameterList:get";
    
    private final String cmd_parameter_motion_get = "Motion:ParameterList:get";
    
    private final String cmd_write_behavior = "Cognition:file::write";
    private final String behaviorPath = "Config/behavior-ic.dat";
    
    private String agent = "";
    private List<String> agent_list = new ArrayList<>();
    private final TreeMap<String, TreeItem<Parameter>> parameter = new TreeMap<>();
    private final SimpleObjectProperty<File> behaviorFile = new SimpleObjectProperty<>(null);
    
    
    private ChangeListener cognitionDebugRequest = (ChangeListener) (ObservableValue o, Object v, Object n) -> {
        request("Cognition:debugrequest:set", o, (boolean) n);
    };
    
    private ChangeListener motionDebugRequest = (ChangeListener) (ObservableValue o, Object v, Object n) -> {
        request("Motion:debugrequest:set", o, (boolean) n);
    };
    
    private ChangeListener cognitionModuleRequest = (ChangeListener) (ObservableValue o, Object v, Object n) -> {
        request("Cognition:modules:set", o, (boolean) n);
    };
    
    private ChangeListener motionModuleRequest = (ChangeListener) (ObservableValue o, Object v, Object n) -> {
        request("Motion:modules:set", o, (boolean) n);
    };
    
    private ChangeListener parameterRequest = (ChangeListener) (ObservableValue o, Object v, Object n) -> {
        Parameter p = (Parameter) ((StringProperty)o).getBean();
        Map<String, byte[]> args = new HashMap<>();
        args.put("<name>", p.getModule().getBytes());
        args.put(p.getName(), p.getValue().getBytes());
        request(p.getType()+":ParameterList:set", args);
    };
    
    @FXML
    protected TreeView<String> debugTree;
    
    @FXML
    protected TreeView<String> moduleTree;
    
    @FXML
    protected TreeTableView<Parameter> parameterTree;
    
    @FXML
    protected SplitPane splitPane;
    
    @FXML
    protected Button btnSaveModules;
    
    @FXML
    protected Button btnUpdateDebug;
    
    @FXML
    protected Button btnUpdateModules;
    
    @FXML
    protected Button btnUpdateParameters;
    
    @FXML
    protected Button btnSaveParameters;
    
    @FXML
    protected Button btnSendBehavior;
    
    @FXML
    protected ComboBox<String> agentList;
    
    
    public AgentTab() {
        super();
        
        try {
            FXMLLoader loader = new FXMLLoader(AgentTab.class.getResource("/de/naoth/rc/dialogs/multiagentconfiguration/ui/AgentTab.fxml"));
            loader.setController(this);
            setContent(loader.load());
        } catch (IOException ex) {
            Logger.getLogger(Tab.class.getName()).log(Level.SEVERE, null, ex);
        }

        // setup ui
        debugTree.setShowRoot(false);
        debugTree.setEditable(true);
        debugTree.setCellFactory(e -> new CheckableTreeCell());

        debugTree.setRoot(new TreeItem());
        debugTree.getRoot().setExpanded(true);
        
        moduleTree.setShowRoot(false);
        moduleTree.setEditable(true);
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
        });
        
        behaviorFile.addListener((o, v, f) -> { sendBehaviorFile(); });

        // set some tooltips
        btnSaveModules.setTooltip(new Tooltip("Save module configuration on robot(s)."));
        btnSaveParameters.setTooltip(new Tooltip("Save parameters locally."));
        btnUpdateDebug.setTooltip(new Tooltip("Update debug requests."));
        btnUpdateModules.setTooltip(new Tooltip("Update modules."));
        btnUpdateParameters.setTooltip(new Tooltip("Update parameters."));
        btnSendBehavior.setTooltip(new Tooltip("Sends a new behavior file to the robot(s)."));
        agentList.setTooltip(new Tooltip("Select executing agent."));
    }
    
    public AgentTab(String host, int port)  {
        this();
        setText(host + ":" + port);
        
        server.addConnectionStatusListener(this);
        
        setOnCloseRequest((event) -> {
            System.out.println("closing tab");
            server.disconnect();
        });
        
        try {
            server.connect(host, port);
        } catch (IOException ex) {
            Logger.getLogger(Tab.class.getName()).log(Level.SEVERE, null, ex);
        }
        
        agentList.getSelectionModel().selectedItemProperty().addListener((observable, oldValue, newValue) -> {
            if(newValue != null && !newValue.isEmpty()) {
                if(cmd_agent_set.getArguments() != null) {
                    cmd_agent_set.getArguments().clear();
                }
                cmd_agent_set.addArg("agent", newValue);
                sendCommand(cmd_agent_set);
            }
        });

        updateModules();
        updateDebugRequests();
        updateParameters();
        sendCommand(cmd_agent_list);
        sendCommand(cmd_agent);
    }
    
    private boolean sendCommand(Command cmd) {
        if(server.isConnected()) {
            server.executeCommand(this, cmd);
            return true;
        }
        return false;
    }
    
    @FXML
    protected void updateParameters() {
        parameterTree.getRoot().getChildren().clear();
        sendCommand(cmd_parameter_cognition);
        sendCommand(cmd_parameter_motion);
    }
    
    @FXML
    protected void updateModules() {
        moduleTree.getRoot().getChildren().clear();
        sendCommand(cmd_modules_cognition);
        sendCommand(cmd_modules_motion);
    }
    
    @FXML
    protected void updateDebugRequests() {
        debugTree.getRoot().getChildren().clear();
        sendCommand(cmd_debug_cognition);
        sendCommand(cmd_debug_motion);
    }
    
    @FXML
    protected void saveModules() {
        sendCommand(cmd_modules_cognition_store);
        sendCommand(cmd_modules_motion_store);
    }
    
    @FXML
    protected void saveParameters() {
        System.out.println("saveParameters");
    }
    
    @FXML
    protected void selectBehaviorFile() {
        FileChooser fileChooser = new FileChooser();
        fileChooser.setTitle("Open Behavior File");
        fileChooser.setInitialFileName("behavior-ic.dat");
        fileChooser.setInitialDirectory(new File("../../NaoTHSoccer/Config/"));
        fileChooser.getExtensionFilters().addAll(
                new ExtensionFilter("Behavior Files", "*.dat"),
                new ExtensionFilter("All Files", "*.*"));
        File selectedFile = fileChooser.showOpenDialog(getTabPane().getScene().getWindow());
        if (selectedFile != null) {
            behaviorFile.set(selectedFile);
        }
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
                request(cmd_write_behavior, args);
            } catch (FileNotFoundException ex) {
                Logger.getLogger(AgentTab.class.getName()).log(Level.SEVERE, null, ex);
            } catch (IOException ex) {
                Logger.getLogger(AgentTab.class.getName()).log(Level.SEVERE, null, ex);
            }
            // reset behavior file, in order to be able to select file again
            behaviorFile.set(null);
        }
    }
    
    public void requestClose() {
        TabPaneBehavior behavior = getBehavior();
        if(behavior.canCloseTab(this)) {
            behavior.closeTab(this);
        }
    }

    private TabPaneBehavior getBehavior() {
        return ((TabPaneSkin) getTabPane().getSkin()).getBehavior();
    }

    @Override
    public void connected(ConnectionStatusEvent event) {
        // TODO: 
        System.out.println("connected to robot");
    }

    @Override
    public void disconnected(ConnectionStatusEvent event) {
        // TODO: 
        System.out.println("disconnected from robot");
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
            } else {
                System.out.println(command.getName() + ": " + new String(result));
            }
        });
    }

    @Override
    public void handleError(int code) {
        System.out.println("ERROR: " + code);
    }
    
    private void handleCognitionDebugResponse(byte[] result) {
        try {
            Messages.DebugRequest request = Messages.DebugRequest.parseFrom(result);
            debugTree.getRoot().getChildren().add(Utils.createDebugRequestCognition(request, cognitionDebugRequest));
        } catch (InvalidProtocolBufferException ex) {
            Logger.getLogger(Tab.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
    
    private void handleMotionDebugResponse(byte[] result) {
        try {
            Messages.DebugRequest request = Messages.DebugRequest.parseFrom(result);
            debugTree.getRoot().getChildren().add(0, Utils.createDebugRequestMotion(request, motionDebugRequest));
        } catch (InvalidProtocolBufferException ex) {
            Logger.getLogger(Tab.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
    
    private void handleCognitionModulesResponse(byte[] result) {
        try {
            ModuleList list = ModuleList.parseFrom(result);
            moduleTree.getRoot().getChildren().add(Utils.createModulesCognition(list, cognitionModuleRequest));
        } catch (InvalidProtocolBufferException ex) {
            Logger.getLogger(Tab.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
    
    private void handleMotionModulesResponse(byte[] result) {
        try {
            ModuleList list = ModuleList.parseFrom(result);
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
            
            sendCommand(new Command(cmd).addArg("<name>", p));
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
                    // create parameter objects (local, global)
                    Parameter pl = new Parameter(type, param, parts[0], parts[1]);
                    Parameter pg = new Parameter(type, param, parts[0], parts[1]);
                    // add listener to global parameter (changes the local)
                    pg.valueProperty().addListener((o, v1, v2) -> { pl.setValue(v2); });
                    pl.valueProperty().addListener(parameterRequest);
                    // add to respective parameter tree
                    treeItem.getChildren().add(new TreeItem<>(pl));
                    treeItem_global.getChildren().add(new TreeItem<>(pg));
                } else {
                    System.err.println("Invalid motion parameter: " + Arrays.asList(parts));
                }
            });
        }
        
    }
    
    private void request(String cmd, ObservableValue o, boolean state) {
        Object i = ((BooleanProperty)o).getBean();
        if(i instanceof RequestTreeItem && ((RequestTreeItem)i).getRequest() != null && server.isConnected()) {
            Command command = new Command(cmd).addArg(((RequestTreeItem)i).getRequest(), state ? "on" : "off");
            sendCommand(command);
        }
    }
    
    private void request(String cmd, Map<String, byte[]> args) {
        Command command = new Command(cmd);
        command.setArguments(args);
        sendCommand(command);
    }
    
    public void connectDivider(AgentTab other) {
        for (int i = 0; i < splitPane.getDividers().size(); i++) {
            other.splitPane.getDividers().get(i).positionProperty().bindBidirectional(splitPane.getDividers().get(i).positionProperty());
        }
    }
    
    public void connectAgentList(AgentTab other) {
        other.agentList.getSelectionModel().selectedItemProperty().addListener((observable, oldValue, newValue) -> {
            agentList.getSelectionModel().select(newValue);
        });
    }
    
    public void connectButtons(AgentTab other) {
        other.btnUpdateDebug.addEventHandler(ActionEvent.ACTION, (e) -> { btnUpdateDebug.fire(); });
        other.btnUpdateModules.addEventHandler(ActionEvent.ACTION, (e) -> { btnUpdateModules.fire(); });
        other.btnUpdateParameters.addEventHandler(ActionEvent.ACTION, (e) -> { btnUpdateParameters.fire(); });
        other.btnSaveModules.addEventHandler(ActionEvent.ACTION, (e) -> { btnSaveModules.fire(); });
        other.behaviorFile.addListener((o, v, f) -> { behaviorFile.set(f); });
    }
}
