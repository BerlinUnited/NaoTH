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
import de.naoth.rc.logmanager.BlackBoard;
import de.naoth.rc.logmanager.LogFrameListener;
import java.util.Arrays;
import java.util.Collection;
import java.util.HashMap;
import java.util.Map;
import javafx.application.Platform;
import javafx.beans.property.ObjectProperty;
import javafx.beans.property.SimpleObjectProperty;
import javafx.fxml.FXML;
import javafx.scene.control.ListView;
import javafx.scene.control.TextArea;
import javafx.scene.control.TextField;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyCodeCombination;
import javafx.scene.input.KeyCombination;
import javafx.scene.input.KeyEvent;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class RepresentationPanel
{
    /** The used robot control instance */
    private RobotControl control;
    private LogFileEventManager log;
    private GenericManagerFactory factory;
    
    /** The available sources */
    public enum Source { None, Connection, Binary, Log }
    /** The types of the sources */
    public enum Type { Cognition, Motion }
    /** The current active source */
    private final ObjectProperty<Source> source = new SimpleObjectProperty<>(Source.None);
    /** The current active source type */
    private final ObjectProperty<Type> type = new SimpleObjectProperty<>(Type.Cognition);
    
    /** UI elements */
    @FXML ListView<String> list;
    @FXML TextArea content;
    @FXML TextField search;
    
    /** Some shortcuts */
    private final KeyCombination shortcutShowSearch = new KeyCodeCombination(KeyCode.F, KeyCombination.CONTROL_DOWN);
    private final KeyCombination shortcutHideSearch = new KeyCodeCombination(KeyCode.ESCAPE);
    private final KeyCombination shortcutContSearchEnter = new KeyCodeCombination(KeyCode.ENTER);
    private final KeyCombination shortcutContSearchF3 = new KeyCodeCombination(KeyCode.F3);
    private final KeyCombination shortcutRefresh = new KeyCodeCombination(KeyCode.F5);
    
    /** Some search related vars */
    private int lastSearchPos = -1;
    private long listSearchTimeout = 0;
    private String listSearchString = "";

    /** The commands to retrieve the available representations */
    private final Command cmd_list_cognition = new Command("Cognition:representation:list");
    private final Command cmd_list_motion = new Command("Motion:representation:list");
    
    /** Some data handler */
    private GenericManager dataHandlerManager;
    private final RepresentationListUpdater representationsListUpdater = new RepresentationListUpdater();
    private final DataHandlerPrint dataHandlerPrint = new DataHandlerPrint();
    private final DataHandlerBinary dataHandlerBinary = new DataHandlerBinary();
    private final DataHandlerLog dataHandlerLog = new DataHandlerLog();
    
    /**
     * Default constructor for the FXML loader.
     */
    public RepresentationPanel() {}
    
    /**
     * Constructor for custom initialization.
     * @param control
     */
    public RepresentationPanel(RobotControl control) {
        setRobotControl(control);
    }
    
    /**
     * Gets called, after the FXML file was loaded.
     */
    @FXML
    private void initialize() {
        // handle changes of the global state (set by parent)
        type.addListener((ob) -> { changeType(); });
        source.addListener((ob) -> { changeSource(); });
        // handle list changes
        list.disableProperty().bind(source.isEqualTo(Source.None));
        list.getSelectionModel().selectedItemProperty().addListener((o) -> { subscribeRepresentation(); });
        // handle search input
        search.textProperty().addListener((ob) -> { search(true); });
    }

    /**
     * Sets the robot control instance.
     * @param control the used robot control instance
     */
    public void setRobotControl(RobotControl control) {
        this.control = control;
    }
    
    /**
     * Sets the log event manager.
     * 
     * @param manager the (global) log event manager
     */
    public void setLogFileEventManager(LogFileEventManager manager) {
        this.log = manager;
    }

    /**
     * Sets the manager for subscribing to representations.
     * 
     * @param factory the subscriber manager
     */
    public void setGenericManagerFactory(GenericManagerFactory factory) {
        this.factory = factory;
    }
    
    /**
     * Returns the current source property (Print, Binary, Log)
     * 
     * @return the (current) source property
     */
    public ObjectProperty<Source> getSourceProperty() {
        return source;
    }

    /**
     * Returns the current type property (Cognition, Motion).
     * 
     * @return the (current) type property
     */
    public ObjectProperty<Type> getTypeProperty() {
        return type;
    }

    /**
     * Is called, if the key pressed event is triggered by the list view.
     * 
     * @param k the key (event), which triggered the event
     */
    @FXML
    private void fxListShortcuts(KeyEvent k) {
        // check if we got a printable character
        if (k.getText() != null && !k.getText().isEmpty()) {
            // "fast" typing is one search string; a pause results in a new search
            if (System.currentTimeMillis() - listSearchTimeout < 1000) {
                listSearchString += k.getText().toLowerCase();
            } else {
                listSearchString = k.getText().toLowerCase();
            }
            listSearchTimeout = System.currentTimeMillis();
            // search for the typed string and select/scroll to the respective item
            for (String item : list.getItems()) {
                if (item.toLowerCase().startsWith(listSearchString)) {
                    list.getSelectionModel().select(item);
                    list.scrollTo(item);
                    break;
                }
            }
        } else if (shortcutRefresh.match(k)) {
            // refresh makes only sence when somehow connected
            if (source.get() != Source.Connection || source.get() != Source.Binary) {
                changeSource();
            }
        }
    }
    
    /**
     * Is called, if the key pressed event is triggered by the content text area.
     * 
     * @param k the key (event), which triggered the event
     */
    @FXML
    private void fxTextAreaShortcuts(KeyEvent k) {
        if (shortcutShowSearch.match(k)) {
            search.setVisible(true);
            search.requestFocus();
            search(true);
        } else if(shortcutContSearchF3.match(k)) {
            search(false);
        } else if (shortcutRefresh.match(k)) {
            // F5 is consumed by the textarea and does not bubble to the parent 
            // node, so we must handle it here
            if (source.get() != Source.Connection || source.get() != Source.Binary) {
                changeSource();
            }
        }
    }
    
    /**
     * Is called, if the key pressed event is triggered by the search text field.
     * 
     * @param k the key (event), which triggered the event
     */
    @FXML
    private void fxSearchFieldShortcuts(KeyEvent k) {
        if (shortcutHideSearch.match(k)) {
            search.setVisible(false);
            content.deselect();
            content.requestFocus();
        } else if (shortcutContSearchEnter.match(k) || shortcutContSearchF3.match(k)) {
            search(false);
        }
    }
    
    /**
     * Searches the content for the search string.
     * 
     * @param begin true, if the search should start from the beginning, false otherwise
     */
    private void search(boolean begin) {
        String text = search.getText();
        if (text != null && !text.isEmpty()){
            int found = content.getText().indexOf(text);
            int foundNext = content.getText().indexOf(text, lastSearchPos);
            // is there any occurence?
            if (found != -1) {
                if (!begin && foundNext != -1) {
                    // select the next, if there's one
                    content.selectRange(foundNext, foundNext + text.length());
                    lastSearchPos = foundNext + text.length();
                } else {
                    // otherwise select the first occurence
                    content.selectRange(found, found + text.length());
                    lastSearchPos = found + text.length();
                }
            }
        }
    }
    
    /**
     * Handles changes to the current source.
     * The appropiate handler is set or unset and the selected representation is
     * subscribed or unsubscribed.
     */
    private void changeSource() {
        switch (source.get()) {
            case Connection:
            case Binary:
                retrieveRepresentations();
                break;
            case Log:
                list.getItems().clear();
                log.addListener(dataHandlerLog);
                dataHandlerLog.showFrame();
                break;
            case None:
                unsubscribeRepresentation();
                log.removeListener(dataHandlerLog);
                break;
        }
    }

    /**
     * Handles changes of the current source type (cognition, motion) and retrieves
     * the appropiate reperesentation based on the type.
     */
    private void changeType() {
        if (source.get() == Source.Connection || source.get() == Source.Binary) {
            unsubscribeRepresentation();
            retrieveRepresentations();
        }
    }
    
    /**
     * Retrieves a list of the available representations.
     * If not connected, the source is set to "None".
     */
    private void retrieveRepresentations() {
        if (control != null && control.checkConnected()) {
            if (type.get() == Type.Cognition) {
                control.getMessageServer().executeCommand(representationsListUpdater, cmd_list_cognition);
            } else if (type.get() == Type.Motion) {
                control.getMessageServer().executeCommand(representationsListUpdater, cmd_list_motion);
            }
        } else {
            source.set(Source.None);
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
            switch (source.get()) {
                case Connection:
                    Command cmdPrint = new Command(type.get().toString() + ":representation:print").addArg(representation);
                    dataHandlerManager = factory.getManager(cmdPrint);
                    dataHandlerManager.addListener(dataHandlerPrint);
                    break;
                case Binary:
                    dataHandlerBinary.setRepresentation(representation);
                    Command cmdBinary = new Command(type.get().toString() + ":representation:get").addArg(representation);
                    dataHandlerManager = factory.getManager(cmdBinary);
                    dataHandlerManager.addListener(dataHandlerBinary);
                    break;
                case Log:
                    dataHandlerLog.showFrame();
                    break;
                case None:
                    // do nothing, since no source is set/selected
                    break;
            }
        }
    }
    
    /**
     * Unsubscribes from a (previously subscribed) representation.
     */
    private void unsubscribeRepresentation() {
        if (dataHandlerManager != null) {
            // make sure there's no handler subscribed
            dataHandlerManager.removeListener(dataHandlerBinary);
            dataHandlerManager.removeListener(dataHandlerPrint);
        }
    }
    
    /**
     * Updates the representation list and re-selects a previously selected item.
     * 
     * @param items the new representations
     */
    private void updateList(Collection<String> items) {
        Platform.runLater(() -> {
            String selected = list.getSelectionModel().getSelectedItem();
            list.getItems().setAll(items);
            list.getSelectionModel().select(selected);
        });
    }
    
    /**
     * Updates the content view with the given text and restores the highlighted
     * search text.
     * 
     * @param text  the new text to show
     */
    private void updateContent(String text) {
        Platform.runLater(() -> {
            // remember selection/search before replace text
            String selection = content.getSelectedText();
            if (!selection.isEmpty()) {
                lastSearchPos = lastSearchPos - selection.length();
            }
            
            // NOTE: content.setText() changes the scrolbar position and the caret
            //       position has sometimes also unwanted behavior, so we set it
            //       back where it was before
            int pos = content.getCaretPosition();
            content.replaceText(0, content.getText().length(), text);
            content.positionCaret(pos);
            
            // re-select/search text in the new/replaced text
            if (!selection.isEmpty()) {
                search(false);
            }
        });
    }

    /**
     * Handles the response of a representation list command.
     */
    class RepresentationListUpdater implements ResponseListener
    {
        @Override
        public void handleResponse(byte[] result, Command command) {
            if (command.equals(cmd_list_cognition) || command.equals(cmd_list_motion)) {
                updateList(Arrays.asList(new String(result).split("\n")));
            }
        }

        @Override
        public void handleError(int code) {
            System.err.println("Got error response: " + code);
        }
    }
    
    /**
     * Handles the string representation of the subscribed representation.
     */
    class DataHandlerPrint implements ObjectListener<byte[]>
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
    class DataHandlerBinary implements ObjectListener<byte[]>
    {
        private String name;
        
        public void setRepresentation(String name) {
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
            System.err.println(cause);
            Platform.runLater(() -> {
                content.setPromptText(cause);
                content.clear();
            });
        }
    }//end class DataHandlerBinary
    
    /**
     * Handles log frame events and displays the data from the log frame.
     */
    private class DataHandlerLog implements LogFrameListener
    {
        /** The data for the current frame indexed by representation name. */
        private final Map<String, byte[]> dataByName = new HashMap<>();
        
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

        public void showFrame() {
            String selected = list.getSelectionModel().getSelectedItem();
            if(selected != null) {
                // EVIL HACK: remove suffix "Top"
                String representation = selected.endsWith("Top") ? selected.substring(0, selected.length()-3) : selected;
                if(MessageRegistry.has(representation)) {
                    if(dataByName.containsKey(selected)) {
                        try {
                            updateContent(MessageRegistry.parse(representation, dataByName.get(selected)).toString());
                        } catch (InvalidProtocolBufferException ex) {
                            updateContent("Error while parsing.");
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
}
