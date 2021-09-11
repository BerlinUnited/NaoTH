package de.naoth.rc.dialogsFx.debugrequests;

import de.naoth.rc.RobotControl;
import de.naoth.rc.core.dialog.AbstractJFXDialog;
import de.naoth.rc.core.dialog.DialogPlugin;
import de.naoth.rc.core.dialog.RCDialog;
import java.net.URL;
import javafx.fxml.FXML;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyCodeCombination;
import javafx.scene.input.KeyCombination;
import javafx.scene.input.KeyEvent;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class DebugRequestsDialog extends AbstractJFXDialog
{
    @RCDialog(category = RCDialog.Category.Debug, name = "DebugRequest (FX)")
    @PluginImplementation
    public static class Plugin extends DialogPlugin<DebugRequestsDialog> {
        @InjectPlugin
        public static RobotControl parent;

        @Override
        public String getDisplayName() { return "DebugRequest (FX)"; }
    }
    
    /** Controller of the DebugRequestsTree - set by FXML */
    @FXML private DebugRequestsTree debugRequestsTreeController;
    
    /** Dialog shortcut for the corresponding button */
    private final KeyCombination shortcutUpdate = new KeyCodeCombination(KeyCode.U, KeyCombination.ALT_DOWN, KeyCombination.SHIFT_DOWN);

    /**
     * Returns the ui definition.
     * @return the url to the fxml file
     */
    @Override
    public URL getFXMLRessource() {
        return getClass().getResource("DebugRequestsDialog.fxml");
    }

    /**
     * This the controller of the FXML file.
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
        debugRequestsTreeController.setRobotControl(Plugin.parent);
        
        // set the dialog shortcut
        getScene().getAccelerators().put(shortcutUpdate, () -> { fxUpdateRequests(); });
    }
    
    /**
     * Handles the update button click.
     */
    @FXML
    private void fxUpdateRequests() {
        debugRequestsTreeController.updateRequests();
    }
}
