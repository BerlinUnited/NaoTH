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
import java.util.stream.Collectors;
import javafx.application.Platform;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.collections.ObservableMap;
import javafx.event.Event;
import javafx.event.EventHandler;
import javafx.scene.control.Button;
import javafx.scene.control.ButtonType;
import javafx.scene.control.CheckBox;
import javafx.scene.control.Dialog;
import javafx.scene.control.ListView;
import javafx.scene.control.Tooltip;
import javafx.scene.control.cell.CheckBoxListCell;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.HBox;
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

    public AgentSelectionDialog(List<String> ips) {
        setTitle("Agent Selection");
        initStyle(StageStyle.UTILITY);
        
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
        
        BorderPane bp = new BorderPane();
        CheckBox cb = new CheckBox("All Agents");
        bp.setLeft(cb);
        
        HBox hb = new HBox();
        // TODO: add input field for manual robot connection
        
        Button refresh = new Button();
        refresh.setTooltip(new Tooltip("Refresh agents list"));
        refresh.getStyleClass().add("refresh_button"); // TODO: doesn't work ?!
        refresh.setOnAction((e) -> { pingAgents(); });
        bp.setRight(refresh);
        
        vb.getChildren().add(bp);
        
        getDialogPane().setContent(vb);
        getDialogPane().getButtonTypes().addAll(ButtonType.OK, ButtonType.CANCEL);
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
                            AgentItem agent = new AgentItem(ip);
                            if(!hostList.stream().anyMatch((other) -> { return agent.getHost().equals(other.getHost()) && agent.getPort() == other.getPort(); })) {
                                System.out.println(agent);
                                hostList.add(agent);
                            }
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
                            AgentItem agent = new AgentItem("localhost", port);
                            if(!hostList.stream().anyMatch((other) -> { return agent.getHost().equals(other.getHost()) && agent.getPort() == other.getPort(); })) {
                                System.out.println(agent);
                                hostList.add(agent);
                            }
                    });
                } catch (IOException ex) { /* ignore exception */ }
            });
        }
    } // END pingAgents()
} // END AgentSelectionDialog