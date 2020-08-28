package de.naoth.rc.dialogsFx.moduleconfiguration;

import javafx.fxml.FXML;
import javafx.scene.control.TreeView;

/**
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class ModuleConfigurationModules
{
    /** The current module configuration */
    private ModuleConfiguration mConfig;
    
    @FXML private TreeView<String> moduleTree;
    
    /**
     * Default constructor for the FXML loader.
     */
    public ModuleConfigurationModules() {}

    /**
     * Constructor for custom initialization.
     * 
     * @param config the current module configuration
     */
    public ModuleConfigurationModules(ModuleConfiguration config) {
        setModuleConfiguration(config);
    }
    
    /**
     * Sets the current module configuration and updates the internal binding.
     * 
     * @param config the new module configuration
     */
    public void setModuleConfiguration(ModuleConfiguration config) {
        this.mConfig = config;
        
        // set binding to the active module/representation
        //mConfig.getActiveProperty().addListener((ob) -> { updateDependencyPanel(); });
    }

    /**
     * Gets called, after the FXML file was loaded.
     */
    @FXML
    private void initialize() 
    {
        /*
        // TODO: need the TreeNode implementation from the DebugRequest branch
        moduleTree.setCellFactory((p) -> new TreeNodeCell<>());
        moduleTree.setRoot(new TreeNodeItem<>());
        moduleTree.getRoot().setExpanded(true);
        */
        
    }
}
