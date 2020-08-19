package de.naoth.rc.dialogsFx.representationinspector;

import com.google.protobuf.InvalidProtocolBufferException;
import de.naoth.rc.RobotControl;
import de.naoth.rc.core.manager.ObjectListener;
import de.naoth.rc.logmanager.LogFileEventManager;
import de.naoth.rc.manager.GenericManager;
import de.naoth.rc.manager.GenericManagerFactory;
import de.naoth.rc.core.messages.MessageRegistry;
import de.naoth.rc.core.server.Command;
import de.naoth.rc.core.server.ResponseListener;
import java.util.Arrays;
import java.util.List;
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
    
    private final RepresentationListListener representationsListListener = new RepresentationListListener();
    
    private GenericManager currentManager;
    private DataHandler currentHandler;
    
    //private final LogRepresentationListener logHandler = new LogRepresentationListener();
    
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
        // handle list changes
        list.disableProperty().bind(btnRefresh.selectedProperty().or(btnLog.selectedProperty()).not());
        list.getSelectionModel().selectedItemProperty().addListener((o) -> { subscribeRepresentation(); });
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
            unsubscribeRepresentation();
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
     * Is called, when the binary button is toggled.
     * Subscribes to a selected representation in binary mode.
     */
    @FXML
    private void retrieveBinary() {
        subscribeRepresentation();
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
    
    /**
     * Subscribes to a selected representation, if one is selected.
     */
    private void subscribeRepresentation() {
        // unsubscribe previous listener
        unsubscribeRepresentation();
        // subscribe to the new representation (if set)
        String representation = list.getSelectionModel().getSelectedItem();
        if (representation != null) {
            if (btnLog.isSelected()) {
                // TODO!
                //logHandler.showFrame((String) o);
            } else {
                String suffix;
                if (btnBinary.isSelected()) {
                    currentHandler = new DataHandlerBinary(representation);
                    suffix = "get";
                } else {
                    currentHandler = new DataHandlerPrint();
                    suffix = "print";
                }

                Command cmd = new Command(type.getValue().toString() + ":representation:" + suffix).addArg(representation);
                currentManager = factory.getManager(cmd);
                currentManager.addListener((ObjectListener<byte[]>) currentHandler);
            }
        }
    }
    
    /**
     * Unsubscribes from a (previously subscribed) representation.
     */
    private void unsubscribeRepresentation() {
        if (currentManager != null) {
            currentManager.removeListener((ObjectListener<byte[]>)currentHandler);
        }
    }
    
    /**
     * Updates the content view with the given text.
     * 
     * @param text  the new text to show
     */
    private void updateContent(String text) {
        Platform.runLater(() -> {
            // NOTE: content.setText() changes the scrolbar position
            content.replaceText(0, content.getText().length(), text);
        });
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
    class RepresentationListListener implements ResponseListener
    {
        @Override
        public void handleResponse(byte[] result, Command command) {
            if (command.equals(cmd_list_cognition) || command.equals(cmd_list_motion)) {
                Platform.runLater(() -> {
                    List<String> items = Arrays.asList(new String(result).split("\n"));
                    String selected = list.getSelectionModel().getSelectedItem();
                    list.getItems().setAll(items);
                    list.getSelectionModel().select(selected);
                    list.scrollTo(selected);
                });
            }
        }

        @Override
        public void handleError(int code) {
            System.err.println("Got error response: " + code);
        }
    }
    
    /**
     * Common name for all data handler (print, binary, log).
     */
    interface DataHandler {}
    
    /**
     * Handles the string representation of the subscribed representation.
     */
    class DataHandlerPrint implements ObjectListener<byte[]>, DataHandler
    {
        @Override
        public void newObjectReceived(byte[] object) {
            updateContent(new String(object));
        }

        @Override
        public void errorOccured(String cause) {
            Platform.runLater(() -> {
                content.setPromptText(cause);
                content.clear();
            });
        }
    }
    
    /**
     * Handles the binary representation of the subscribed representation.
     */
    class DataHandlerBinary implements ObjectListener<byte[]>, DataHandler
    {
        private final String name;
        
        public DataHandlerBinary(String name) {
            // EVIL HACK: remove suffix "Top"
            this.name = name.endsWith("Top") ? name.substring(0, name.length()-3) : name;
        }
        
        @Override
        public void newObjectReceived(byte[] data) {
            if (MessageRegistry.has(name)) {
                try {
                    updateContent(MessageRegistry.parse(name, data).toString());
                } catch (InvalidProtocolBufferException ex) {
                    updateContent("Error while parsing: " + ex.getMessage());
                }
            } else {
                updateContent("No binary serialization avaliable.");
            }
        }//end newObjectReceived

        @Override
        public void errorOccured(String cause) {
            Platform.runLater(() -> {
                content.setPromptText(cause);
                content.clear();
            });
        }
    }//end class DataHandlerBinary
    
    /*
    // TODO
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
    */
}
