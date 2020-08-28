package de.naoth.rc.dialogsFx.representationinspector;

import de.naoth.rc.RobotControl;
import de.naoth.rc.core.dialog.AbstractJFXDialog;
import de.naoth.rc.core.dialog.DialogPlugin;
import de.naoth.rc.core.dialog.RCDialog;
import de.naoth.rc.logmanager.LogFileEventManager;
import de.naoth.rc.manager.GenericManagerFactory;
import java.net.URL;
import javafx.fxml.FXML;
import javafx.scene.control.ChoiceBox;
import javafx.scene.control.ToggleButton;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyCodeCombination;
import javafx.scene.input.KeyCombination;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class RepresentationInspectorDialog  extends AbstractJFXDialog
{
    @RCDialog(category = RCDialog.Category.Status, name = "Representations (FX)")
    @PluginImplementation
    public static class Plugin extends DialogPlugin<RepresentationInspectorDialog> {
        @InjectPlugin
        public static RobotControl parent;
        @InjectPlugin
        public static GenericManagerFactory genericManagerFactory;
        @InjectPlugin
        public static LogFileEventManager logFileEventManager;
        
        @Override
        public String getDisplayName() { return "Representations (FX)"; }
    }

    /** The controller of the representation panel */
    @FXML RepresentationPanel representationPanelController;
    
    /** Some UI elements */
    @FXML ToggleButton btnRefresh;
    @FXML ToggleButton btnBinary;
    @FXML ToggleButton btnLog;
    @FXML ChoiceBox<RepresentationPanel.Type> type;
    
    /** some dialog shortcuts for the corresponding buttons */
    private final KeyCombination shortcutRefresh = new KeyCodeCombination(KeyCode.R, KeyCombination.ALT_DOWN, KeyCombination.SHIFT_DOWN);
    private final KeyCombination shortcutBinary = new KeyCodeCombination(KeyCode.B, KeyCombination.ALT_DOWN, KeyCombination.SHIFT_DOWN);
    private final KeyCombination shortcutLog = new KeyCodeCombination(KeyCode.L, KeyCombination.ALT_DOWN, KeyCombination.SHIFT_DOWN);
    
    /**
     * Returns the ui definition.
     * @return the url to the fxml file
     */
    @Override
    public URL getFXMLRessource() {
        return getClass().getResource("RepresentationInspectorDialog.fxml");
    }

    /**
     * Marks this class as controller for the corresponding FXML file.
     * 
     * @return true
     */
    @Override
    protected boolean isSelfController() {
        return true;
    }
    
    /**
     * Gets called after ui initialization and sets the message server of the ui controller.
     */
    @Override
    public void afterInit() {
        // set the global plugin references to the view controller
        representationPanelController.setLogFileEventManager(Plugin.logFileEventManager);
        representationPanelController.setGenericManagerFactory(Plugin.genericManagerFactory);
        representationPanelController.setRobotControl(Plugin.parent);
        
        // set the dropdownbox to the available representation types
        type.getItems().addAll(RepresentationPanel.Type.values());
        type.setValue(RepresentationPanel.Type.Cognition);
        
        // bind some "callbacks" to the (internal) representation panel state
        representationPanelController.getTypeProperty().bind(type.valueProperty());
        representationPanelController.getSourceProperty().addListener((o) -> {
            // make sure, all buttons reflect the current (internal) source state
            if (representationPanelController.getSourceProperty().get() == RepresentationPanel.Source.None) {
                btnRefresh.setSelected(false);
                btnBinary.setSelected(false);
                btnLog.setSelected(false);
            }
        });
        
        // install some ui (button) dependencies (enable/disable on select)
        type.disableProperty().bind(btnLog.selectedProperty());
        
        // set the dialog shortcuts
        getScene().getAccelerators().put(shortcutRefresh, () -> { btnRefresh.fire(); });
        getScene().getAccelerators().put(shortcutBinary, () -> { btnBinary.fire(); });
        getScene().getAccelerators().put(shortcutLog, () -> { btnLog.fire(); });
    }

    /**
     * Is called, when the refresh button is clicked.
     */
    @FXML
    private void fxSourcePrint() {
        if (btnRefresh.isSelected()) {
            representationPanelController.getSourceProperty().set(RepresentationPanel.Source.Connection);
        } else {
            representationPanelController.getSourceProperty().set(RepresentationPanel.Source.None);
        }
    }
    
    /**
     * Is called, when the binary button is clicked.
     */
    @FXML
    private void fxSourceBinary() {
        if (btnBinary.isSelected()) {
            representationPanelController.getSourceProperty().set(RepresentationPanel.Source.Binary);
        } else {
            representationPanelController.getSourceProperty().set(RepresentationPanel.Source.None);
        }
    }
    
    /**
     * Is called, when the log button is clicked.
     */
    @FXML
    private void fxSourceLog() {
        if (btnLog.isSelected()) {
            representationPanelController.getSourceProperty().set(RepresentationPanel.Source.Log);
        } else {
            representationPanelController.getSourceProperty().set(RepresentationPanel.Source.None);
        }
    }
}
