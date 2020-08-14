package de.naoth.rc.dialogsFx.debugrequests;

import de.naoth.rc.RobotControl;
import de.naoth.rc.core.dialog.AbstractJFXDialog;
import de.naoth.rc.core.dialog.DialogPlugin;
import de.naoth.rc.core.dialog.RCDialog;
import java.net.URL;
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

    /**
     * Returns the ui definition.
     * @return the url to the fxml file
     */
    @Override
    public URL getFXMLRessource() {
        return getClass().getResource("DebugRequestsView.fxml");
    }

    /**
     * Gets called after ui initialization and sets the message server of the ui controller.
     */
    @Override
    public void afterInit() {
        // set the global message server to the view controller
        ((DebugRequestsController)getController()).setMessageServer(Plugin.parent.getMessageServer());
    }
}
