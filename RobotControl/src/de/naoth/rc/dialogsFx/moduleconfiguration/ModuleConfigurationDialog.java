package de.naoth.rc.dialogsFx.moduleconfiguration;

import de.naoth.rc.RobotControl;
import de.naoth.rc.componentsFx.AutoCompleteComboBox;
import de.naoth.rc.core.dialog.AbstractJFXDialog;
import de.naoth.rc.core.dialog.DialogPlugin;
import de.naoth.rc.core.dialog.RCDialog;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.net.URL;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.fxml.FXML;
import javafx.scene.control.Button;
import javafx.scene.control.ComboBox;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyCodeCombination;
import javafx.scene.input.KeyCombination;
import javafx.stage.FileChooser;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class ModuleConfigurationDialog extends AbstractJFXDialog
{
    @RCDialog(category = RCDialog.Category.Configure, name = "Modules (FX)")
    @PluginImplementation
    public static class Plugin extends DialogPlugin<ModuleConfigurationDialog> {
        @InjectPlugin
        public static RobotControl parent;
        
        @Override
        public String getDisplayName() { return "ModuleConfiguration (FX)"; }
    }
    
    /** The current module configuration */
    private final ModuleConfiguration mConfig = new ModuleConfiguration(Plugin.parent);
    
    /** The controllers of the sub-panels */
    @FXML private ModuleConfigurationDependency dependenciesController;
    @FXML private ModuleConfigurationModules modulesPanelController;
    @FXML private ModuleConfigurationRepresentations representationsPanelController;
    
    /** The ui tree of the available debug requests */
    @FXML private Button btnSave;
    @FXML private Button btnExport;
    @FXML private ComboBox<Module> module;
    @FXML private ComboBox<Representation> representation;
    
    /** Shortcuts definitions */
    private final KeyCombination shortcutUpdate = new KeyCodeCombination(KeyCode.U, KeyCombination.ALT_DOWN, KeyCombination.SHIFT_DOWN);
    private final KeyCombination shortcutSave = new KeyCodeCombination(KeyCode.S, KeyCombination.ALT_DOWN, KeyCombination.SHIFT_DOWN);
    private final KeyCombination shortcutExport = new KeyCodeCombination(KeyCode.E, KeyCombination.ALT_DOWN, KeyCombination.SHIFT_DOWN);

    static final private String CONFIG_PATH_PARAMS = "parameter_save_path";
    static final private String CONFIG_PATH_DEFAULT = "../NaoTHSoccer/Config/scheme";
    
    /**
     * Returns the ui definition.
     * @return the url to the fxml file
     */
    @Override
    public URL getFXMLRessource() {
        return getClass().getResource("ModuleConfigurationDialog.fxml");
    }

    /**
     * This class is the controller for the FXML file.
     * @return true
     */
    @Override
    protected boolean isSelfController() {
        return true;
    }

    /**
     * Returns the global theme.
     * @return path to the global theme stylesheet
     */
    @Override
    protected String getTheme() {
        return Plugin.parent.getTheme();
    }
    
    /**
     * Gets called after ui initialization and sets the message server of the ui controller.
     */
    @Override
    public void afterInit() {
        // set the global module configuration to the separate view controllers
        dependenciesController.setModuleConfiguration(mConfig);
        modulesPanelController.setModuleConfiguration(mConfig);
        representationsPanelController.setModuleConfiguration(mConfig);
        
        // setup ui
        btnSave.disableProperty().bind(mConfig.getModulesProperty().emptyProperty());
        btnExport.disableProperty().bind(mConfig.getModulesProperty().emptyProperty());
        
        module.setItems(mConfig.getModulesProperty().sorted());
        module.disableProperty().bind(mConfig.getModulesProperty().emptyProperty());
        module.setConverter(mConfig.getModuleConverter());
        AutoCompleteComboBox.install(module);
        
        representation.setItems(mConfig.getRepresentationsProperty().sorted());
        representation.disableProperty().bind(mConfig.getRepresentationsProperty().emptyProperty());
        representation.setConverter(mConfig.getRepresentationConverter());
        AutoCompleteComboBox.install(representation);

        // update the selection property based on the combobox selection
        module.getSelectionModel().selectedItemProperty().addListener((ob,o,n) -> {
            // NOTE: make sure the clear selection (below) doesn't trigger an active property change!
            if (n != null) { mConfig.getActiveProperty().set(n); }
        });
        representation.getSelectionModel().selectedItemProperty().addListener((ob,o,n) -> {
            // NOTE: make sure the clear selection (below) doesn't trigger an active property change!
            if (n != null) { mConfig.getActiveProperty().set(n); }
        });
        
        // set binding to the active module/representation
        mConfig.getActiveProperty().addListener((ob) -> {
            // clear the module selection, if another module/representation is selected
            if (module.getValue() != mConfig.getActiveProperty().get()) {
                module.getSelectionModel().clearSelection();
            }
            // clear the representation selection, if another module/representation is selected
            if (representation.getValue() != mConfig.getActiveProperty().get()) {
                representation.getSelectionModel().clearSelection();
            }
        });
        
        // set the dialog shortcuts
        getScene().getAccelerators().put(shortcutUpdate, () -> { fxUpdateModules(); });
        getScene().getAccelerators().put(shortcutSave, () -> { btnSave.fire(); });
        getScene().getAccelerators().put(shortcutExport, () -> { btnExport.fire(); });
    }

    /**
     * Handles the update button click and sends the retrieving commands.
     */
    @FXML
    private void fxUpdateModules() {
        mConfig.updateModules();
    }

    /**
     * Handles the save button click and sends the saving commands.
     */
    @FXML
    private void fxSaveModules() {
        mConfig.saveModules();
    }

    /**
     * Handles the export button click and exports the module configuration to a file.
     */
    @FXML
    public void fxExportModules() {
        String parameterPath = Plugin.parent.getConfig().getProperty(CONFIG_PATH_PARAMS, CONFIG_PATH_DEFAULT);
        // configure the filechooser ...
        FileChooser fileChooser = new FileChooser();
        fileChooser.setTitle("Save module configuration");
        fileChooser.getExtensionFilters().addAll(new FileChooser.ExtensionFilter("Config files (*.cfg)", "*.cfg"));
        fileChooser.setInitialDirectory(new File(parameterPath));
        fileChooser.setInitialFileName("modules.cfg");

        File selectedFile = fileChooser.showSaveDialog(btnExport.getScene().getWindow());
        if(selectedFile != null) {
            // if selected file has a file extension - use this, otherwise append ".cfg" to the filename
            File f = (selectedFile.getName().lastIndexOf(".") == -1) ? new File(selectedFile+".cfg") : selectedFile;

            // create file and write module configuration to this file
            try(BufferedWriter bf = new BufferedWriter(new FileWriter(f))) {
                bf.write("[modules]\n");
                for (Module m : mConfig.getModulesProperty()) {
                    bf.write(m.getName() + "=" + (m.isActive() ? "true" : "false") + "\n");
                }
            } catch (IOException ex) {
                Logger.getLogger(getClass().getName()).log(Level.SEVERE, null, ex);
                // TODO: show alert dialog
            }

            // save the path for later
            Plugin.parent.getConfig().setProperty(CONFIG_PATH_PARAMS, selectedFile.getParent());
        }
    }
}
