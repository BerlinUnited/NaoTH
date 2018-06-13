package de.naoth.rc.dialogs.multiagentconfiguration;

import de.naoth.rc.core.dialog.AbstractJFXDialog;
import de.naoth.rc.core.dialog.DialogPlugin;
import de.naoth.rc.core.dialog.RCDialog;
import de.naoth.rc.dialogs.multiagentconfiguration.ui.AgentTab;
import de.naoth.rc.dialogs.multiagentconfiguration.ui.AgentTabGlobal;
import java.awt.SplashScreen;
import java.io.IOException;
import java.net.URL;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Optional;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.application.Application;
import static javafx.application.Application.launch;
import javafx.event.EventType;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.geometry.Pos;
import javafx.scene.Node;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.ButtonBar.ButtonData;
import javafx.scene.control.ButtonType;
import javafx.scene.control.CheckBoxTreeItem;
import javafx.scene.control.ChoiceDialog;
import javafx.scene.control.Dialog;
import javafx.scene.control.Tab;
import javafx.scene.control.TabPane;
import javafx.scene.control.TextField;
import javafx.scene.control.ToggleButton;
import javafx.scene.image.Image;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyCodeCombination;
import javafx.scene.input.KeyCombination;
import javafx.scene.layout.Pane;
import javafx.scene.layout.StackPane;
import javafx.stage.Stage;
import net.xeoh.plugins.base.annotations.PluginImplementation;

/**
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class MultiAgentConfigurationFx extends AbstractJFXDialog
{
    
    @RCDialog(category = RCDialog.Category.Tools, name = "MultiAgentConfigurationFx")
    @PluginImplementation
    public static class Plugin extends DialogPlugin<MultiAgentConfigurationFx> {}

    @FXML
    private ToggleButton btn_connect;
    
    @FXML
    private TextField field_ip;
    
    @FXML
    private TextField field_ip_end;
    
    @FXML
    private TextField field_port_start;
    
    @FXML
    private TextField field_port_end;
    
    @FXML
    private TabPane tabpane;
    
    private final AgentTabGlobal allTab = new AgentTabGlobal();
    
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
        // TODO
        field_ip.disableProperty().bind(btn_connect.selectedProperty());
        field_ip_end.disableProperty().bind(btn_connect.selectedProperty());
        field_port_start.disableProperty().bind(btn_connect.selectedProperty());
        field_port_end.disableProperty().bind(btn_connect.selectedProperty());
        btn_connect.selectedProperty().addListener((ov, t, t1) -> {
            if(t1) {
                connecting();
                allTab.setDisable(false);
            } else {
                disconnecting();
                allTab.setDisable(true);
            }
        });
        
        allTab.setDisable(true);
        tabpane.getTabs().add(allTab);
        tabpane.getTabs().add(new Tab("+"));
        tabpane.setTabClosingPolicy(TabPane.TabClosingPolicy.UNAVAILABLE);
        tabpane.getSelectionModel().select(allTab);
        
        tabpane.getSelectionModel().selectedItemProperty().addListener((observable, oldValue, newValue) -> {
            if(newValue.getText().equals("+")) {
                System.out.println("new tab");
                tabpane.getSelectionModel().select(oldValue);
//                tabpane.getTabs().add(new Tab("+"));
                ChoiceDialog<String> d = new ChoiceDialog();
                d.getItems().add("a");
                d.getItems().add("b");
                d.getItems().add("c");
                
                Optional<String> result = d.showAndWait();
                
                if (result.isPresent()) {
                    Tab n = new Tab(result.get());
                    tabpane.getTabs().add(tabpane.getTabs().size()-1, n);
                    tabpane.getSelectionModel().select(n);
//                     formatSystem();
                }
                System.out.println(result);
                
            }
        });
        
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
    
    private void connecting() {
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
        
        List<String> ip_parts = Arrays.asList(field_ip.getText().trim().split("\\.", -1));
        String ip_range = field_ip_end.getText().trim().equals("0") ? ip_parts.get(3) : field_ip_end.getText().trim();
        int port_start = Integer.parseInt(field_port_start.getText().trim());
        int port_end = Integer.parseInt(field_port_end.getText().trim());

        if(ip_range.compareTo(ip_parts.get(3)) < 0 || port_end < port_start) {
            // TODO: indicate invalid values!?
            btn_connect.setSelected(false);
            return;
        }

        for (int i = Integer.parseInt(ip_parts.get(3)); i <= Integer.parseInt(ip_range); i++) {
            String ip = ip_parts.get(0) + "." + ip_parts.get(1) + "." + ip_parts.get(2) + "." + i;
            for (int port = port_start; port <= port_end; port++) {
                System.out.println(ip + ":" + port);
//                System.out.println(tabpane.getTabs());
                AgentTab tab = new AgentTab(ip, port);
                tab.connectDivider(allTab);
                tab.connectAgentList(allTab);
                tab.connectButtons(allTab);
                tabpane.getTabs().add(tab);
            }
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
    }
}
