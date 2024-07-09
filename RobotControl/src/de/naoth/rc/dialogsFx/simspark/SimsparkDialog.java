package de.naoth.rc.dialogsFx.simspark;

import de.naoth.rc.RobotControl;
import de.naoth.rc.components.simspark.SimsparkManager;
import de.naoth.rc.core.dialog.AbstractJFXDialog;
import de.naoth.rc.core.dialog.DialogPlugin;
import de.naoth.rc.core.dialog.RCDialog;
import de.naoth.rc.drawingmanager.DrawingEventManager;
import java.net.URL;
import javafx.beans.property.BooleanProperty;
import javafx.fxml.FXML;
import javafx.scene.control.CheckBox;
import javafx.scene.control.Spinner;
import javafx.scene.control.TextField;
import javafx.scene.control.ToggleButton;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class SimsparkDialog extends AbstractJFXDialog
{
    @RCDialog(category = RCDialog.Category.Tools, name = "Simspark (FX)")
    @PluginImplementation
    public static class Plugin extends DialogPlugin<SimsparkDialog> {
        @InjectPlugin
        public static RobotControl parent;
        @InjectPlugin
        public static SimsparkManager simsparkManager;
        @InjectPlugin
        public static DrawingEventManager drawingEventManager;
        @Override
        public String getDisplayName() { return "Simspark (FX)"; }
    }

    /** The controller of the representation panel */
    @FXML
    SimsparkPanel simsparkPanelController;
    @FXML ToggleButton connectBtn;
    @FXML TextField host;
    @FXML
    Spinner<Integer> port;
    @FXML
    CheckBox fieldDrawings;

    /**
     * Returns the ui definition.
     * @return the url to the fxml file
     */
    @Override
    public URL getFXMLRessource()
    {
        return getClass().getResource("SimsparkDialog.fxml");
    }

    /**
     * This is the controller for FXML file.
     * 
     * @return true
     */
    @Override
    protected boolean isSelfController()
    {
        return true;
    }

    /**
     * Returns the global theme.
     * @return path to the global theme stylesheet
     */
    @Override
    protected String getTheme()
    {
        return SimsparkDialog.Plugin.parent.getTheme();
    }
    
    /**
     * Gets called after ui initialization and sets the message server of the ui controller.
     */
    @Override
    public void afterInit()
    {
        simsparkPanelController.setSimsparkManager(Plugin.simsparkManager);
        simsparkPanelController.setDrawingEventManager(Plugin.drawingEventManager);
        // handle some ui changes
        host.disableProperty().bind(Plugin.simsparkManager.isConnected());
        port.disableProperty().bind(Plugin.simsparkManager.isConnected());
        Plugin.simsparkManager.isConnected().addListener((b) -> {
            connectBtn.setSelected(((BooleanProperty)b).get());
        });
        fieldDrawings.selectedProperty().addListener((v) ->
        {
            if (((BooleanProperty) v).get())
            {
                simsparkPanelController.enableFieldDrawings();
            }
            else
            {
                simsparkPanelController.disableFieldDrawings();
            }
        });
    }
    
    /**
     * Is called, when the connect button is clicked.
     */
    @FXML
    private void fxHandleConnectBtn()
    {
        if (Plugin.simsparkManager.isConnected().get())
        {
            Plugin.simsparkManager.disconnect();
        }
        else
        {
            // TODO: check host/port values
            String h = host.getText().trim();
            int p = port.getValue();
            Plugin.simsparkManager.connect(h, p);
        }
    }
}
