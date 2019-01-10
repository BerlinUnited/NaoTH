package de.naoth.rc.dialogs.multiagentconfiguration;

import de.naoth.rc.RobotControl;
import de.naoth.rc.core.dialog.AbstractJFXDialog;
import de.naoth.rc.core.dialog.DialogPlugin;
import de.naoth.rc.core.dialog.RCDialog;
import de.naoth.rc.dialogs.multiagentconfiguration.ui.AgentSelectionDialog;
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
import java.util.Properties;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.application.Application;
import static javafx.application.Application.launch;
import javafx.application.Platform;
import javafx.beans.binding.Bindings;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.Tab;
import javafx.scene.control.TabPane;
import javafx.scene.control.cell.CheckBoxListCell;
import javafx.scene.image.Image;
import javafx.scene.input.KeyCombination;
import javafx.stage.Stage;
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
    private TabPane tabpane;
    
    private final AgentTabGlobal allTab = new AgentTabGlobal();
    
    private AgentSelectionDialog dialog;
    
    public MultiAgentConfigurationFx() {
        super();
        
        if(Platform.isFxApplicationThread()) {
            dialog = new AgentSelectionDialog();
        } else {
            Platform.runLater(() -> { dialog = new AgentSelectionDialog(); });
        }
    }

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
        
        tabpane.getTabs().add(allTab);
        tabpane.setTabClosingPolicy(TabPane.TabClosingPolicy.UNAVAILABLE);
        tabpane.getSelectionModel().select(allTab);
        // if there's no robot connected, disable "all" tab and select it
        allTab.disableProperty().bind(Bindings.size(tabpane.getTabs()).lessThanOrEqualTo(1));
        allTab.disableProperty().addListener((obs, o, n) -> { if(n) { tabpane.getSelectionModel().select(allTab); } });
    }

    public void setConfig(Properties c) {
        dialog.setHosts(Arrays.asList(c.getProperty("iplist","").split(",")));
    }

    @Override
    public Map<KeyCombination, Runnable> getGlobalShortcuts() {
        HashMap<KeyCombination, Runnable> shortcuts = new HashMap<KeyCombination, Runnable>();
        // TODO: add shortcuts!!
        /*
        shortcuts.put(new KeyCodeCombination(KeyCode.C, KeyCombination.CONTROL_DOWN, KeyCombination.SHIFT_DOWN), () -> {
            connect();
        });
        shortcuts.put(new KeyCodeCombination(KeyCode.D, KeyCombination.ALT_DOWN, KeyCombination.SHIFT_DOWN), () -> {
            disconnect();
        });
        */
        return shortcuts;
    }
    
    @FXML
    public void addRobots() {
        Optional<List<AgentItem>> result = dialog.showAndWait();
        result.ifPresent((al) -> {
            if(!al.isEmpty()) {
                connecting(al);
            }
        });
    }
    
    public void connecting(List<AgentItem> l) {
        for (AgentItem agentItem : l) {
            // check if already connected!
            if(tabpane.getTabs().stream().anyMatch((t) -> { return t.getText().equals(agentItem.toString()); })) {
                continue;
            }
            // add new tab
            AgentTab tab = new AgentTab(agentItem.getHost(), agentItem.getPort());
            tab.connectDivider(allTab);
            tab.connectAgentList(allTab);
            tab.connectButtons(allTab);
            tabpane.getTabs().add(tab);
        }
    }
    
    @FXML
    public void disconnectAll() {
        // disconnect all, except the first ("all") tabs
        tabpane.getTabs().stream().forEach((t) -> {
            if(t instanceof Tab) {
                ((AgentTab)t).disconnect();
            }
        });
    }
    
    private void disconnecting() {
        // remove all, except the first ("all") tabs
        while (tabpane.getTabs().size() > 1) {
            Tab t = tabpane.getTabs().get(tabpane.getTabs().size()-1);
            if(t instanceof Tab) {
                System.out.println(t);
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
}
