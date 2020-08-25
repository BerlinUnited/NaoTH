package de.naoth.rc.dialogsFx.parameters;

import de.naoth.rc.RobotControl;
import de.naoth.rc.componentsFx.AlertDialog;
import de.naoth.rc.core.server.Command;
import de.naoth.rc.core.server.ResponseListener;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Arrays;
import java.util.Collection;
import java.util.HashMap;
import java.util.Map;
import java.util.TreeMap;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import javafx.animation.FadeTransition;
import javafx.application.Platform;
import javafx.fxml.FXML;
import javafx.scene.control.ButtonType;
import javafx.scene.control.Label;
import javafx.scene.control.TextArea;
import javafx.scene.control.TreeItem;
import javafx.scene.control.TreeView;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyCodeCombination;
import javafx.scene.input.KeyCombination;
import javafx.scene.input.KeyEvent;
import javafx.scene.paint.Color;
import javafx.stage.DirectoryChooser;
import javafx.stage.FileChooser;
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
    
    /** Some key shortcut definitions */
    private final KeyCombination shortcutUpdate = new KeyCodeCombination(KeyCode.F5);
    private final KeyCombination shortcutExport = new KeyCodeCombination(KeyCode.E, KeyCombination.CONTROL_DOWN);
    private final KeyCombination shortcutSaveValueEnter = new KeyCodeCombination(KeyCode.ENTER);
    private final KeyCombination shortcutSaveValuesCtrlS = new KeyCodeCombination(KeyCode.S, KeyCombination.CONTROL_DOWN);
    
    private final Command cmdScheme = new Command("Cognition:representation:print").addArg("PlayerInfo");
    private final Command cmdCognitionParams = new Command("Cognition:ParameterList:list");
    private final Command cmdMotionParams = new Command("Motion:ParameterList:list");
    
    private static final Pattern KEY_VALUE_PATTERN = Pattern.compile("^(?<key>.+)=(?<value>.*)$", Pattern.MULTILINE);
    private final ParameterResponseHandler responseHandler = new ParameterResponseHandler();
    
    private final FadeTransition fadeOut = new FadeTransition(Duration.millis(2000));
    
    final private String parameterSavePathKey = "parameter_save_path";
    final private String defaultConfigPath = "../NaoTHSoccer/Config/scheme";
    
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
        // set the options for the fade-out animation
        fadeOut.setNode(notice);
        fadeOut.setFromValue(1.0);
        fadeOut.setToValue(0.0);
        fadeOut.setCycleCount(1);
        fadeOut.setAutoReverse(false);
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
        if (shortcutSaveValueEnter.match(k) || shortcutSaveValuesCtrlS.match(k)) {
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
        } else if (shortcutUpdate.match(k)) {
            // F5 is consumed by the textarea and does not bubble to the parent node,
            // so we must handle it here
            fxUpdateParams();
        }
    }
    
    /**
     * Is called, when key event is triggered.
     * The parameter update and the export is handled.
     * 
     * @param k the triggered key event
     */
    @FXML
    private void fxDialogShortcuts(KeyEvent k) {
        if (shortcutUpdate.match(k)) {
            fxUpdateParams();
        } else if (shortcutExport.match(k)) {
            fxExport();
        }
    }
    
    /**
     * Is called, when the export button is pressed.
     */
    @FXML
    private void fxExport() {
        TreeItem<String> selected = params.getSelectionModel().getSelectedItem();
        // check if we got a valid selection; ignore Cognition/Motion entries, which doesn't have a "valid" parent
        if (selected == null || selected.getParent().getValue() == null) {
            AlertDialog.showError("Error", "You have to choose a parameter configuration!");
        } else {
            String parameterPath = control.getConfig().getProperty(parameterSavePathKey, defaultConfigPath);
            // configure the filechooser ...
            FileChooser fileChooser = new FileChooser();
            fileChooser.setTitle("Save configuration ("+selected.getValue()+")");
            fileChooser.getExtensionFilters().addAll(new FileChooser.ExtensionFilter("Config files (*.cfg)", "*.cfg"));
            fileChooser.setInitialDirectory(new File(parameterPath));
            fileChooser.setInitialFileName(selected.getValue() + ".cfg");
            
            File selectedFile = fileChooser.showSaveDialog(params.getScene().getWindow());
            if(selectedFile != null) {
                // if selected file has a file extension - use this, otherwise append ".cfg" to the filename
                File f = (selectedFile.getName().lastIndexOf(".") == -1) ? new File(selectedFile+".cfg") : selectedFile;

                try {
                    new FileWriter(f).close(); // trigger exception (if couldn't write)

                    // create file and write parameter configuration to this file
                    writeParameterConfig(selected.getValue(), f);
                } catch (IOException ex) {
                    Logger.getLogger(ParametersController.class.getName()).log(Level.SEVERE, null, ex);
                    AlertDialog.showError("Not writeable", "Selected file is not writeable!");
                }
                
                // save the path for later
                control.getConfig().setProperty(parameterSavePathKey, selectedFile.getParent());
            }
        }
    }
    
    /**
     * Is called, when the export all menu item is pressed.
     */
    @FXML
    private void fxExportAll() {
        // make sure, we're connected!
        if (control != null && control.checkConnected()) {
            // update ui after connecting ...
            fxUpdateParams();
            // configure filechooser ...
            DirectoryChooser directoryChooser = new DirectoryChooser();
            directoryChooser.setTitle("Save configuration (All)");
            // show save dialog
            File directory = directoryChooser.showDialog(params.getScene().getWindow());
            if(directory != null)
            {
                if(AlertDialog.showConfirmation("Overwrite Files?", "Any existing file will be overwritten!\nProceed?") == ButtonType.OK)
                {
                    try {
                        // trigger exception (if couldn't write)
                        File.createTempFile("tmp_"+Math.random(), ".tmp", directory).delete();
                        
                        // iterate over all listed parameter configurations
                        params.getRoot().getChildren().forEach((p) -> {
                            p.getChildren().forEach((i) -> {
                                ParameterResponseWriter h = new ParameterResponseWriter(
                                        i.getValue(),
                                        new File(directory.getPath()+File.separator+p.getValue()+"_"+i.getValue()+".cfg")
                                );
                                retrieveValues(p.getValue(), i.getValue(), h);
                            });
                        });
                        
                        AlertDialog.showInfo("All parameters saved", "All parameters should be saved in the directory:\n" + directory.getAbsolutePath());
                    } catch (IOException ex) {
                        Logger.getLogger(ParametersController.class.getName()).log(Level.SEVERE, null, ex);
                        AlertDialog.showError("Not writeable", "Selected directory is not writeable!");
                    }
                }
            }
        }
    }
    
    /**
     * Writes the given parameter configuration to the specified file.
     * @param p the parameter configuration
     * @param f the output file
     * @return true, if writing was successfull
     */
    private boolean writeParameterConfig(String name, File f) {
        try {
            BufferedWriter bf = new BufferedWriter(new FileWriter(f));
            bf.write("["+name+"]\n");
            for (Map.Entry<String, String> e : parseText().entrySet()) {
                bf.write(e.getKey() + "=" + e.getValue() + "\n");
            }
            bf.close();
            return true;
        } catch (IOException ex) {
            Logger.getLogger(ParametersController.class.getName()).log(Level.SEVERE, null, ex);
        }
        return false;
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
     * Checks, if a parameter group is selected and schedules a command.
     */
    private void retrieveValues() {
        TreeItem<String> selected = params.getSelectionModel().getSelectedItem();
        // check if we got a valid selection; ignore Cognition/Motion entries, which doesn't have a "valid" parent
        if (selected != null && selected.getParent().getValue() != null) {
            retrieveValues(selected.getParent().getValue(), selected.getValue(), responseHandler);
        }
    }
    
    /**
     * Schedules the command for retrieving the parameters of a parameter group.
     */
    private void retrieveValues(String type, String name, ResponseListener handler) {
        if (control != null && control.checkConnected()) {
            control.getMessageServer().executeCommand(
                    handler, 
                    new Command(type + ":ParameterList:get").addArg("<name>", name)
            );
        }
    }
    
    /**
     * Updates the parameters of a parameter group
     * 
     * @param text the retrieved paramenter group content
     */
    private void updateValues(String text) {
        Platform.runLater(() -> {
            int pos = values.getCaretPosition();
            values.setText(text);
            values.positionCaret(pos);
            values.setEditable(true);
            values.requestFocus();
        });
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
        fadeOut.playFromStart();
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
    
    /**
     * The response handler class for writing the parameter values to the given file.
     */
    class ParameterResponseWriter implements ResponseListener {

        private final String param;
        private final File f;

        public ParameterResponseWriter(String param, File f) {
            this.param = param;
            this.f = f;
        }

        @Override
        public void handleResponse(byte[] result, Command command) {
            try {
                BufferedWriter bf = new BufferedWriter(new FileWriter(f));
                bf.write("[" + param + "]\n");
                bf.write(new String(result));
                bf.close();
            } catch (IOException ex) {
                Logger.getLogger(ParametersController.class.getName()).log(Level.SEVERE, null, ex);
            }
        }

        @Override
        public void handleError(int code) {
            Logger.getLogger(ParametersController.class.getName()).log(Level.SEVERE, null, code);
        }
    }
}
