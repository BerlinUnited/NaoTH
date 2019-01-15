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
import javafx.beans.property.DoubleProperty;
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
        
        all.tabs.getSelectionModel().selectedIndexProperty().addListener((ob, o, n) -> {
            tabs.getSelectionModel().select(n.intValue());
        });
        
        all.addRepresentationsView(representationsTabViewController.content, getText());
        
        all.type.getSelectionModel().selectedIndexProperty().addListener((ob, o, n) -> {
            representationsTabViewController.type.getSelectionModel().select(n.intValue());
        });
        
        all.list.getSelectionModel().selectedItemProperty().addListener((ob, o, n) -> {
            if(n != null) {
                // make sure it is the same representation type
                representationsTabViewController.type.getSelectionModel().select(all.type.getSelectionModel().getSelectedIndex());
                // select item in list
                representationsTabViewController.list.getSelectionModel().select(n.toString());
            }
        });
        
        all.btnUpdateList.addEventHandler(ActionEvent.ACTION, (e) -> { representationsTabViewController.updateList(); });
        all.configurationsTabViewController.btnUpdateDebug.addEventHandler(ActionEvent.ACTION, (e) -> { configurationsTabViewController.updateDebugRequests(); });
        all.configurationsTabViewController.btnUpdateModules.addEventHandler(ActionEvent.ACTION, (e) -> { configurationsTabViewController.updateModules(); });
        all.configurationsTabViewController.btnUpdateParameters.addEventHandler(ActionEvent.ACTION, (e) -> { configurationsTabViewController.updateParameters(); });
        all.configurationsTabViewController.btnSaveModules.addEventHandler(ActionEvent.ACTION, (e) -> { configurationsTabViewController.saveModules(); });
        all.configurationsTabViewController.behaviorFile.addListener((ob, o, n) -> {
            if(n != null) { configurationsTabViewController.behaviorFile.set(n); }
        });
    }
    
    private void unbindUiElements() {
        all.removeRepresentationsView(getText());
        // TODO: remove event handlers & bindings?!
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
}
