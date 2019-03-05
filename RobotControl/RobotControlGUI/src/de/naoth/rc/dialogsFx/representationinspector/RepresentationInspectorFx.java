package de.naoth.rc.dialogsFx.representationinspector;

import de.naoth.rc.RobotControl;
import de.naoth.rc.core.dialog.AbstractJFXDialog;
import de.naoth.rc.core.dialog.DialogPlugin;
import de.naoth.rc.core.dialog.RCDialog;
import de.naoth.rc.core.manager.ObjectListener;
import de.naoth.rc.manager.GenericManager;
import de.naoth.rc.manager.GenericManagerFactory;
import de.naoth.rc.server.Command;
import de.naoth.rc.server.ResponseListener;
import java.net.URL;
import java.util.Arrays;
import java.util.List;
import javafx.application.Platform;
import javafx.beans.binding.Bindings;
import javafx.fxml.FXML;
import javafx.scene.control.ChoiceBox;
import javafx.scene.control.ListView;
import javafx.scene.control.TextArea;
import javafx.scene.control.ToggleButton;
import javafx.scene.control.ToggleGroup;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class RepresentationInspectorFx  extends AbstractJFXDialog implements ResponseListener
{
    @RCDialog(category = RCDialog.Category.Status, name = "Representations (FX)")
    @PluginImplementation
    public static class Plugin extends DialogPlugin<RepresentationInspectorFx> {
        @InjectPlugin
        public static RobotControl parent;
        @InjectPlugin
        public static GenericManagerFactory genericManagerFactory;
    }
    
    @FXML ToggleGroup listener;
    @FXML ToggleButton btnRefresh;
    @FXML ToggleButton btnBinary;
    @FXML ToggleButton btnLog;
    @FXML ChoiceBox<Command> type;
    @FXML ListView<String> list;
    @FXML TextArea content;

    private final Command cmd_list_cognition = new ListCommand("Cognition", "Cognition:representation:list");
    private final Command cmd_list_motion = new ListCommand("Motion", "Motion:representation:list");
    
    private GenericManager currentManager;
    private ObjectListener currentHandler;
    
    @Override
    protected boolean isSelfController() {
        return true;
    }

    @Override
    public URL getFXMLRessource() {
        return getClass().getResource("RepresentationInspectorFx.fxml");
    }
    
    @Override
    public void afterInit() {
        // setup ui
        type.getItems().add(cmd_list_cognition);
        type.getItems().add(cmd_list_motion);
        
        type.setValue(cmd_list_cognition);
        type.setOnAction((e) -> { typeChanged(); });
        
        list.getSelectionModel().selectedItemProperty().addListener((ob, o, n) -> {
            subscribeRepresentations(n);
        });
        
        listener.selectedToggleProperty().addListener((o, oldButton, newButton) -> {
            if(oldButton != null) {
                unsubscribeRepresentations();
            }
            
            if(newButton != null) {
                if(newButton.equals(btnRefresh)) {
                    if(Plugin.parent.checkConnected()) {
                        Plugin.parent.getMessageServer().executeCommand(this, type.getValue());
                    } else {
                        btnRefresh.setSelected(false);
                    }
                } else if(newButton.equals(btnBinary)) {
                    content.setText("TODO: Binary!");
                } else if(newButton.equals(btnLog)) {
                    content.setText("TODO: Log!");
                }
            }
        });
    }
    
    private void typeChanged() {
        unsubscribeRepresentations();
        if(Plugin.parent.getMessageServer().isConnected()) {
            Plugin.parent.getMessageServer().executeCommand(this, type.getValue());
        }
    }
    
    private void subscribeRepresentations(String n) {
        unsubscribeRepresentations();
        if(n != null) {
            currentHandler = new DataHandlerPrint();
            Command cmd = new Command(type.getValue().toString() + ":representation:print").addArg(n);

            currentManager = Plugin.genericManagerFactory.getManager(cmd);
            currentManager.addListener(currentHandler);
        }
    }
    
    private void unsubscribeRepresentations() {
        if (currentManager != null) {
            currentManager.removeListener(currentHandler);
        }
    }
    
    @Override
    public void handleResponse(byte[] result, Command command) {
        if(command.equals(cmd_list_cognition) || command.equals(cmd_list_motion)) {
            Platform.runLater(() -> {
                List<String> items = Arrays.asList(new String(result).split("\n"));
                list.getItems().setAll(items);
            });
        }
    }

    @Override
    public void handleError(int code) {
        System.err.println("Got error response: " + code);
    }
    
    class ListCommand extends Command {
        private final String listName;

        public ListCommand(String listName, String commandName) {
            super(commandName);
            this.listName = listName;
        }

        @Override
        public String toString() {
            return listName;
        }
    }
    
    class DataHandlerPrint implements ObjectListener<byte[]>
    {
        @Override
        public void newObjectReceived(byte[] object) {
            Platform.runLater(() -> {
                content.setText(new String(object));
            });
        }

        @Override
        public void errorOccured(String cause) {
            Platform.runLater(() -> {
                content.setPromptText(cause);
                content.clear();
            });
        }
    }
}
