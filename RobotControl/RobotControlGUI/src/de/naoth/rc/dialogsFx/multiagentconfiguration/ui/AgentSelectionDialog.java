package de.naoth.rc.dialogsFx.multiagentconfiguration.ui;

import de.naoth.rc.dialogsFx.multiagentconfiguration.AgentItem;
import java.io.IOException;
import java.net.InetAddress;
import java.net.Socket;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.function.IntConsumer;
import java.util.regex.Pattern;
import java.util.stream.Collectors;
import java.util.stream.IntStream;
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
    private final ObservableList<AgentItem> hostList = FXCollections.synchronizedObservableList(FXCollections.observableArrayList());
    private final List<AgentItem> existing = new ArrayList<>();

    private final ExecutorService executor = Executors.newCachedThreadPool();
    
    private List<String> hosts;
    
    private TextField host;
    private TextField port;
    private Button addHost;
    private CheckBox allAgents;
    
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
        
        setOnShowing((e) -> { onShowing(); });
    }

    public AgentSelectionDialog() {
        this(new ArrayList<>());
    }
        
    private void initUi() {
        VBox vb = new VBox();
        
        ListView<AgentItem> l = new ListView(hostList);
        //l.setCellFactory(CheckBoxListCell.forListView((param) -> { return param.activeProperty(); }));
        l.setCellFactory((ListView<AgentItem> list) -> {
            // create new cell and bind to agent active property
            CheckBoxListCell<AgentItem> c = new CheckBoxListCell<>((p) -> { return p.activeProperty(); });
            // set mouse listener to cell
            c.setOnMouseClicked((e) -> {
                c.getItem().activeProperty().set(!c.getItem().activeProperty().get());
            });
            return c;
        });
        // enable selection via 'space' key
        l.setOnKeyReleased((e) -> {
            if(e.getCode() == KeyCode.SPACE) {
                AgentItem a = l.getSelectionModel().getSelectedItem();
                a.activeProperty().set(!a.activeProperty().get());
            }
        });
        vb.getChildren().add(l);
        
        host = new TextField();
        host.setPromptText("host ip");
        host.setOnKeyPressed((e) -> {
            if(e.getCode() == KeyCode.ENTER) {
                host.commitValue();
                addHost.fire();
                e.consume(); // prevent closing dialog, when host textfield has focus
            }
        });
        
        Label sep = new Label(" : ");
        sep.setAlignment(Pos.BOTTOM_CENTER);
        
        port = new TextField();
        port.setPromptText("port");
        port.setPrefColumnCount(5);
        port.setOnKeyPressed((e) -> {
            if(e.getCode() == KeyCode.ENTER) {
                port.commitValue();
                addHost.fire();
                e.consume(); // prevent closing dialog, when host textfield has focus
            }
        });
        
        addHost = new Button("+");
        // prevent closing dialog, when host textfield has focus
        addHost.setOnKeyPressed((e) -> { if(e.getCode() == KeyCode.ENTER) { e.consume(); } });
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
        
        allAgents = new CheckBox("All Agents");
        allAgents.setSelected(true);
        GridPane.setVgrow(allAgents, Priority.ALWAYS);
        allAgents.setOnAction((e) -> {
            hostList.stream().forEach((a) -> {
                a.activeProperty().set(allAgents.isSelected());
            });
        });
        bp.setLeft(allAgents);
        
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
    
    private void onShowing() {
        // clear "old" list
        hostList.clear();
        existing.forEach((a) -> { addAgent(a); });
        // clear textfields
        resetTextFields();
        // retrieve agents
        pingAgents();
        // set to a fixed size
        setHeight(500);
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
        // function to detect a simspark agent on the given port
        IntConsumer detectSimsparkAgents = (int port) -> {
            try {
                // NOTE: throws an exception, if the port isn't used.
                (new Socket("localhost", port)).close();
                Platform.runLater(() -> {
                        // check if agent is already in the list
                        addAgent(new AgentItem("127.0.0.1", port));
                });
            } catch (IOException ex) { /* ignore exception */ }
        };
        // try to find some simspark agents
        IntStream.rangeClosed(5401, 5411).parallel().forEach(detectSimsparkAgents);
        IntStream.rangeClosed(5501, 5511).parallel().forEach(detectSimsparkAgents);
    } // END pingAgents()
    
    private boolean addAgent(AgentItem agent) {
        if(!hostList.stream().anyMatch((other) -> { return agent.getHost().equals(other.getHost()) && agent.getPort() == other.getPort(); })) {
            // deactivate agent item, if it already open
            agent.activeProperty().set(!existing.stream().anyMatch((i) -> { return i.compareTo(agent) == 0; }));
            hostList.add(agent);
            return true;
        }
        return false;
    }
    
    public void setExisting(List<AgentItem> l) {
        existing.clear();
        existing.addAll(l);
    }
} // END AgentSelectionDialog