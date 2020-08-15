package de.naoth.rc.dialogsFx.representationinspector;

import de.naoth.rc.RobotControl;
import de.naoth.rc.core.dialog.AbstractJFXDialog;
import de.naoth.rc.core.dialog.DialogPlugin;
import de.naoth.rc.core.dialog.RCDialog;
import de.naoth.rc.logmanager.LogFileEventManager;
import de.naoth.rc.manager.GenericManagerFactory;
import java.net.URL;
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
    
    /**
     * Returns the ui definition.
     * @return the url to the fxml file
     */
    @Override
    public URL getFXMLRessource() {
        return getClass().getResource("RepresentationInspectorView.fxml");
    }

    /**
     * Gets called after ui initialization and sets the message server of the ui controller.
     */
    @Override
    public void afterInit() {
        RepresentationInspectorController controller = (RepresentationInspectorController)getController();
        // set the global plugin references to the view controller
        controller.setMessageServer(Plugin.parent.getMessageServer());
        controller.setLogFileEventManager(Plugin.logFileEventManager);
        controller.setGenericManagerFactory(Plugin.genericManagerFactory);
    }
}
