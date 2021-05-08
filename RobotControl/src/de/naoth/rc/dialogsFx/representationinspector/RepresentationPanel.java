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
import java.util.Map;
import java.util.TreeMap;
import javafx.application.Platform;
import javafx.beans.property.ObjectProperty;
import javafx.beans.property.SimpleObjectProperty;
import javafx.collections.FXCollections;
import javafx.fxml.FXML;
import javafx.scene.control.Accordion;
import javafx.scene.control.ListView;
import javafx.scene.control.TextArea;
import javafx.scene.control.TextField;
import javafx.scene.control.TitledPane;
import javafx.scene.control.Toggle;
import javafx.scene.control.ToggleButton;
import javafx.scene.control.ToggleGroup;
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
    public enum Source { None, Cognition, Motion, Log }
    /** The types of the sources */
    public enum Format { Text, Binary }
    /** The current active source */
    private final ObjectProperty<Source> source = new SimpleObjectProperty<>(Source.None);
    /** The current active source type */
    private final ObjectProperty<Format> format = new SimpleObjectProperty<>(Format.Text);
    
    /** UI elements */
    @FXML Accordion sourcePanel;
    @FXML TitledPane cognitionPane;
    @FXML TitledPane motionPane;
    @FXML TitledPane logPane;
    @FXML ListView<String> cognitionList;
    @FXML ListView<String> motionList;
    @FXML ListView<String> logList;
    @FXML ToggleGroup formatPanel;
    @FXML ToggleButton btnText;
    @FXML ToggleButton btnBinary;
    @FXML TextArea content;
    @FXML TextField search;
    
    /** Some shortcuts */
    private final KeyCombination shortcutCognition = new KeyCodeCombination(KeyCode.C, KeyCombination.ALT_DOWN, KeyCombination.SHIFT_DOWN);
    private final KeyCombination shortcutMotion = new KeyCodeCombination(KeyCode.M, KeyCombination.ALT_DOWN, KeyCombination.SHIFT_DOWN);
    private final KeyCombination shortcutLog = new KeyCodeCombination(KeyCode.L, KeyCombination.ALT_DOWN, KeyCombination.SHIFT_DOWN);
    private final KeyCombination shortcutText = new KeyCodeCombination(KeyCode.T, KeyCombination.ALT_DOWN, KeyCombination.SHIFT_DOWN);
    private final KeyCombination shortcutBinary = new KeyCodeCombination(KeyCode.B, KeyCombination.ALT_DOWN, KeyCombination.SHIFT_DOWN);
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
        // handle changes of the current state
        source.addListener((ob) -> { sourceChanged(); });
        format.addListener((ob) -> { formatChanged(); });

        // change the source
        sourcePanel.expandedPaneProperty().addListener((ob) -> { switchSource(); });
        formatPanel.selectedToggleProperty().addListener((ob, o, n) -> { switchFormat(o, n); });

        // handle list changes
        cognitionList.getSelectionModel().selectedItemProperty().addListener((o) -> { subscribeRepresentation(cognitionList); });
        motionList.getSelectionModel().selectedItemProperty().addListener((o) -> { subscribeRepresentation(motionList); });
        // update content view with the new selected representation
        logList.getSelectionModel().selectedItemProperty().addListener((o) -> { dataHandlerLog.showFrame(); });

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
    public ObjectProperty<Format> getFormatProperty() {
        return format;
    }
    
    /**
     * Handles the Text format button click.
     */
    @FXML
    private void fxFormatText() {
        if (btnText.isSelected()) {
            format.set(Format.Text);
        }
    }

    /**
     * Handles the Binary format button click.
     */
    @FXML
    private void fxFormatBinary() {
        if (btnText.isSelected()) {
            format.set(Format.Binary);
        }
    }

    /**
     * Handles the "Not connected" button click on the empty cogntion list.
     */
    @FXML
    private void fxCognitionConnect() {
        retrieveRepresentations(cmd_list_cognition);
    }
    
    /**
     * Handles the "Not connected" button click on the empty motion list.
     */
    @FXML
    private void fxMotionConnect() {
        retrieveRepresentations(cmd_list_motion);
    }

    /**
     * Is called, if the key pressed event is triggered on the panel.
     * 
     * @param k the key (event), which triggered the event
     */
    @FXML
    private void fxPanelShortcuts(KeyEvent k) {
        if (shortcutRefresh.match(k)) {
            sourceChanged();
        } else if (shortcutCognition.match(k)) {
            source.set(Source.Cognition);
        } else if (shortcutMotion.match(k)) {
            source.set(Source.Motion);
        } else if (shortcutLog.match(k)) {
            source.set(Source.Log);
        } else if (shortcutText.match(k)) {
            format.set(Format.Text);
        } else if (shortcutBinary.match(k)) {
            format.set(Format.Binary);
        }
    }
    
    /**
     * Is called, if the key pressed event is triggered on the list view.
     * 
     * @param k the key (event), which triggered the event
     */
    @FXML
    private void fxListShortcuts(KeyEvent k) {
        // check if we got a printable character and the ALT key isn't pressed
        // since this is used for shortcuts
        if (source.get() != Source.None && !k.isAltDown() && k.getText() != null && !k.getText().isEmpty()) {
            // "fast" typing is one search string; a pause results in a new search
            if (System.currentTimeMillis() - listSearchTimeout < 1000) {
                listSearchString += k.getText().toLowerCase();
            } else {
                listSearchString = k.getText().toLowerCase();
            }
            listSearchTimeout = System.currentTimeMillis();
            // search for the typed string and select/scroll to the respective item
            ListView<String> list = (ListView<String>) sourcePanel.getExpandedPane().getContent();
            for (String item : list.getItems()) {
                if (item.toLowerCase().startsWith(listSearchString)) {
                    list.getSelectionModel().select(item);
                    list.scrollTo(item);
                    break;
                }
            }
        }
    }
    
    /**
     * Is called, if the key pressed event is triggered on the content text area.
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
            sourceChanged();
        }
    }
    
    /**
     * Is called, if the key pressed event is triggered on the search text field.
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
     * Handles the source change by the ui.
     */
    private void switchSource() {
        TitledPane expanded = sourcePanel.getExpandedPane();
        if (expanded == cognitionPane) {
            source.set(Source.Cognition);
        } else if (expanded == motionPane) {
            source.set(Source.Motion);
        } else if (expanded == logPane) {
            source.set(Source.Log);
        } else {
            source.set(Source.None);
        }
    }
    
    /**
     * Handles the format change by the ui.
     */
    private void switchFormat(Toggle o, Toggle n) {
        if (n == null) {
            o.setSelected(true);
        } else if (n == btnText) {
            format.set(Format.Text);
        } else if (n == btnBinary) {
            format.set(Format.Binary);
        }
    }

    /**
     * Handles changes to the current source.
     * The appropiate handler is set or unset, the selected representation is
     * subscribed or unsubscribed and the ui is updated.
     */
    private void sourceChanged() {
        switch (source.get()) {
            case Cognition:
                retrieveRepresentations(cmd_list_cognition);
                sourcePanel.setExpandedPane(cognitionPane);
                break;
            case Motion:
                retrieveRepresentations(cmd_list_motion);
                sourcePanel.setExpandedPane(motionPane);
                break;
            case Log:
                log.addListener(dataHandlerLog);
                dataHandlerLog.showFrame();
                sourcePanel.setExpandedPane(logPane);
                break;
            case None:
                unsubscribeRepresentation();
                log.removeListener(dataHandlerLog);
                sourcePanel.setExpandedPane(null);
                break;
        }
    }

    /**
     * Handles changes of the current format (text, binary) and retrieves
     * the appropiate reperesentation based on the format.
     */
    private void formatChanged() {
        // selects the appropiate button, if not already selected
        switch (format.get()) {
            case Text: btnText.setSelected(true); break;
            case Binary: btnBinary.setSelected(true); break;
        }
        // re-subscribe to the selected representation
        if (source.get() == Source.Cognition) {
            subscribeRepresentation(cognitionList);
        } else if (source.get() == Source.Motion) {
            subscribeRepresentation(motionList);
        }
    }
    
    /**
     * Retrieves a list of the available representations.
     */
    private void retrieveRepresentations(Command cmd) {
        if (control != null && control.checkConnected()) {
            control.getMessageServer().executeCommand(representationsListUpdater, cmd);
        }
    }
    
    /**
     * Subscribes to a selected representation, if one is selected.
     */
    private void subscribeRepresentation(ListView<String> list) {
        // unsubscribe previous listener
        unsubscribeRepresentation();
        // only Cognition and Motion can be subscribed
        if (list != null && source.get() != Source.Cognition && source.get() != Source.Motion) {
            return;
        }
        // make sure we're connected
        if (control != null && control.checkConnected()) {
            // subscribe to the new representation (if set)
            String selected = list.getSelectionModel().getSelectedItem();
            if (selected != null) {
                switch (format.get()) {
                    case Text:
                        Command cmdPrint = new Command(source.get().toString() + ":representation:print").addArg(selected);
                        dataHandlerManager = factory.getManager(cmdPrint);
                        dataHandlerManager.addListener(dataHandlerPrint);
                        break;
                    case Binary:
                        dataHandlerBinary.setRepresentation(selected);
                        Command cmdBinary = new Command(source.get().toString() + ":representation:get").addArg(selected);
                        dataHandlerManager = factory.getManager(cmdBinary);
                        dataHandlerManager.addListener(dataHandlerBinary);
                        break;
                }
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
    private void updateList(ListView<String> list, Collection<String> items) {
        Platform.runLater(() -> {
            String selected = list.getSelectionModel().getSelectedItem();
            list.setItems(FXCollections.observableArrayList(items));
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
            if (command.equals(cmd_list_cognition)) {
                updateList(cognitionList, Arrays.asList(new String(result).split("\n")));
            } else if (command.equals(cmd_list_motion)) {
                updateList(motionList, Arrays.asList(new String(result).split("\n")));
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
        /** The data for the "current" frame indexed by representation name. */
        private final Map<String, byte[]> dataByName = new TreeMap<>();

        @Override
        public void newFrame(BlackBoard b) {
            // NOTE: the blackboard never deletes representations!
            //       so we don't need to clear the map, since in the next step it is
            //       overwritten anyway and the ui must only be updated if blackboard
            //       actually changes.

            // map new frame data
            b.getNames().forEach((name) -> {
                dataByName.put(name, b.get(name).getData());
            });

            // update list, if necessary
            if (logList.getItems().size() != dataByName.keySet().size()) {
                updateList(logList, dataByName.keySet());
            }

            showFrame();
        }

        public void showFrame() {
            String selected = logList.getSelectionModel().getSelectedItem();
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
