package de.naoth.rc.dialogsFx.multiagentconfiguration;

import de.naoth.rc.RobotControl;
import de.naoth.rc.core.dialog.AbstractJFXDialog;
import de.naoth.rc.core.dialog.DialogPlugin;
import de.naoth.rc.core.dialog.RCDialog;
import de.naoth.rc.dialogsFx.multiagentconfiguration.ui.AgentSelectionDialog;
import de.naoth.rc.dialogsFx.multiagentconfiguration.ui.AgentTab;
import de.naoth.rc.dialogsFx.multiagentconfiguration.ui.AllTab;
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
import java.util.stream.Collectors;
import javafx.application.Application;
import static javafx.application.Application.launch;
import javafx.application.Platform;
import javafx.beans.binding.Bindings;
import javafx.beans.binding.BooleanBinding;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.MenuItem;
import javafx.scene.control.Tab;
import javafx.scene.control.TabPane;
import javafx.scene.image.Image;
import javafx.scene.input.KeyCombination;
import javafx.stage.Stage;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 * This dialog can handle multiple connections and can be used to configure multiple
 * robots or show their representations at once.
 *
 * NOTE: this is only for intermediate usage and should be removed, if the other
 *       dialogs can handle multiple connections!
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class MultiAgentConfigurationFx extends AbstractJFXDialog
{
    
    @RCDialog(category = RCDialog.Category.Tools, name = "MultiAgentConfiguration (FX)")
    @PluginImplementation
    public static class Plugin extends DialogPlugin<MultiAgentConfigurationFx> {
        @InjectPlugin
        public static RobotControl parent;
    }

    @FXML private TabPane tabpane;
    @FXML private AllTab allTabViewController;
    
    @FXML private MenuItem miDisconnect;
    @FXML private MenuItem miReconnect;
    @FXML private MenuItem miClose;
    
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

    /**
     * Returns the global theme.
     * @return path to the global theme stylesheet
     */
    @Override
    protected String getTheme() {
        return Plugin.parent.getTheme();
    }
    
    @Override
    public void afterInit() {
        if(Plugin.parent != null) {
            setConfig(Plugin.parent.getConfig());
        }
        
        tabpane.setTabClosingPolicy(TabPane.TabClosingPolicy.UNAVAILABLE);
        // add 'all' Tab
        allTabViewController = new AllTab();
        tabpane.getTabs().add(allTabViewController);
        // if there's no robot connected, disable "all" tab and select it
        tabpane.getTabs().get(0).disableProperty().bind(Bindings.size(tabpane.getTabs()).lessThanOrEqualTo(1));
        tabpane.getTabs().get(0).disableProperty().addListener((obs, o, n) -> { if(n) { tabpane.getSelectionModel().select(0); } });
        
        BooleanBinding b = Bindings.size(tabpane.getTabs()).lessThanOrEqualTo(1);
        miDisconnect.disableProperty().bind(b);
        miReconnect.disableProperty().bind(b);
        miClose.disableProperty().bind(b);
        // apply theme to connection dialog
        dialog.getDialogPane().getStylesheets().add(getTheme());
    }

    public void setConfig(Properties c) {
        dialog.setHosts(Arrays.asList(c.getProperty("iplist","").split(",")));
    }

    @Override
    public Map<KeyCombination, Runnable> getGlobalShortcuts() {
        HashMap<KeyCombination, Runnable> shortcuts = new HashMap<>();
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
        // generate a list of existing agent items
        List<AgentItem> existing = tabpane.getTabs().stream().map((t) -> {
            return (t instanceof AgentTab) ? new AgentItem(((AgentTab)t).getHost(), ((AgentTab)t).getPort()) : null;
        }).filter((i) -> { return i != null;}).collect(Collectors.toList());
        dialog.setExisting(existing);
        
        Optional<List<AgentItem>> result = dialog.showAndWait();
        result.ifPresent((al) -> {
            if(!al.isEmpty()) {
                connecting(al);
            }
        });
    }
    
    public void connecting(List<AgentItem> l) {
        l.sort((a1, a2) -> { return a1.toString().compareTo(a2.toString()); });
        for (AgentItem agentItem : l) {
            // check if we got already a tab for this agent
            Optional<Tab> etab = tabpane.getTabs().stream().filter((t) -> { return t.getText().equals(agentItem.toString()); }).findFirst();
            if(etab.isPresent()) {
                // reconnect if necessary
                ((AgentTab)etab.get()).connect();
            } else {
                // add new tab
                AgentTab tab = new AgentTab(agentItem.getHost(), agentItem.getPort(), allTabViewController);
                tabpane.getTabs().add(tab);
            }
        }
    }
    
    @FXML
    public void disconnectAll() {
        // disconnect all, except the first ("all") tabs
        tabpane.getTabs().stream().forEach((t) -> {
            if(t instanceof Tab && tabpane.getTabs().indexOf(t) != 0) {
                ((AgentTab)t).disconnect();
            }
        });
    }
    
    @FXML
    public void reconnectAll() {
        // reconnect all, except the first ("all") tabs
        tabpane.getTabs().stream().forEach((t) -> {
            if(t instanceof Tab && tabpane.getTabs().indexOf(t) != 0) {
                ((AgentTab)t).connect();
            }
        });
    }
    
    @FXML
    public void closeAll() {
        // close all, except the first ("all") tabs
        Platform.runLater(() -> {
            while(tabpane.getTabs().size() > 1) {
                Tab t = tabpane.getTabs().get(tabpane.getTabs().size()-1);
                if(t instanceof Tab && tabpane.getTabs().indexOf(t) != 0) {
                    ((AgentTab)t).requestClose();
                }
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
}
