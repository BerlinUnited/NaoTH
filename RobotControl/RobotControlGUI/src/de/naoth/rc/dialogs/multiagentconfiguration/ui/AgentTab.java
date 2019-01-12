package de.naoth.rc.dialogs.multiagentconfiguration.ui;

import com.sun.javafx.scene.control.behavior.TabPaneBehavior;
import com.sun.javafx.scene.control.skin.TabPaneSkin;
import de.naoth.rc.core.manager.ObjectListener;
import de.naoth.rc.server.Command;
import de.naoth.rc.server.ConnectionStatusEvent;
import de.naoth.rc.server.ConnectionStatusListener;
import de.naoth.rc.server.MessageServer;
import de.naoth.rc.server.ResponseListener;
import de.naoth.rc.manager.GenericManager;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.application.Platform;
import javafx.event.ActionEvent;
import javafx.event.Event;
import javafx.event.EventHandler;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.control.ContentDisplay;
import javafx.scene.control.MenuButton;
import javafx.scene.control.MenuItem;
import javafx.scene.control.Tab;
import javafx.scene.control.Tooltip;
import javafx.scene.text.Text;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class AgentTab extends Tab implements ConnectionStatusListener
{
    private String host;
    private int port;
    private MessageServer server = new MessageServer();
    
    private MenuItem miConnection = new MenuItem("Connect");
    private MenuItem miClose = new MenuItem("Close");

    private EventHandler<ActionEvent> menuConnectHandler = (e) -> { connect(); }; 
    private EventHandler<ActionEvent> menuDisconnectHandler = (e) -> { disconnect(); };
    
    @FXML protected ConfigurationsTab configurationsTabViewController;
    @FXML protected RepresentationsTab representationsTabViewController;
    
    public AgentTab() {
        super();
        
        try {
            FXMLLoader loader = new FXMLLoader(AgentTab.class.getResource("/de/naoth/rc/dialogs/multiagentconfiguration/ui/AgentTab.fxml"));
            loader.setController(this);
            setContent(loader.load());
        } catch (IOException ex) {
            Logger.getLogger(Tab.class.getName()).log(Level.SEVERE, null, ex);
        }

        // creates the tab menu for closing/disconnecting ....
        setGraphic(createTabButton());
        
        representationsTabViewController.setParent(this);
        configurationsTabViewController.setParent(this);
    }
    
    public AgentTab(String host, int port)  {
        this();
        
        this.host = host;
        this.port = port;
        setTitle("");
        
        server.addConnectionStatusListener(this);
        
        setOnCloseRequest((e) -> {
            disconnect();
        });
        
        connect();
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
        TabPaneBehavior behavior = getBehavior();
        if(behavior.canCloseTab(this)) {
            behavior.closeTab(this);
        }
    }

    private TabPaneBehavior getBehavior() {
        return ((TabPaneSkin) getTabPane().getSkin()).getBehavior();
    }

    private void connect() {
        try {
            // try to connect to robot
            server.connect(host, port);
            // send "initial" commands
            configurationsTabViewController.connected();
        } catch (IOException ex) {
            updateConnectionStatusUI(false);
            
            Logger.getLogger(Tab.class.getName()).log(Level.WARNING, "Can not connect to robot.");
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

    /*
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
    public void connectTabs(AgentTab other) {
        other.tabs.getSelectionModel().selectedIndexProperty().addListener((observable, oldValue, newValue) -> {
            tabs.getSelectionModel().select(newValue.intValue());
        });
    }
    */
}
