package de.naoth.rc.dialogs.multiagentconfiguration.ui;

import de.naoth.rc.dialogs.multiagentconfiguration.AgentItem;
import java.io.IOException;
import java.net.InetAddress;
import java.net.Socket;
import java.util.ArrayList;
import java.util.List;
import java.util.TreeSet;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.regex.Pattern;
import java.util.stream.Collectors;
import javafx.application.Platform;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.control.Button;
import javafx.scene.control.ButtonType;
import javafx.scene.control.CheckBox;
import javafx.scene.control.Dialog;
import javafx.scene.control.Label;
import javafx.scene.control.ListView;
import javafx.scene.control.TextField;
import javafx.scene.control.Tooltip;
import javafx.scene.control.cell.CheckBoxListCell;
import javafx.scene.input.KeyCode;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.Priority;
import javafx.scene.layout.VBox;
import javafx.stage.StageStyle;

/**
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class AgentSelectionDialog extends Dialog
{
    private final ObservableList<AgentItem> hostList = FXCollections.observableArrayList(new TreeSet());
//    private final ObservableMap<String, AgentItem> hostList = FXCollections.observableHashMap();

    private final ExecutorService executor = Executors.newCachedThreadPool();
    
    private List<String> hosts;
    
    private TextField host;
    private TextField port;
    private Button addHost;
    
    private Pattern ip_pattern = Pattern.compile("\\b(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."+
                                                 "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\." +
                                                 "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\." +
                                                 "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");

    public AgentSelectionDialog(List<String> ips) {
        setTitle("Agent Selection");
        initStyle(StageStyle.UTILITY);
        getDialogPane().getStylesheets().add(getClass().getResource("style.css").toExternalForm());
        
        this.hosts = ips;
        
        initUi();
        
        setResultConverter(dialogButton -> {
            if (dialogButton == ButtonType.OK) {
                return hostList.stream().filter(AgentItem::getActive).collect(Collectors.toList());
            }
            return new ArrayList<>();
        });
        
        setOnShowing((e) -> {
            // clear "old" list
            hostList.clear();
            // clear textfields
            resetTextFields();
            // retrieve agents
            pingAgents();
        });
    }

    public AgentSelectionDialog() {
        this(new ArrayList<>());
    }
    
    private void initUi() {
        VBox vb = new VBox();
        
        ListView<AgentItem> l = new ListView(hostList);
        l.setCellFactory(CheckBoxListCell.forListView((param) -> { return param.activeProperty(); }));
        vb.getChildren().add(l);
        
        host = new TextField();
        host.setPromptText("host ip");
        host.setOnKeyReleased((e) -> {
            if(e.isControlDown() && e.getCode() == KeyCode.ENTER) {
                host.commitValue();
                addHost.fire();
            }
        });
        
        Label sep = new Label(" : ");
        sep.setAlignment(Pos.BOTTOM_CENTER);
        
        port = new TextField();
        port.setPromptText("port");
        port.setPrefColumnCount(5);
        port.setOnKeyReleased((e) -> {
            if(e.isControlDown() && e.getCode() == KeyCode.ENTER) {
                port.commitValue();
                addHost.fire();
            }
        });
        
        addHost = new Button("+");
        addHost.setOnAction((e) -> {
            boolean error = false;
            String rawHost = host.getText().trim();
            String rawPort = port.getText().trim();
            int p = 0;
            // check host ip format is valid
            if(!ip_pattern.matcher(rawHost).matches()) {
                host.setStyle("-fx-border-color:red;");
                error = true;
            }
            // check if port is valid
            try {
                p = Integer.parseInt(rawPort);
            } catch (NumberFormatException ex) {
                port.setStyle("-fx-border-color:red;");
                error = true;
            }
            // if there wasn't an error, add host to agent list
            if(!error) {
                // reset style
                host.setStyle("");
                port.setStyle("");
                addAgent(new AgentItem(rawHost, p));
            }
        });
        
        HBox hb = new HBox();
        hb.setAlignment(Pos.CENTER);
        hb.getChildren().addAll(host, sep, port, addHost);
        vb.getChildren().add(hb);
        
        BorderPane bp = new BorderPane();
        bp.setPadding(new Insets(10, 0, 0, 0));
        
        CheckBox cb = new CheckBox("All Agents");
        GridPane.setVgrow(cb, Priority.ALWAYS);
        bp.setLeft(cb);
        
        Button refresh = new Button();
        refresh.setTooltip(new Tooltip("Refresh agents list"));
        refresh.getStyleClass().add("refresh_button"); // TODO: doesn't work ?!
        refresh.setOnAction((e) -> { pingAgents(); });
        bp.setRight(refresh);
        
        vb.getChildren().add(bp);
        
        getDialogPane().setContent(vb);
        getDialogPane().getButtonTypes().addAll(ButtonType.OK, ButtonType.CANCEL);
        //((Button)getDialogPane().lookupButton(ButtonType.OK)).setDefaultButton(false);
    }
    
    private void resetTextFields() {
        host.setText("");
        port.setText("5401");
    }
    
    public void setHosts(List<String> h) {
        hosts = h;
    }
    
    private void pingAgents() {
        // try to find some naos
        for(final String ip: hosts) {
            executor.submit(() -> {
                try {
                    InetAddress address = InetAddress.getByName(ip);
                    if(address.isReachable(500)) {
                        Platform.runLater(() -> {
                            // check if agent is already in the list
                            addAgent(new AgentItem(ip));
                        });
                    }
                } catch (IOException e) { /* ignore exception */ }
            });
        }
        // try to find some simspark agents
        for (int p = 5401; p < 5411; p++) {
            final int port = p;
            executor.submit(() -> {
                try {
                    // NOTE: throws an exception, if the port isn't used.
                    (new Socket("localhost", port)).close();
                    Platform.runLater(() -> {
                            // check if agent is already in the list
                            addAgent(new AgentItem("localhost", port));
                    });
                } catch (IOException ex) { /* ignore exception */ }
            });
        }
    } // END pingAgents()
    
    private boolean addAgent(AgentItem agent) {
        if(!hostList.stream().anyMatch((other) -> { return agent.getHost().equals(other.getHost()) && agent.getPort() == other.getPort(); })) {
            hostList.add(agent);
            return true;
        }
        return false;
    }
} // END AgentSelectionDialog