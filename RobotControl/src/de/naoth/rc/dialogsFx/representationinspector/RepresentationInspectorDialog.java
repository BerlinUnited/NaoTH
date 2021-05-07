package de.naoth.rc.dialogsFx.representationinspector;

import de.naoth.rc.RobotControl;
import de.naoth.rc.core.dialog.AbstractJFXDialog;
import de.naoth.rc.core.dialog.DialogPlugin;
import de.naoth.rc.core.dialog.RCDialog;
import de.naoth.rc.logmanager.LogFileEventManager;
import de.naoth.rc.manager.GenericManagerFactory;
import java.net.URL;
import javafx.fxml.FXML;
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
        // set the global plugin references to the view controller
        representationPanelController.setLogFileEventManager(Plugin.logFileEventManager);
        representationPanelController.setGenericManagerFactory(Plugin.genericManagerFactory);
        representationPanelController.setRobotControl(Plugin.parent);
    }

    /**
     * Is called, when the dialog gets closed.
     */
    @Override
    public void dispose() {
        // setting the source to "None" disconnects the panel
        representationPanelController.getSourceProperty().set(RepresentationPanel.Source.None);
    }
}
