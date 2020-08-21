package de.naoth.rc.dialogsFx.parameters;

import de.naoth.rc.RobotControl;
import de.naoth.rc.core.server.Command;
import de.naoth.rc.core.server.ResponseListener;
import java.util.Arrays;
import java.util.Collection;
import java.util.HashMap;
import java.util.Map;
import java.util.TreeMap;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import javafx.animation.FadeTransition;
import javafx.application.Platform;
import javafx.fxml.FXML;
import javafx.scene.control.Label;
import javafx.scene.control.TextArea;
import javafx.scene.control.TreeItem;
import javafx.scene.control.TreeView;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyCodeCombination;
import javafx.scene.input.KeyCombination;
import javafx.scene.input.KeyEvent;
import javafx.scene.paint.Color;
import javafx.util.Duration;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class ParametersController
{
    /** The controlling instance representing the connection to the robot. */
    private RobotControl control;
    
    @FXML private TreeView<String> params;
    @FXML private TextArea values;
    @FXML private Label scheme;
    @FXML private Label notice;
    
    private final KeyCombination saveValuesEnter = new KeyCodeCombination(KeyCode.ENTER);
    private final KeyCombination saveValuesCtrlS = new KeyCodeCombination(KeyCode.S, KeyCombination.CONTROL_DOWN);
    
    private final Command cmdScheme = new Command("Cognition:representation:print").addArg("PlayerInfo");
    private final Command cmdCognitionParams = new Command("Cognition:ParameterList:list");
    private final Command cmdMotionParams = new Command("Motion:ParameterList:list");
    
    private static final Pattern KEY_VALUE_PATTERN = Pattern.compile("^(?<key>.+)=(?<value>.*)$", Pattern.MULTILINE);
    private final ParameterResponseHandler responseHandler = new ParameterResponseHandler();
    
    private final FadeTransition fadeIn = new FadeTransition(Duration.millis(2000));
    
    /**
     * Default constructor for the FXML loader.
     */
    public ParametersController() {}
    
    /**
     * Constructor for custom initialization.
     */
    public ParametersController(RobotControl control) {
        setRobotControl(control);
    }
    
    /**
     * Gets called, after the FXML file was loaded.
     */
    @FXML
    private void initialize() 
    {
        params.setRoot(new TreeItem<>());
        params.getRoot().setExpanded(true);
        params.getSelectionModel().selectedItemProperty().addListener((o) -> { retrieveValues(); });
        // set some options to the fade animation
        fadeIn.setNode(notice);
        fadeIn.setFromValue(1.0);
        fadeIn.setToValue(0.0);
        fadeIn.setCycleCount(1);
        fadeIn.setAutoReverse(false);
    }
    
    /**
     * Sets the controlling instance of the robot.
     * @param control 
     */
    public void setRobotControl(RobotControl control) {
        this.control = control;
    }
    
    /**
     * Is called, when the update button is clicked.
     */
    @FXML
    private void fxUpdateParams() {
        if (control != null && control.checkConnected()) {
            control.getMessageServer().executeCommand(responseHandler, cmdScheme);
            control.getMessageServer().executeCommand(responseHandler, cmdCognitionParams);
            control.getMessageServer().executeCommand(responseHandler, cmdMotionParams);
        }
    }
    
    /**
     * Is called, when key event is triggered on the values textarea.
     * 
     * @param k the triggered key event
     */
    @FXML
    private void fxValuesShortcuts(KeyEvent k) {
        if (saveValuesEnter.match(k) || saveValuesCtrlS.match(k)) {
            TreeItem<String> selected = params.getSelectionModel().getSelectedItem();
            if (selected != null && control != null && control.checkConnected()) {
                // prepare the command
                Command cmd = new Command(selected.getParent().getValue() + ":ParameterList:set").addArg("<name>", selected.getValue());
                parseText().forEach((o, v) -> { cmd.addArg(o, v); });
                // ... and send
                control.getMessageServer().executeCommand(responseHandler, cmd);
                // disable until we get response
                values.setEditable(false);
                // make sure we're in sync with the robot
                retrieveValues();
            }
        }
    }
    
    /**
     * Updates the scheme, based on the command response.
     * 
     * @param response the response of the scheme command
     */
    private void updateScheme(String response) {
        // extract representation from string
        Map<String, String> repr = new HashMap<>();
        Matcher m = KEY_VALUE_PATTERN.matcher(response);
        while (m.find()) {
            repr.put(m.group("key").trim(), m.group("value").trim());
        }
        String s = repr.getOrDefault("active scheme", "?");
        Platform.runLater(() -> { scheme.setText(s.equals("-")?"default":s); });
    }
    
    /**
     * Updates the parameter list and re-selects a previously selected item.
     * 
     * @param items the new representations
     */
    private void updateList(String type, int idx, Collection<String> items) {
        Platform.runLater(() -> {
            // remember selected item
            TreeItem selectedItem = null;
            String selected = params.getSelectionModel().getSelectedItem() != null ? params.getSelectionModel().getSelectedItem().getValue() : "";
            
            // get the root item of the type or create a new, if it doesn't exists
            TreeItem root;
            if (params.getRoot().getChildren().size() > idx && params.getRoot().getChildren().get(idx).getValue().equals(type)) {
                root = params.getRoot().getChildren().get(idx);
            } else {
                root = new TreeItem(type);
                root.setExpanded(true);
                // inserts the tree node at the index position or at the end
                if(params.getRoot().getChildren().size() > idx) {
                    params.getRoot().getChildren().add(idx, root);
                } else {
                    params.getRoot().getChildren().add(root);
                }
            }
            root.getChildren().clear();

            // add the new items and remember the (previously) selected tree item
            for (String i : items) {
                TreeItem item = new TreeItem(i);
                if (i.equals(selected)) {
                    selectedItem = item;
                }
                root.getChildren().add(item);
            }
            
            // if we got a (previously) selected item, re-select it
            if (selectedItem != null) {
                params.getSelectionModel().select(selectedItem);
            }
        });
    }
    
    /**
     * Schedules the command for retrieving the parameters of a parameter group.
     */
    private void retrieveValues() {
        TreeItem<String> selected = params.getSelectionModel().getSelectedItem();
        if (selected != null && control != null && control.checkConnected()) {
            control.getMessageServer().executeCommand(
                    responseHandler, 
                    new Command(selected.getParent().getValue() + ":ParameterList:get").addArg("<name>", selected.getValue())
            );
        }
    }
    
    /**
     * Updates the parameters of a parameter group
     * 
     * @param text the retrieved paramenter group content
     */
    private void updateValues(String text) {
        int pos = values.getCaretPosition();
        values.setText(text);
        values.positionCaret(pos);
        values.setEditable(true);
        values.requestFocus();
    }
    
    /**
     * Handles the response of a parameter group save request
     * 
     * @param result the response text from the robot
     */
    private void handleParamSaved(String result) {
        if (result.contains("successfully")) {
            notice.setTextFill(Color.GREEN);
        } else {
            notice.setTextFill(Color.RED);
        }
        notice.setVisible(true);
        fadeIn.playFromStart();
        values.setEditable(true);
    }
    
    /**
     * Prepares the the parameter configuration for saving and/or sending to the nao.
     * @return the "prepared" parameter configuration
     */
    private Map<String,String> parseText() {
        TreeMap<String, String> result = new TreeMap();
        String text = values.getText();
        text = text.replaceAll("( |\t)+", "");
        String[] lines = text.split("(\n)+");
        for (String l : lines) {
            String[] splitted = l.split("=");
            if (splitted.length == 2) {
                String key = splitted[0].trim();
                String value = splitted[1].trim();
                // remove the last ;
                if (value.charAt(value.length() - 1) == ';') {
                    value = value.substring(0, value.length() - 1);
                }
                result.put(key, value);
            }
        }//end for
        return result;
    }
    
    /**
     * The response handler class for various commands.
     */
    class ParameterResponseHandler implements ResponseListener
    {
        @Override
        public void handleResponse(byte[] result, Command command) {
             if (command.equals(cmdScheme)) {
                updateScheme(new String(result));
            } else if (command.equals(cmdCognitionParams)) {
                updateList("Cognition", 0, Arrays.asList(new String(result).split("\n")));
            } else if (command.equals(cmdMotionParams)) {
                updateList("Motion", 1, Arrays.asList(new String(result).split("\n")));
            } else if (command.getName().endsWith(":ParameterList:get")) {
                updateValues(new String(result));
            } else if (command.getName().endsWith(":ParameterList:set")) {
                 handleParamSaved(new String(result));
            } else {
                System.err.println("Unknown command!");
            }
        }

        @Override
        public void handleError(int code) {
            // TODO: is there a better way to view errors?!
            values.clear();
            values.setPromptText("Got error response: " + code);
            values.setEditable(false);
        }
    }
}
