package de.naoth.rc.dialogsFx.parameters;

import de.naoth.rc.RobotControl;
import de.naoth.rc.core.dialog.AbstractJFXDialog;
import de.naoth.rc.core.dialog.DialogPlugin;
import de.naoth.rc.core.dialog.RCDialog;
import java.net.URL;
import javafx.fxml.FXML;
import javafx.scene.control.Label;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyCodeCombination;
import javafx.scene.input.KeyCombination;
import javafx.scene.input.KeyEvent;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class ParametersDialog extends AbstractJFXDialog
{
    @RCDialog(category = RCDialog.Category.Configure, name = "Parameter (FX)")
    @PluginImplementation
    public static class Plugin extends DialogPlugin<ParametersDialog> {
        @InjectPlugin
        public static RobotControl parent;

        @Override
        public String getDisplayName() { return "Parameter (FX)"; }
    }
    
    @FXML private ParametersPanel parametersPanelController;
    @FXML private Label scheme;

    /** Some key shortcut definitions */
    private final KeyCombination shortcutUpdate = new KeyCodeCombination(KeyCode.U, KeyCombination.ALT_DOWN, KeyCombination.SHIFT_DOWN);
    private final KeyCombination shortcutExport = new KeyCodeCombination(KeyCode.E, KeyCombination.ALT_DOWN, KeyCombination.SHIFT_DOWN);
    private final KeyCombination shortcutExportAll = new KeyCodeCombination(KeyCode.A, KeyCombination.ALT_DOWN, KeyCombination.SHIFT_DOWN);
    
    /**
     * Returns the ui definition.
     * @return the url to the fxml file
     */
    @Override
    public URL getFXMLRessource() {
        return getClass().getResource("ParametersDialog.fxml");
    }

    /**
     * This is the controller for FXML file.
     * 
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
        // set the global robot control instance to the view controller
        parametersPanelController.setRobotControl(Plugin.parent);
        scheme.textProperty().bind(parametersPanelController.getSchemeProperty());
        
        // set the dialog shortcuts
        getScene().getAccelerators().put(shortcutUpdate, () -> { fxUpdateParams(); });
        getScene().getAccelerators().put(shortcutExport, () -> { fxExport(); });
        getScene().getAccelerators().put(shortcutExportAll, () -> { fxExportAll(); });
    }
    
    /**
     * Is called, when the update button is clicked.
     */
    @FXML
    private void fxUpdateParams() {
        parametersPanelController.updateParams();
    }
    
    /**
     * Is called, when the export button is pressed.
     */
    @FXML
    private void fxExport() {
        parametersPanelController.exportParameter();
    }
    
    /**
     * Is called, when the export all menu item is pressed.
     */
    @FXML
    private void fxExportAll() {
        parametersPanelController.exportAllParameters();
    }
}
