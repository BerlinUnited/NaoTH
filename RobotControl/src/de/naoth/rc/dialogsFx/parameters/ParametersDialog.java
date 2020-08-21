package de.naoth.rc.dialogsFx.parameters;

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

    /**
     * Returns the ui definition.
     * @return the url to the fxml file
     */
    @Override
    public URL getFXMLRessource() {
        return getClass().getResource("ParametersView.fxml");
    }

    /**
     * Gets called after ui initialization and sets the message server of the ui controller.
     */
    @Override
    public void afterInit() {
        // set the global robot control instance to the view controller
        ((ParametersController)getController()).setRobotControl(Plugin.parent);
    }    
}
