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
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.TreeMap;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.application.Platform;
import javafx.beans.property.BooleanProperty;
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
    
    
    private String agent = "";
    private List<String> agent_list = new ArrayList<>();
    private final TreeMap<String, TreeItem<Parameter>> cognitionParameter = new TreeMap<>();
    private final TreeMap<String, TreeItem<Parameter>> motionParameter = new TreeMap<>();
    
    
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
        parameterTreeRoot.getChildren().addAll(new TreeItem<>(new Parameter("Motion", null)), new TreeItem<>(new Parameter("Cognition", null)));
        
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
                server.executeCommand(this, cmd_agent_set);
            }
        });

        updateModules();
        updateDebugRequests();
        updateParameters();
        server.executeCommand(this, cmd_agent_list);
        server.executeCommand(this, cmd_agent);
    }
    
    @FXML
    protected void updateParameters() {
        server.executeCommand(this, cmd_parameter_cognition);
        server.executeCommand(this, cmd_parameter_motion);
    }
    
    @FXML
    protected void updateModules() {
        moduleTree.getRoot().getChildren().clear();
        server.executeCommand(this, cmd_modules_cognition);
        server.executeCommand(this, cmd_modules_motion);
    }
    
    @FXML
    protected void updateDebugRequests() {
        debugTree.getRoot().getChildren().clear();
        server.executeCommand(this, cmd_debug_cognition);
        server.executeCommand(this, cmd_debug_motion);
    }
    
    @FXML
    protected void saveModules() {
        server.executeCommand(this, cmd_modules_cognition_store);
        server.executeCommand(this, cmd_modules_motion_store);
    }
    
    @FXML
    protected void saveParameters() {
        System.out.println("saveParameters");
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
        TreeItem<Parameter> root = parameterTree.getRoot().getChildren().get(1);
        root.getChildren().clear();
        root.setExpanded(true);
        Arrays.asList(new String(result).split("\n")).forEach((p) -> {
            TreeItem<Parameter> parameter = new TreeItem<>(new Parameter(p, null));
            cognitionParameter.put(p, parameter);
            root.getChildren().add(parameter);
            server.executeCommand(this, new Command(cmd_parameter_cognition_get).addArg("<name>", p));
        });
    }
    
    private void handleMotionParameterResponse(byte[] result) {
        TreeItem<Parameter> root = parameterTree.getRoot().getChildren().get(0);
        root.getChildren().clear();
        root.setExpanded(true);
        Arrays.asList(new String(result).split("\n")).forEach((p) -> {
            TreeItem<Parameter> parameter = new TreeItem<>(new Parameter(p, null));
            motionParameter.put(p, parameter);
            root.getChildren().add(parameter);
            server.executeCommand(this, new Command(cmd_parameter_motion_get).addArg("<name>", p));
        });
    }
    
    private void handleCognitionParameterGetResponse(String param, byte[] result) {
        TreeItem<Parameter> treeItem = cognitionParameter.get(param);
        if(treeItem != null) {
            Arrays.asList(new String(result).split("\n")).forEach((p) -> {
                String[] parts = p.split("=");
                if(parts.length == 2) {
                    treeItem.getChildren().add(new TreeItem<>(new Parameter(parts[0], parts[1])));
                } else {
                    System.err.println("Invalid cognition parameter: " +Arrays.asList(parts));
                }
            });
        }
    }
    
    private void handleMotionParameterGetResponse(String param, byte[] result) {
        TreeItem<Parameter> treeItem = motionParameter.get(param);
        if(treeItem != null) {
            Arrays.asList(new String(result).split("\n")).forEach((p) -> {
                String[] parts = p.split("=");
                if(parts.length == 2) {
                    treeItem.getChildren().add(new TreeItem<>(new Parameter(parts[0], parts[1])));
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
            server.executeCommand(this, command);
        }
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
    }
}
