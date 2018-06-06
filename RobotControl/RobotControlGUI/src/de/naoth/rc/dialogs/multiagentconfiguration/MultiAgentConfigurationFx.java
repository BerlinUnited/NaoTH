package de.naoth.rc.dialogs.multiagentconfiguration;

import de.naoth.rc.core.dialog.AbstractJFXDialog;
import de.naoth.rc.core.dialog.DialogPlugin;
import de.naoth.rc.core.dialog.RCDialog;
import de.naoth.rc.dialogs.multiagentconfiguration.ui.AgentTab;
import java.io.IOException;
import java.net.URL;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.application.Application;
import static javafx.application.Application.launch;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.CheckBoxTreeItem;
import javafx.scene.control.Tab;
import javafx.scene.control.TabPane;
import javafx.scene.control.TextField;
import javafx.scene.control.ToggleButton;
import javafx.scene.image.Image;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyCodeCombination;
import javafx.scene.input.KeyCombination;
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
    
    private final AgentTabAll allTab = new AgentTabAll();
    
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
    
    class AgentTabAll extends AgentTab
    {
        public AgentTabAll() {
            super();
            
            setText("All");
            
            CheckBoxTreeItem<String> motion_root = new CheckBoxTreeItem<>("Motion");
            motion_root.setExpanded(true);
            debugTree.getRoot().getChildren().add(motion_root);
            Utils.global_debug_requests.put(motion_root.getValue(), motion_root);
            
            CheckBoxTreeItem<String> cognition_root = new CheckBoxTreeItem<>("Cognition");
            cognition_root.setExpanded(true);
            debugTree.getRoot().getChildren().add(cognition_root);
            Utils.global_debug_requests.put(cognition_root.getValue(), cognition_root);
            
            CheckBoxTreeItem<String> motion_modules_root = new CheckBoxTreeItem<>("Motion");
            motion_modules_root.setExpanded(true);
            System.out.println(moduleTree);
            moduleTree.getRoot().getChildren().add(motion_modules_root);
            Utils.global_modules.put(motion_modules_root.getValue(), motion_modules_root);

            CheckBoxTreeItem<String> cognition_modules_root = new CheckBoxTreeItem<>("Cognition");
            cognition_modules_root.setExpanded(true);
            moduleTree.getRoot().getChildren().add(cognition_modules_root);
            Utils.global_modules.put(cognition_modules_root.getValue(), cognition_modules_root);
            
            agentList.setItems(Utils.global_agent_list);
        }
    }
    
    public static void main(String[] args) {
        launch(MultiAgentConfigurationFxMain.class, args);
    }
    
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
            } catch (IOException ex) {
                Logger.getLogger(MultiAgentConfigurationFxMain.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
    }
}
