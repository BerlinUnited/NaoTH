package de.naoth.rc.dialogsFx.moduleconfiguration;

import com.google.protobuf.InvalidProtocolBufferException;
import de.naoth.rc.RobotControl;
import de.naoth.rc.core.messages.Messages;
import de.naoth.rc.core.server.Command;
import de.naoth.rc.core.server.ResponseListener;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.stream.Collectors;
import javafx.application.Platform;
import javafx.beans.property.ListProperty;
import javafx.beans.property.ObjectProperty;
import javafx.beans.property.SimpleListProperty;
import javafx.beans.property.SimpleObjectProperty;
import javafx.collections.FXCollections;
import javafx.collections.transformation.FilteredList;
import javafx.fxml.FXML;
import javafx.scene.control.Button;
import javafx.scene.control.ComboBox;
import javafx.scene.control.ListCell;
import javafx.scene.control.ListView;
import javafx.scene.control.ToggleButton;
import javafx.scene.control.TreeView;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyCodeCombination;
import javafx.scene.input.KeyCombination;
import javafx.scene.input.KeyEvent;
import javafx.scene.layout.Priority;
import javafx.scene.layout.VBox;
import javafx.stage.FileChooser;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class ModuleConfigurationController implements ResponseListener
{
    /** The controlling instance representing the connection to the robot. */
    private RobotControl control;
    
    /** The commands used in this dialog */
    private final Command cmd_modules_cognition = new Command("Cognition:modules:list");
    private final Command cmd_modules_cognition_store = new Command("Cognition:modules:store");
    private final Command cmd_modules_motion = new Command("Motion:modules:list");
    private final Command cmd_modules_motion_store = new Command("Motion:modules:store");
    
    /** The ui tree of the available debug requests */
    @FXML private Button btnSave;
    @FXML private Button btnExport;
    @FXML private ToggleButton btnErrors;
    @FXML private ToggleButton btnWarnings;
    @FXML private ToggleButton btnInfos;
    @FXML private TreeView<String> moduleTree;
    @FXML private ListView<Representation> infoList;
    @FXML private ComboBox<Module> module;
    @FXML private ComboBox<Representation> representation;
    @FXML private VBox dependencySelection;
    @FXML private VBox dependencyRequire;
    @FXML private VBox dependencyProvide;
    
    /** Shortcuts definitions */
    private final KeyCombination shortcutSave = new KeyCodeCombination(KeyCode.S, KeyCombination.CONTROL_DOWN);
    private final KeyCombination shortcutExport = new KeyCodeCombination(KeyCode.E, KeyCombination.CONTROL_DOWN);
    private final KeyCombination shortcutUpdate = new KeyCodeCombination(KeyCode.F5);
    
    /** The module/representation configuration and their dependencies */
    private final ListProperty<Module> moduleList = new SimpleListProperty<>(FXCollections.observableArrayList());
    private final ListProperty<Representation> representationList = new SimpleListProperty<>(FXCollections.observableArrayList());
    
    /** The currently selected module/representation, which dependency graph is shown */
    private final ObjectProperty<Type> selection = new SimpleObjectProperty<>();
    
    static final private String CONFIG_PATH_PARAMS = "parameter_save_path";
    static final private String CONFIG_PATH_DEFAULT = "../NaoTHSoccer/Config/scheme";
    
    /**
     * Default constructor for the FXML loader.
     */
    public ModuleConfigurationController() {}

    /**
     * Constructor for custom initialization.
     */
    public ModuleConfigurationController(RobotControl control) {
        setRobotControl(control);
    }
    
    /**
     * Gets called, after the FXML file was loaded.
     */
    @FXML
    private void initialize() 
    {
        // setup ui
        /*
        // TODO: need the TreeNode implementation from the DebugRequest branch
        moduleTree.setCellFactory((p) -> new TreeNodeCell<>());
        moduleTree.setRoot(new TreeNodeItem<>());
        moduleTree.getRoot().setExpanded(true);
        */
        
        btnSave.disableProperty().bind(moduleList.emptyProperty());
        btnExport.disableProperty().bind(moduleList.emptyProperty());
        
        // TODO: add autocompletion to the combobox
        module.setItems(moduleList.sorted());
        module.disableProperty().bind(moduleList.emptyProperty());
        representation.setItems(representationList.sorted());
        representation.disableProperty().bind(representationList.emptyProperty());
        
        // handle new selection
        selection.addListener((ob) -> { updateDependencyPanel(); });
        
        // update the selection property based on the combobox selection
        module.getSelectionModel().selectedItemProperty().addListener((ob,o,n) -> { selection.set(n); });
        representation.getSelectionModel().selectedItemProperty().addListener((ob,o,n) -> { selection.set(n); });
        
        // show only a filtered & sorted version of the representations
        FilteredList<Representation> filteredRepresentationList = new FilteredList<>(representationList, (r) -> showRepresentationInfo(r));
        infoList.setItems(filteredRepresentationList.sorted((r1, r2) -> {
            // sort by representation info first
            return r1.getInfo() == r2.getInfo() ? r1.compareTo(r2) : r2.getInfo().compareTo(r1.getInfo());
        }));
        infoList.setCellFactory((f) -> new RepresentationListCell());
        infoList.getSelectionModel().selectedItemProperty().addListener((ob, o, n) -> { selection.set(n); });
        // HACK: force re-evaluation of the filtered list
        btnErrors.setOnAction((e) -> { filteredRepresentationList.setPredicate((r) -> showRepresentationInfo(r)); });
        btnWarnings.setOnAction((e) -> { filteredRepresentationList.setPredicate((r) -> showRepresentationInfo(r)); });
        btnInfos.setOnAction((e) -> { filteredRepresentationList.setPredicate((r) -> showRepresentationInfo(r)); });
    }
    
    /**
     * Sets the controlling instance of the robot.
     * @param control 
     */
    public void setRobotControl(RobotControl control) {
        this.control = control;
    }

    /**
     * Handles the update button click and sends the retrieving commands.
     */
    @FXML
    private void fxUpdateModules() {
        if (control != null && control.checkConnected()) {
            moduleList.clear();
            representationList.clear();
            
            control.getMessageServer().executeCommand(this, cmd_modules_cognition);
            control.getMessageServer().executeCommand(this, cmd_modules_motion);
        }
    }

    /**
     * Handles the save button click and sends the saving commands.
     */
    @FXML
    public void fxSaveModules() {
        if (control != null && control.checkConnected()) {
            control.getMessageServer().executeCommand(this, cmd_modules_cognition_store);
            control.getMessageServer().executeCommand(this, cmd_modules_motion_store);
        }
    }
    
    /**
     * Handles the export button click and exports the module configuration to a file.
     */
    @FXML
    public void fxExportModules() {
        String parameterPath = control.getConfig().getProperty(CONFIG_PATH_PARAMS, CONFIG_PATH_DEFAULT);
        // configure the filechooser ...
        FileChooser fileChooser = new FileChooser();
        fileChooser.setTitle("Save module configuration");
        fileChooser.getExtensionFilters().addAll(new FileChooser.ExtensionFilter("Config files (*.cfg)", "*.cfg"));
        fileChooser.setInitialDirectory(new File(parameterPath));
        fileChooser.setInitialFileName("modules.cfg");

        File selectedFile = fileChooser.showSaveDialog(btnExport.getScene().getWindow());
        if(selectedFile != null) {
            // if selected file has a file extension - use this, otherwise append ".cfg" to the filename
            File f = (selectedFile.getName().lastIndexOf(".") == -1) ? new File(selectedFile+".cfg") : selectedFile;

            // create file and write module configuration to this file
            try(BufferedWriter bf = new BufferedWriter(new FileWriter(f))) {
                bf.write("[modules]\n");
                for (Module m : moduleList) {
                    bf.write(m.getName() + "=" + (m.isActive() ? "true" : "false") + "\n");
                }
                bf.close();
            } catch (IOException ex) {
                Logger.getLogger(getClass().getName()).log(Level.SEVERE, null, ex);
                // TODO: show alert dialog
            }

            // save the path for later
            control.getConfig().setProperty(CONFIG_PATH_PARAMS, selectedFile.getParent());
        }
    }
    
    /**
     * Is called, when key event is triggered.
     * 
     * @param k the triggered key event
     */
    @FXML
    private void fxDialogShortcuts(KeyEvent k) {
        if (shortcutSave.match(k) && !btnSave.isDisabled()) {
            fxSaveModules();
        } else if(shortcutExport.match(k) && !btnExport.isDisabled()) {
            fxExportModules();
        } else if (shortcutUpdate.match(k)) {
            fxUpdateModules();
        }
    }
    
    /**
     * Handles the response of various commands.
     * 
     * @param result    the response data
     * @param command   the used command of the response
     */
    @Override
    public void handleResponse(byte[] result, Command command) {
        Platform.runLater(() -> {
            if(command.equals(cmd_modules_cognition)) {
                handleModuleResponse("Cognition", result);
            } else if(command.equals(cmd_modules_motion)) {
                handleModuleResponse("Motion", result);
            } else if (command.equals(cmd_modules_cognition_store) || command.equals(cmd_modules_motion_store)) {
                // TODO: show AlertDialog with error/info!
            } else {
                Logger.getLogger(getClass().getName()).log(Level.SEVERE, "Unknown command response: {0}", command);
            }
        });
    }

    /**
     * Logs the error of a failed command.
     * 
     * @param code the error code
     */
    @Override
    public void handleError(int code) {
        Logger.getLogger(getClass().getName()).log(Level.SEVERE, null, code);
    }
    
    /**
     * Handles the response of the retrieving module command.
     * It parses the response and builds the dependencies between modules and representations.
     * After processing the data, the dependency lists are updated.
     * 
     * @param type   the response type (cognition/motion)
     * @param result the response data
     */
    private void handleModuleResponse(String type, byte[] result) {
        try {
            // temp. reference LUT
            final Map<String, Module> ml = new HashMap<>();
            final Map<String, Representation> rl = representationList.stream().collect(Collectors.toMap(Representation::getName, rep -> rep));
            
            Messages.ModuleList list = Messages.ModuleList.parseFrom(result);
            list.getModulesList().forEach((m) -> {
                String[] str = m.getName().split("\\|");
                if (str.length == 2) {
                    Module mod = new Module(type, str[0], str[1], m.getActive());
                    // process the provided representations
                    m.getProvidedRepresentationsList().forEach((r) -> {
                        // add representation to LUT
                        if (!rl.containsKey(r)) {
                            rl.put(r, new Representation(r));
                        }
                        // set the providing info to the module and representation
                        Representation rep = rl.get(r);
                        mod.addProvide(rep);
                        rep.addProvider(mod);
                    });
                    // process the required representations
                    m.getUsedRepresentationsList().forEach((r) -> {
                        // add representation to LUT
                        if (!rl.containsKey(r)) {
                            rl.put(r, new Representation(r));
                        }
                        // set the requiring info to the module and representation
                        Representation rep = rl.get(r);
                        mod.addRequire(rep);
                        rep.addRequirer(mod);
                    });
                    ml.put(mod.name, mod);
                } else {
                    System.err.println("Invalid module string: " + m.getName() + " [" + type + "]");
                }
            });
            
            // finally update the list
            moduleList.addAll(ml.values());
            representationList.clear(); // TODO: is there a better way to prevent adding the same rep. twice?
            representationList.addAll(rl.values());
            
            // TODO:
            //moduleTree.getRoot().getChildren().add(Utils.createModulesCognition(list, cognitionModuleRequest));
        } catch (InvalidProtocolBufferException ex) {
            Logger.getLogger(getClass().getName()).log(Level.SEVERE, null, ex);
        }
    }
    
    /**
     * Updates the dependency view based on the current selection (module/representation).
     */
    private void updateDependencyPanel() {
        if (selection.get() != null) {
            // clear dependency graph first
            dependencySelection.getChildren().clear();
            dependencyRequire.getChildren().clear();
            dependencyProvide.getChildren().clear();
            // set the button for the current selection
            dependencySelection.getChildren().add(createNodeButton(selection.get()));
            // with representation, the dependency is reversed!: provide <> require ; default require <> provide
            VBox dependencyLeft = selection.get() instanceof Representation ? dependencyProvide : dependencyRequire;
            VBox dependencyRight = selection.get() instanceof Representation ? dependencyRequire : dependencyProvide;
            dependencyLeft.getChildren().addAll(selection.get().getRequire().stream().map((r) -> { return createNodeButton(r); }).collect(Collectors.toList()));
            dependencyRight.getChildren().addAll(selection.get().getProvide().stream().map((r) -> { return createNodeButton(r); }).collect(Collectors.toList()));

            // clear the module selection, if another module/representation is selected
            if (module.getValue() != selection.get()) {
                module.getSelectionModel().clearSelection();
            }
            // clear the representation selection, if another module/representation is selected
            if (representation.getValue() != selection.get()) {
                representation.getSelectionModel().clearSelection();
            }
            // clear the representation selection of the representation info list, if another module/representation is selected
            if (infoList.getSelectionModel().getSelectedItem() != selection.get()) {
                infoList.getSelectionModel().clearSelection();
            }
        }
    }
    
    /**
     * Creates a button for the dependency view.
     * Based on the given type (module/representation) the button is styled slightly different.
     * 
     * @param n the type, for which the button should be generated for (module/representation)
     * @return the generated button
     */
    private Button createNodeButton(Type n) {
        Button b = new Button(n.getName());
        b.setMaxWidth(Double.MAX_VALUE);
        b.setMaxHeight(Double.MAX_VALUE);
        b.setOnAction((a) -> { selection.set(n); });
        VBox.setVgrow(b, Priority.ALWAYS);
        // set the color based on the node type
        if (n instanceof Module) {
            b.getStyleClass().add("module");
            // mark disabled modules
            if (!((Module) n).isActive()) {
                b.getStyleClass().add("inactive");
            }
        } else if (n instanceof Representation) {
            b.getStyleClass().add("representation");
        }
        
        return b;
    }
    
    /**
     * The filter function to filter the representation info view based on the 
     * current active configuration (info state of the representation and selected
     * buttons [info, warning, error])
     * 
     * @param r the representation to check
     * @return true, if the representation should be shown, false otherwise
     */
    private boolean showRepresentationInfo(Representation r) {
        return r.getInfo() == Representation.Info.ERROR && btnErrors.isSelected()
            || r.getInfo() == Representation.Info.WARNING && btnWarnings.isSelected()
            || r.getInfo() == Representation.Info.INFO && btnInfos.isSelected();
    }
    
    /**
     * Base class for module/representation
     */
    abstract static class Type {
        /**
         * Return the name of this type.
         * 
         * @return name of the type
         */
        abstract public String getName();
        
        /**
         * Returns the required types by this type.
         * 
         * @return list of requirements
         */
        abstract public List<Type> getRequire();
        
        /**
         * Returns the provided types by this type.
         * 
         * 
         * @return list of providing types
         */
        abstract public List<Type> getProvide();
    }
    
    /**
     * The module type class.
     * Represents a module with its dependencies (representations).
     */
    static class Module extends Type implements Comparable<Module>
    {
        /** The type of moulde (cognition/motion) */
        private final String type;
        /** The modules name */
        private final String name;
        /** The path, where the module is defined in C++ */
        private final String path;
        /** The module state flag: enabled or disabled */
        private boolean active = false;
        /** The requiring representations */
        private final List<Type> require = new ArrayList<>();
        /** The providing representations */
        private final List<Type> provide = new ArrayList<>();

        /**
         * Constructor - initialise the module.
         * 
         * @param type      module type (cognition/motion)
         * @param name      module name
         * @param path      module path (C++)
         * @param active    enabled/disabled flag
         */
        public Module(String type, String name, String path, boolean active) {
            this.type = type;
            this.name = name;
            this.path = path;
            this.active = active;
        }

        /**
         * Returns the name and type of this module
         * 
         * @return name and type in the form "name [type]"
         */
        @Override
        public String toString() {
            return name + " [" + type + "]";
        }
        
        /**
         * Adds the representation as required to this module.
         * 
         * @param r the required representation
         */
        public void addRequire(Representation r) {
            require.add(r);
        }
        
        /**
         * Adds the representation as provided to this module.
         * 
         * @param r the providing representation
         */
        public void addProvide(Representation r) {
            provide.add(r);
        }

        /**
         * Compares this module with another based on their names (case insensitive).
         * Used to sort the list of modules.
         * 
         * @param o the other module
         * @return  result of the comparison: -1, 0, 1
         */
        @Override
        public int compareTo(Module o) {
            return name.compareToIgnoreCase(o.name);
        }

        /**
         * Returns the name of this module.
         * 
         * @return modules name
         */
        @Override
        public String getName() {
            return name;
        }

        /**
         * Returns the list of required types (representations)
         * 
         * @return required representations
         */
        @Override
        public List<Type> getRequire() {
            return require;
        }

        /**
         * Returns the list of provided types (representations)
         * 
         * @return provided representations
         */
        @Override
        public List<Type> getProvide() {
            return provide;
        }
        
        /**
         * Returns whether this module is enabled or not
         * 
         * @return true, if enabled, false otherwise
         */
        public boolean isActive() {
            return active;
        }
    }
    
    /**
     * The representation type class.
     * Represents a representation with its dependencies (modules).
     */
    static class Representation extends Type implements Comparable<Representation>
    {
        /** The possible representation "info state" based on the module configuration (see getInfo()) */
        public static enum Info {
            EMPTY,   // not provided and not required
            INFO,    //     provided and     required
            WARNING, //     provided but not required
            ERROR    // not provided but     required
        }

        /** Name of the representation */
        private final String name;
        
        /** The requiring modules */
        private final List<Type> required = new ArrayList<>();
        /** The providing modules */
        private final List<Type> provided = new ArrayList<>();
        
        /**
         * Constructor.
         * Creates the representation with the given name.
         * 
         * @param name of the representation
         */
        public Representation(String name) {
            this.name = name;
        }

        /**
         * Returns just the name of this representation
         * 
         * @return name of the representation
         */
        @Override
        public String toString() {
            return name;
        }
        
        /**
         * Adds the providing module to this representation.
         * 
         * @param m the providing module
         */
        public void addProvider(Module m) {
            provided.add(m);
        }
        
        /**
         * Adds the module which requires this representation.
         * 
         * @param m the module requiring this representation
         */
        public void addRequirer(Module m) {
            required.add(m);
        }

        /**
         * Compares this representation with another based on their names (case insensitive).
         * Used to sort the list of representations.
         * 
         * @param o the other module
         * @return  result of the comparison: -1, 0, 1
         */
        @Override
        public int compareTo(Representation o) {
            return name.compareToIgnoreCase(o.name);
        }
        
        /**
         * Returns the name of this representation.
         * 
         * @return the representations name
         */
        @Override
        public String getName() {
            return name;
        }

        /**
         * Returns the list of modules requiring this representation.
         * 
         * @return module list
         */
        @Override
        public List<Type> getRequire() {
            return required;
        }

        /**
         * Returns the list of modules providing this representation.
         * 
         * @return module list
         */
        @Override
        public List<Type> getProvide() {
            return provided;
        }
        
        /**
         * Returns the "info state" of the representation.
         * The "info state" determines, whether the representation is used 
         * correctly with the current active module configuration:
         * If the representation is
         * - neither provided nor required: EMPTY state
         * - provided but not required: WARNING state
         * - not provided but required: ERROR state
         * - provided and required: INFO state
         * 
         * @return the current state
         */
        public Info getInfo() {
            // check if there's an active providing/requiring module
            boolean isProvided = provided.stream().anyMatch((m) -> ((Module)m).isActive());
            boolean isRequired = required.stream().anyMatch((m) -> ((Module)m).isActive());
            
            if (provided.isEmpty() && required.isEmpty()) {
                return Info.EMPTY;   // not provided and not required
            } else if(isProvided && !isRequired) {
                return Info.WARNING; //     provided but not required
            } else if(!isProvided && isRequired) {
                return Info.ERROR;   // not provided but     required
            }
            
            return Info.INFO;        //     provided and     required
        }
    }
    
    /**
     * Class handling, how the representation is shown in the list view based on
     * the "info state" of the representation.
     */
    public class RepresentationListCell extends ListCell<Representation>
    {
        @Override
        protected void updateItem(Representation item, boolean empty) {
            super.updateItem(item, empty);
            
            if (item == null || empty) {
                setGraphic(null);
                setText(null);
            } else {
                switch (item.getInfo()) {
                    case INFO:
                        setText("NOTE: " + item + " is provided and required");
                        getStyleClass().add("info");
                        getStyleClass().removeAll("warning", "error");
                        break;
                    case WARNING:
                        setText("WARNING: " + item + " is provided but not required (" + item.provided.size() + ")");
                        getStyleClass().add("warning");
                        getStyleClass().removeAll("info", "error");
                        break;
                    case ERROR:
                        setText("ERROR: " + item + " is required but not provided (" + item.required.size() + ")");
                        getStyleClass().add("error");
                        getStyleClass().removeAll("info", "warning");
                        break;
                    case EMPTY:
                        // NOTE: should never happen: not provided and not required
                    default:
                        setText(null);
                        setGraphic(null);
                        break;
                }
            }
        }
    }
}
