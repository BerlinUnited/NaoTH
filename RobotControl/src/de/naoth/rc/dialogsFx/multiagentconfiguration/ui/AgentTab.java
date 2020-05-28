package de.naoth.rc.dialogsFx.multiagentconfiguration.ui;

import de.naoth.rc.core.manager.ObjectListener;
import de.naoth.rc.core.server.Command;
import de.naoth.rc.core.server.ConnectionStatusEvent;
import de.naoth.rc.core.server.ConnectionStatusListener;
import de.naoth.rc.core.server.MessageServer;
import de.naoth.rc.core.server.ResponseListener;
import de.naoth.rc.manager.GenericManager;
import java.io.File;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.application.Platform;
import javafx.beans.property.DoubleProperty;
import javafx.beans.value.ChangeListener;
import javafx.event.ActionEvent;
import javafx.event.Event;
import javafx.event.EventHandler;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.control.ContentDisplay;
import javafx.scene.control.MenuButton;
import javafx.scene.control.MenuItem;
import javafx.scene.control.Tab;
import javafx.scene.control.TabPane;
import javafx.scene.control.Tooltip;
import javafx.scene.text.Text;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class AgentTab extends Tab implements ConnectionStatusListener
{
    private String host;
    private int port;
    private AllTab all;
    private MessageServer server = new MessageServer();
    
    private MenuItem miConnection = new MenuItem("Connect");
    private MenuItem miClose = new MenuItem("Close");

    private EventHandler<ActionEvent> menuConnectHandler = (e) -> { connect(); }; 
    private EventHandler<ActionEvent> menuDisconnectHandler = (e) -> { disconnect(); };
    
    @FXML protected ConfigurationsTab configurationsTabViewController;
    @FXML protected RepresentationsTab representationsTabViewController;
    
    @FXML protected TabPane tabs;
    
    // listeners and handlers for various ui elements
    private EventHandler<ActionEvent> updateModules = (e) -> { configurationsTabViewController.updateModules(); };
    private EventHandler<ActionEvent> updateParameters = (e) -> { configurationsTabViewController.updateParameters(); };
    private EventHandler<ActionEvent> updateDebugRequests = (e) -> { configurationsTabViewController.updateDebugRequests(); };
    private EventHandler<ActionEvent> updateRepresentationList = (e) -> { representationsTabViewController.updateList(); };
    private EventHandler<ActionEvent> saveModules = (e) -> { configurationsTabViewController.saveModules(); };
    private ChangeListener<Number> tabsListener = (ob, o, n) -> { tabs.getSelectionModel().select(n.intValue()); };
    private ChangeListener<Number> typeListener = (ob, o, n) -> { representationsTabViewController.type.getSelectionModel().select(n.intValue()); };
    private ChangeListener<String> listListener = (ob, o, n) -> {
        if(n != null) {
            // make sure it is the same representation type
            representationsTabViewController.type.getSelectionModel().select(all.type.getSelectionModel().getSelectedIndex());
            // select item in list
            representationsTabViewController.list.getSelectionModel().select(n);
        }
    };
    private ChangeListener<File> behaviorFileListener = (ob, o, n) -> { if(n != null) { configurationsTabViewController.behaviorFile.set(n); } };
    
    public AgentTab() {
        super();
        
        try {
            FXMLLoader loader = new FXMLLoader(AgentTab.class.getResource("AgentTab.fxml"));
            loader.setController(this);
            setContent(loader.load());
        } catch (IOException ex) {
            Logger.getLogger(getClass().getName()).log(Level.SEVERE, null, ex);
        }

        // creates the tab menu for closing/disconnecting ....
        setGraphic(createTabButton());
        
        representationsTabViewController.setParent(this);
        configurationsTabViewController.setParent(this);
    }
    
    public AgentTab(String host, int port, AllTab all)  {
        this();
        
        this.host = host;
        this.port = port;
        this.all = all;
        
        setTitle("");
        
        server.addConnectionStatusListener(this);
        
        setOnCloseRequest((e) -> {
            disconnect();
        });
        
        connect();
        bindUiElements();
        updateUiElements();
    }
    
    private void setTitle(String prefix) {
        setText(prefix + host + ":" + port);
    }
    
    protected MenuButton createTabButton() {
        MenuButton b = new MenuButton();
        b.setContentDisplay(ContentDisplay.TEXT_ONLY); // set via css
        b.setGraphicTextGap(0);
        b.setFocusTraversable(false);
        b.getItems().add(miConnection);
        b.getItems().add(miClose);
        
        miConnection.setOnAction(menuConnectHandler);
        
        miClose.setOnAction((e) -> {
            requestClose();
        });
        
        return b;
    }
    
    private void bindUiElements() {
        // bind configuration dividers
        for (int i = 0; i < configurationsTabViewController.splitPane.getDividers().size(); i++) {
            DoubleProperty p1 = configurationsTabViewController.splitPane.getDividers().get(i).positionProperty();
            DoubleProperty p2 = all.configurationsTabViewController.splitPane.getDividers().get(i).positionProperty();
            p1.bindBidirectional(p2);
        }
        
        configurationsTabViewController.agentList.valueProperty().bind(all.configurationsTabViewController.agentList.valueProperty());
        representationsTabViewController.split.getDividers().get(0).positionProperty().bindBidirectional(all.split.getDividers().get(0).positionProperty());
        
        all.tabs.getSelectionModel().selectedIndexProperty().addListener(tabsListener);
        
        all.addRepresentationsView(representationsTabViewController.content, getText());
        
        all.type.getSelectionModel().selectedIndexProperty().addListener(typeListener);
        
        all.list.getSelectionModel().selectedItemProperty().addListener(listListener);
        
        all.btnUpdateList.addEventHandler(ActionEvent.ACTION, updateRepresentationList);
        all.configurationsTabViewController.btnUpdateDebug.addEventHandler(ActionEvent.ACTION, updateDebugRequests);
        all.configurationsTabViewController.btnUpdateModules.addEventHandler(ActionEvent.ACTION, updateModules);
        all.configurationsTabViewController.btnUpdateParameters.addEventHandler(ActionEvent.ACTION, updateParameters);
        all.configurationsTabViewController.btnSaveModules.addEventHandler(ActionEvent.ACTION, saveModules);
        all.configurationsTabViewController.behaviorFile.addListener(behaviorFileListener);
    }
    
    private void unbindUiElements() {
        for (int i = 0; i < configurationsTabViewController.splitPane.getDividers().size(); i++) {
            DoubleProperty p1 = configurationsTabViewController.splitPane.getDividers().get(i).positionProperty();
            DoubleProperty p2 = all.configurationsTabViewController.splitPane.getDividers().get(i).positionProperty();
            p1.unbindBidirectional(p2);
        }
        
        configurationsTabViewController.agentList.valueProperty().unbind();
        representationsTabViewController.split.getDividers().get(0).positionProperty().unbindBidirectional(all.split.getDividers().get(0).positionProperty());
        
        all.tabs.getSelectionModel().selectedIndexProperty().removeListener(tabsListener);
        
        all.removeRepresentationsView(getText());
        
        all.type.getSelectionModel().selectedIndexProperty().removeListener(typeListener);
        
        all.list.getSelectionModel().selectedItemProperty().removeListener(listListener);
        
        all.btnUpdateList.removeEventHandler(ActionEvent.ACTION, updateRepresentationList);
        all.configurationsTabViewController.btnUpdateDebug.removeEventHandler(ActionEvent.ACTION, updateDebugRequests);
        all.configurationsTabViewController.btnUpdateModules.removeEventHandler(ActionEvent.ACTION, updateModules);
        all.configurationsTabViewController.btnUpdateParameters.removeEventHandler(ActionEvent.ACTION, updateParameters);
        all.configurationsTabViewController.btnSaveModules.removeEventHandler(ActionEvent.ACTION, saveModules);
        all.configurationsTabViewController.behaviorFile.removeListener(behaviorFileListener);
    }
    
    private void updateUiElements() {
        // switch tabs (if necessary)
        tabs.getSelectionModel().select(all.tabs.getSelectionModel().getSelectedIndex());
        // set the representations type if they're different
        if(all.type.getSelectionModel().getSelectedIndex() != representationsTabViewController.type.getSelectionModel().getSelectedIndex()) {
            representationsTabViewController.type.getSelectionModel().select(all.type.getSelectionModel().getSelectedIndex());
        }
        // select the representation if on is selected in the all tab
        if(all.list.getSelectionModel().getSelectedItem() != null) {
            representationsTabViewController.list.getSelectionModel().select(all.list.getSelectionModel().getSelectedItem());
        }
    }
    
    public boolean sendCommand(Command cmd, ResponseListener l) {
        if(server.isConnected()) {
            server.executeCommand(l, cmd);
            return true;
        }
        return false;
    }
    
    public boolean sendCommand(String cmd, Map<String, byte[]> args, ResponseListener l) {
        Command command = new Command(cmd);
        command.setArguments(args);
        return sendCommand(command, l);
    }
    
    private final HashMap<Command, GenericManager> subscription = new HashMap<>();
    private final HashMap<ObjectListener, GenericManager> subscriptionCallback = new HashMap<>();
    
    public void subscribeCommand(Command cmd, ObjectListener<byte[]> callback) {
        if(!subscription.containsKey(cmd)) {
            GenericManager manager = new GenericManager(server, cmd);
            subscription.put(cmd, manager);
            subscriptionCallback.put(callback, manager);
        }
        subscription.get(cmd).addListener(callback);
    }
    
    public void unsubscribeCommand(ObjectListener<byte[]> callback) {
        if(subscriptionCallback.containsKey(callback)) {
            subscriptionCallback.get(callback).removeListener(callback);
            subscriptionCallback.remove(callback);
        }
    }
    
    public void requestClose() {
        unbindUiElements();
        // the event handler are not triggered if the tab is removed programmatically
        // so the appropiate events must be triggered here
        Event.fireEvent(this, new Event(this, this, Tab.TAB_CLOSE_REQUEST_EVENT));        
        if (this.getTabPane().getTabs().remove(this)) {
            Event.fireEvent(this, new Event(this, this, Tab.CLOSED_EVENT));
        }
    }

    public void connect() {
        if(!server.isConnected()) {
            // try to connect to robot
            if(server.connect(host, port)) {
                // send "initial" commands
                configurationsTabViewController.connected();
            }
        }
    }
    
    @Override
    public void connected(ConnectionStatusEvent event) {
        updateConnectionStatusUI(true);
    }
    
    public void disconnect() {
        server.disconnect();
    }

    @Override
    public void disconnected(ConnectionStatusEvent event) {
        updateConnectionStatusUI(false);
    }
    
    private void updateConnectionStatusUI(boolean connected) {
        if(connected) {
            miConnection.setText("Disconnect");
            miConnection.setOnAction(menuDisconnectHandler);
            Platform.runLater(() -> {
                setTooltip(new Tooltip("Connected to " + getText()));
                if(getTabPane() != null) {
                    getTabPane().lookupAll(".tab-label .text").forEach((t) -> {
                        if(((Text)t).getText().equals(getText())) {
                            ((Text)t).setStyle("");
                        }
                    });
                }
            });
        } else {
            miConnection.setText("Connect");
            miConnection.setOnAction(menuConnectHandler);
            Platform.runLater(() -> {
                setTooltip(new Tooltip("Disconnected from " + getText()));
                if(getTabPane() != null) {
                    // strike through the label of the disconnected tab
                    getTabPane().lookupAll(".tab-label .text").forEach((t) -> {
                        if(((Text)t).getText().equals(getText())) {
                            ((Text)t).setStyle("-fx-strikethrough: true;");
                        }
                    });
                }
            });
        }
        configurationsTabViewController.updateConnectionStatusUI(connected);
    }
    
    @FXML
    protected void representationsTabSelect(Event e) {
        representationsTabViewController.show(((Tab) e.getSource()).isSelected());
    }
    
    public String getHost() {
        return host;
    }
    
    public int getPort() {
        return port;
    }
}
