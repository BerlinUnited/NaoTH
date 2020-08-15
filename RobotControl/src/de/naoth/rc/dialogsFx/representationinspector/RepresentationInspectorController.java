package de.naoth.rc.dialogsFx.representationinspector;

import com.google.protobuf.Descriptors;
import com.google.protobuf.DynamicMessage;
import com.google.protobuf.InvalidProtocolBufferException;
import de.naoth.rc.RobotControl;
import de.naoth.rc.core.dialog.AbstractJFXDialog;
import de.naoth.rc.core.dialog.DialogPlugin;
import de.naoth.rc.core.dialog.RCDialog;
import de.naoth.rc.core.manager.ObjectListener;
import de.naoth.rc.logmanager.BlackBoard;
import de.naoth.rc.logmanager.LogFileEventManager;
import de.naoth.rc.logmanager.LogFrameListener;
import de.naoth.rc.manager.GenericManager;
import de.naoth.rc.manager.GenericManagerFactory;
import de.naoth.rc.core.messages.AudioDataOuterClass;
import de.naoth.rc.core.messages.FrameworkRepresentations;
import de.naoth.rc.core.messages.Messages;
import de.naoth.rc.core.messages.Representations;
import de.naoth.rc.core.messages.TeamMessageOuterClass;
import de.naoth.rc.core.server.Command;
import de.naoth.rc.core.server.ResponseListener;
import java.net.URL;
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.stream.Collector;
import java.util.stream.Collectors;
import javafx.application.Platform;
import javafx.beans.binding.Bindings;
import javafx.beans.property.SimpleStringProperty;
import javafx.beans.property.StringProperty;
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
public class RepresentationInspectorController  extends AbstractJFXDialog implements ResponseListener
{
    @RCDialog(category = RCDialog.Category.Status, name = "Representations (FX)")
    @PluginImplementation
    public static class Plugin extends DialogPlugin<RepresentationInspectorController> {
        @InjectPlugin
        public static RobotControl parent;
        @InjectPlugin
        public static GenericManagerFactory genericManagerFactory;
        @InjectPlugin
        public static LogFileEventManager logFileEventManager;
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
    private Listener dataListener;
    private final LogRepresentationListener logHandler = new LogRepresentationListener();
    
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
        
        //SynchronizedObservableList
        list.getSelectionModel().selectedItemProperty().addListener((ob, o, n) -> {
            dataListener.setSelected(n);
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
                    dataListener = new LogRepresentationListener();
                    Plugin.logFileEventManager.addListener((LogFrameListener) dataListener);
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
    
    class Listener
    {
        protected String selected = null;

        /**
         * Set/updates the new list data.
         * @param c 
         */
        public void updateList(Collection<String> c) {
            Platform.runLater(() -> {
                // NOTE: setAll removes the selection, need to preserve it
                String s = selected;
                list.getItems().setAll(c.stream().sorted().collect(Collectors.toList()));
                list.getSelectionModel().select(s);
                selected = s;
            });
        }
        
        public void updateContent(String c) {
            Platform.runLater(() -> {
                content.setText(c);
            });
        }
        
        public void setSelected(String s) {
            selected = s;
        }
    }
    
    
    private class LogRepresentationListener extends Listener implements LogFrameListener
    {
        private final Map<String, byte[]> dataByName = new HashMap<>();
        private final Map<String, Descriptors.Descriptor> pbMessages = new HashMap<>();

        public LogRepresentationListener() {
            retrievePbMessages(FrameworkRepresentations.getDescriptor());
            retrievePbMessages(Representations.getDescriptor());
            retrievePbMessages(TeamMessageOuterClass.getDescriptor());
            retrievePbMessages(Messages.getDescriptor());
            retrievePbMessages(AudioDataOuterClass.getDescriptor());
        }
        
        private void retrievePbMessages(Descriptors.FileDescriptor d) {
            d.getMessageTypes().forEach((desc) -> {
                pbMessages.put(desc.getName(), desc);
            });
        }
        
        @Override
        public void newFrame(BlackBoard b) {
            // clear old frame data
            dataByName.clear();
            // map new frame data
            b.getNames().forEach((name) -> { dataByName.put(name, b.get(name).getData()); });
            // update list
            updateList(dataByName.keySet());
            
            showFrame();
        }

        @Override
        public void setSelected(String s) {
            super.setSelected(s);
            showFrame();
        }
        
        public void showFrame() {
            if(selected != null) {
                if(pbMessages.containsKey(selected)) {
                    if(dataByName.containsKey(selected)) {
                        try {
                            DynamicMessage r = DynamicMessage.parseFrom(pbMessages.get(selected), dataByName.get(selected));
                            updateContent(r.toString());
                        } catch (InvalidProtocolBufferException ex) {
                            updateContent("Error while parsing.");
                            Logger.getLogger(getClass().getName()).log(Level.SEVERE, null, ex);
                        }
                    } else {
                        updateContent("No data available: " + selected);
                    }
                } else {
                    updateContent("Unknown protobuf message: " + selected);
                }
            }
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
