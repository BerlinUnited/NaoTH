package de.naoth.rc.dialogs.multiagentconfiguration;

import de.naoth.rc.RobotControl;
import de.naoth.rc.core.dialog.AbstractJFXDialog;
import de.naoth.rc.core.dialog.DialogPlugin;
import de.naoth.rc.core.dialog.RCDialog;
import de.naoth.rc.dialogs.multiagentconfiguration.ui.AgentTab;
import de.naoth.rc.dialogs.multiagentconfiguration.ui.AgentTabGlobal;
import java.awt.SplashScreen;
import java.io.IOException;
import java.net.InetAddress;
import java.net.Socket;
import java.net.URL;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Optional;
import java.util.Properties;
import java.util.TreeSet;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.stream.Collectors;
import javafx.application.Application;
import static javafx.application.Application.launch;
import javafx.application.Platform;
import javafx.beans.property.BooleanProperty;
import javafx.beans.property.IntegerProperty;
import javafx.beans.property.SimpleBooleanProperty;
import javafx.beans.property.SimpleIntegerProperty;
import javafx.beans.property.SimpleStringProperty;
import javafx.beans.property.StringProperty;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.Node;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.ButtonType;
import javafx.scene.control.CheckBox;
import javafx.scene.control.Dialog;
import javafx.scene.control.ListView;
import javafx.scene.control.Tab;
import javafx.scene.control.TabPane;
import javafx.scene.control.ToggleButton;
import javafx.scene.control.cell.CheckBoxListCell;
import javafx.scene.image.Image;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyCodeCombination;
import javafx.scene.input.KeyCombination;
import javafx.scene.layout.Pane;
import javafx.scene.layout.StackPane;
import javafx.scene.layout.VBox;
import javafx.stage.Stage;
import javafx.stage.StageStyle;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class MultiAgentConfigurationFx extends AbstractJFXDialog
{
    
    @RCDialog(category = RCDialog.Category.Tools, name = "MultiAgentConfigurationFx")
    @PluginImplementation
    public static class Plugin extends DialogPlugin<MultiAgentConfigurationFx> {
        @InjectPlugin
        public static RobotControl parent;
    }

    @FXML
    private ToggleButton btn_connect;
    
    @FXML
    private TabPane tabpane;
    
    private final AgentTabGlobal allTab = new AgentTabGlobal();
    
    private Properties config = new Properties();
    
    private final ExecutorService executor = Executors.newCachedThreadPool();
    
    private final ObservableList<AgentItem> hostList = FXCollections.observableArrayList(new TreeSet());
    
    @Override
    protected boolean isSelfController() {
        return true;
    }

    @Override
    public URL getFXMLRessource() {
        return getClass().getResource("MultiAgentConfigurationFx.fxml");
    }

    @Override
    public void afterInit() {
        if(Plugin.parent != null) {
            setConfig(Plugin.parent.getConfig());
        }
        // TODO
        btn_connect.selectedProperty().addListener((ov, t, t1) -> {
            if(t1) {
                hostList.clear();
                String[] ips = config.getProperty("iplist","").split(",");

                for(final String ip: ips) {
                    executor.submit(() -> {
                        try {
                            InetAddress address = InetAddress.getByName(ip);
                            if(address.isReachable(500)) {
                                Platform.runLater(() -> hostList.add(new AgentItem(ip)));
                            }
                        } catch (IOException e) { /* ignore exception */ }
                    });
                }
                for (int p = 5401; p < 5411; p++) {
                    final int port = p;
                    executor.submit(() -> {
                        try {
                            (new Socket("localhost", port)).close();
                            Platform.runLater(() -> hostList.add(new AgentItem("localhost", port)));
                        } catch (IOException ex) { /* ignore exception */ }
                    });

                }
                VBox vb = new VBox();
                CheckBox cb = new CheckBox("All Agents");
    //            cb.selectedProperty().bind(Bindings.when(Condi));
    //            Bindings.createBooleanBinding(hostList.stream().allMatch(AgentItem::getActive), dependencies)

                ListView<AgentItem> l = new ListView(hostList);
                l.setCellFactory(CheckBoxListCell.forListView((param) -> { return param.activeProperty(); }));
                vb.getChildren().add(l);
                vb.getChildren().add(cb);
                Dialog<List<AgentItem>> dialog = new Dialog<>();
                dialog.setTitle("Agent Selection");
                dialog.initStyle(StageStyle.UTILITY);
    //            dialog.setHeaderText("Select the agents  you want to connect to.");
                dialog.getDialogPane().setContent(vb);
                dialog.getDialogPane().getButtonTypes().addAll(ButtonType.OK, ButtonType.CANCEL);

                dialog.setResultConverter(dialogButton -> {
                    if (dialogButton == ButtonType.OK) {
                        return hostList.stream().filter(AgentItem::getActive).collect(Collectors.toList());
                    }
                    return null;
                });

                Optional<List<AgentItem>> result = dialog.showAndWait();
                result.ifPresent((al) -> {
                    connecting(al);
                });
                // TODO: check if there's something to connect; use binding?!
                allTab.setDisable(false);
            } else {
                disconnecting();
                allTab.setDisable(true);
            }
        });
        
        allTab.setDisable(true);
        tabpane.getTabs().add(allTab);
//        tabpane.getTabs().add(new Tab("+"));
        tabpane.setTabClosingPolicy(TabPane.TabClosingPolicy.UNAVAILABLE);
        tabpane.getSelectionModel().select(allTab);
        
        final Button  tabButton = new Button("+");
        
        Pane controlButtons = (Pane) tabpane.lookup(".tab-header-background");
        System.out.println(controlButtons);
        System.out.println(tabpane.lookup(".tab-header-area"));
        dump(tabpane, 2);
        /*
        controlButtons.getChildren().add(tabButton);
        StackPane.setAlignment(tabButton, Pos.CENTER_LEFT);

        Pane headersRegion = (Pane) tabpane.lookup(".headers-region");
        System.out.println(headersRegion.getWidth());
        tabButton.translateXProperty().bind(
            headersRegion.widthProperty().add(10)
        );
        */
    }
    // debugging routine to dump the scene graph.
  public  static void dump(Node n) { dump(n, 0); }
  private static void dump(Node n, int depth) {
    for (int i = 0; i < depth; i++) System.out.print("  ");
    System.out.println(n);
    if (n instanceof Parent) for (Node c : ((Parent) n).getChildrenUnmodifiable()) dump(c, depth + 1);
  }
    public void setConfig(Properties c) {
        config = c;
    }

    @Override
    public Map<KeyCombination, Runnable> getGlobalShortcuts() {
        HashMap<KeyCombination, Runnable> shortcuts = new HashMap<KeyCombination, Runnable>();
        shortcuts.put(new KeyCodeCombination(KeyCode.C, KeyCombination.CONTROL_DOWN, KeyCombination.SHIFT_DOWN), () -> {
            connect();
        });
        shortcuts.put(new KeyCodeCombination(KeyCode.D, KeyCombination.ALT_DOWN, KeyCombination.SHIFT_DOWN), () -> {
            disconnect();
        });
        return shortcuts;
    }
    
    @FXML
    public void connect() {
        btn_connect.selectedProperty().set(true);
    }
    
    public void connecting(List<AgentItem> l) {
        System.out.println("----" + tabpane.getSkin());
        System.out.println("----" + tabpane.lookup(".tab-header-area"));
        System.out.println("----" + tabpane.lookup(".headers-region")); // headers-region
        System.out.println("----" + tabpane.lookup(".tab-header-background")); //tab-header-background
        System.out.println("----" + tabpane.lookup(".control-buttons-tab")); //control-buttons-tab
        Pane p = new Pane();
        p.setMinWidth(100);
        p.setMinHeight(10);
        p.setStyle("-fx-background-color: red;");
//        tabpane.lookup(".tab-header-area").setStyle("-fx-background: red;");
        ((StackPane)tabpane.lookup(".tab-header-area")).getChildren().add(0,p);
        System.out.println(((StackPane)tabpane.lookup(".tab-header-area")).getChildren());
        System.out.println(((StackPane)tabpane.lookup(".headers-region")).getInsets());
        
        for (AgentItem agentItem : l) {
            // TODO: check if already connected!?
            AgentTab tab = new AgentTab(agentItem.getHost(), agentItem.getPort());
            tab.connectDivider(allTab);
            tab.connectAgentList(allTab);
            tab.connectButtons(allTab);
            tabpane.getTabs().add(tab);
        }
    }
    
    public void disconnect() {
        btn_connect.setSelected(false);
    }
    
    private void disconnecting() {
        // remove all, except the first ("all") tabs
        while (tabpane.getTabs().size() > 1) {
            Tab t = tabpane.getTabs().get(tabpane.getTabs().size()-1);
            if(t instanceof Tab) {
                ((AgentTab)t).requestClose();
            }
        }
        
        System.out.println("disconnect: " + tabpane.getTabs());
    }
    
    /**
     * Start method for the standalone MultiAgentConfigurationFx application.
     * 
     * @param args 
     */
    public static void main(String[] args) {
        launch(MultiAgentConfigurationFxMain.class, args);
    }
    
    /**
     * The standalone MultiAgentConfigurationFx application class.
     */
    public static class MultiAgentConfigurationFxMain extends Application
    {
        @Override
        public void start(Stage stage) throws Exception {
            try {
                FXMLLoader loader = new FXMLLoader(MultiAgentConfigurationFxMain.class.getResource("MultiAgentConfigurationFx.fxml"));
                MultiAgentConfigurationFx controller = new MultiAgentConfigurationFx();
                loader.setController(controller);
                
                Parent root = loader.load();
                controller.setConfig(readConfigFromFile());
                controller.afterInit();

                Scene scene = new Scene(root);

                stage.setScene(scene);
                stage.setMaximized(true);
                stage.getIcons().add(new Image(MultiAgentConfigurationFxMain.class.getResourceAsStream("/de/naoth/rc/res/nao-nice.png")));
                stage.setTitle("MultiAgentConfigurationFx");
                stage.show();

                stage.setOnCloseRequest((e) -> {
                    System.exit(0);
                });
                
                SplashScreen.getSplashScreen().close();
            } catch (IOException ex) {
                Logger.getLogger(MultiAgentConfigurationFxMain.class.getName()).log(Level.SEVERE, null, ex);
            }
        }

        private Properties readConfigFromFile() {
            Properties config = new Properties();
//          splashScreenMessage("Loading config file ...");
            try {
                // load main config
                Properties mainConfig = new Properties();
                mainConfig.load(MultiAgentConfigurationFxMain.class.getResourceAsStream("/de/naoth/rc/config"));
                config.putAll(mainConfig);

            } catch (IOException ex) {}
            return config;
        }
    }
    
    class AgentListCell extends CheckBoxListCell {
        
    }
    
    class AgentItem
    {
        private final StringProperty host = new SimpleStringProperty();
        private final IntegerProperty port = new SimpleIntegerProperty(5401);
        private final BooleanProperty active = new SimpleBooleanProperty(true);

        public AgentItem(String host) {
            this.host.set(host);
        }
        
        public AgentItem(String host, int port) {
            this.host.set(host);
            this.port.set(port);
        }
        
        public String getHost() { return host.get(); }
        public int getPort() { return port.get(); }
        public boolean getActive() { return active.get(); }
        public StringProperty hostProperty() { return host; }
        public IntegerProperty portProperty() { return port; }
        public BooleanProperty activeProperty() { return active; }

        @Override
        public String toString() {
            return getHost() + ":" + getPort();
        }
    }
}
