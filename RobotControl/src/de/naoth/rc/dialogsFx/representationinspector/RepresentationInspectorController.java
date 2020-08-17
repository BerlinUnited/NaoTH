package de.naoth.rc.dialogsFx.representationinspector;

import com.google.protobuf.Descriptors;
import com.google.protobuf.DynamicMessage;
import com.google.protobuf.InvalidProtocolBufferException;
import de.naoth.rc.RobotControl;
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
import java.util.Arrays;
import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.stream.Collectors;
import javafx.application.Platform;
import javafx.fxml.FXML;
import javafx.scene.control.ChoiceBox;
import javafx.scene.control.ListView;
import javafx.scene.control.TextArea;
import javafx.scene.control.ToggleButton;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class RepresentationInspectorController
{
    /** The used robot control instance */
    private RobotControl control;
    private LogFileEventManager log;
    private GenericManagerFactory factory;
    
    @FXML ToggleButton btnRefresh;
    @FXML ToggleButton btnBinary;
    @FXML ToggleButton btnLog;
    @FXML ChoiceBox<Command> type;
    @FXML ListView<String> list;
    @FXML TextArea content;

    private final Command cmd_list_cognition = new ListCommand("Cognition", "Cognition:representation:list");
    private final Command cmd_list_motion = new ListCommand("Motion", "Motion:representation:list");
    
    private final RepresentationsListListener representationsListListener = new RepresentationsListListener();
    private RepresentationListener dataListener;
    
    private GenericManager currentManager;
    private ObjectListener currentHandler;
    private final LogRepresentationListener logHandler = new LogRepresentationListener();
    
    /**
     * Default constructor for the FXML loader.
     */
    public RepresentationInspectorController() {}
    
    /**
     * Constructor for custom initialization.
     * @param control
     */
    public RepresentationInspectorController(RobotControl control) {
        setRobotControl(control);
    }
    
    @FXML
    private void initialize()
    {
        // set up the type dropdownbox
        type.getItems().add(cmd_list_cognition);
        type.getItems().add(cmd_list_motion);
        type.setValue(cmd_list_cognition);
  
        // install some ui (button) dependencies (enable/disable on select)
        btnLog.disableProperty().bind(btnRefresh.selectedProperty());
        btnBinary.disableProperty().bind(btnRefresh.selectedProperty().not());
        type.disableProperty().bind(btnRefresh.selectedProperty().not());
        btnRefresh.disableProperty().bind(btnLog.selectedProperty());
        btnRefresh.selectedProperty().addListener((o) -> {
            if (!btnRefresh.isSelected() && btnBinary.isSelected()) { btnBinary.setSelected(false); }
        });
        
        //SynchronizedObservableList
        list.getSelectionModel().selectedItemProperty().addListener((o) -> {
            
        });
        
        /*
        // TODO
        listener.selectedToggleProperty().addListener((o, oldButton, newButton) -> {
            if(oldButton != null) {
                unsubscribeRepresentations();
            }
            
            if(newButton != null) {
                if(newButton.equals(btnRefresh)) {
                    if(server != null && server.isConnected()) {
                        server.executeCommand(this, type.getValue());
                    } else {
                        btnRefresh.setSelected(false);
                    }
                } else if(newButton.equals(btnBinary)) {
                    content.setText("TODO: Binary!");
                } else if(newButton.equals(btnLog)) {
                    dataListener = new LogRepresentationListener();
                    log.addListener((LogFrameListener) dataListener);
                }
            }
        });
        */
    }

    /**
     * Sets the robot control instance.
     * @param control the used robot control instance
     */
    public void setRobotControl(RobotControl control) {
        this.control = control;
    }
    
    // TODO
    public void setLogFileEventManager(LogFileEventManager manager) {
        this.log = manager;
    }

    // TODO
    public void setGenericManagerFactory(GenericManagerFactory factory) {
        this.factory = factory;
    }
    
    /**
     * Gets called, when the refresh button is un-/selected.
     * Retrieves a list of the available representations.
     */
    @FXML
    private void updateRepresentations() {
        if (btnRefresh.isSelected()) {
            retrieveRepresentations();
        } else {
            // TODO: unregister listener
        }
    }

    /**
     * Is called when the dropdownbox changes.
     * Based on the selected item a list of the available representations is retrieved.
     */
    @FXML
    private void typeChanged() {
        unsubscribeRepresentation();
        retrieveRepresentations();
    }
    
    /**
     * Retrieves a list of the available representations.
     * If not connected, the refresh button is set to "un-selected".
     */
    private void retrieveRepresentations() {
        if (control != null && control.checkConnected()) {
            control.getMessageServer().executeCommand(representationsListListener, type.getValue());
        } else {
            btnRefresh.setSelected(false);
        }
    }
    
    private void subscribeRepresentation(String n) {
        // unsubscribe previous listener
        unsubscribeRepresentation();
        // subscribe to the new representation (if set)
        if(n != null) {
            currentHandler = new DataHandlerPrint();
            Command cmd = new Command(type.getValue().toString() + ":representation:print").addArg(n);

            currentManager = factory.getManager(cmd);
            currentManager.addListener(currentHandler);
        }
    }
    
    private void unsubscribeRepresentation() {
        if (currentManager != null) {
            currentManager.removeListener(currentHandler);
        }
    }

    /**
     * Extends the command with a display name. This is used in the list view.
     */
    class ListCommand extends Command
    {
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
    
    /**
     * Handles the response of a representation list command.
     */
    class RepresentationsListListener implements ResponseListener
    {
        @Override
        public void handleResponse(byte[] result, Command command) {
            if (command.equals(cmd_list_cognition) || command.equals(cmd_list_motion)) {
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
    }
    
    class RepresentationListener
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
    
    
    private class LogRepresentationListener extends RepresentationListener implements LogFrameListener
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
    
    /*
    class DataHandlerBinary implements ObjectListener<byte[]> {

        private final Parser parser;
        //private final List<Descriptor> allDescriptors = getAllProtobufDescriptors();

        public DataHandlerBinary(String name) {
            
            Class<?> parserClass = getProtobufClass(name);
            if (parserClass != null) {
                parser = new ProtobufParser(parserClass);
            } else {
                parser = new EmptyParser();
            }
        }

        @Override
        public void newObjectReceived(byte[] data) {

            Object result = parser.parse(data);
            if (result != null) {
                textArea.setText(result.toString());
            } else {
                textArea.setText("Error while parsing.\n");
                //textArea.append(new String(data));
            }

        }//end newObjectReceived

        @Override
        public void errorOccured(String cause) {
            handleError(cause);
        }
    }//end class DataHandlerPrint
    */
}
