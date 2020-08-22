package de.naoth.rc.dialogsFx.moduleconfiguration;

import com.google.protobuf.InvalidProtocolBufferException;
import de.naoth.rc.RobotControl;
import de.naoth.rc.core.messages.Messages;
import de.naoth.rc.core.server.Command;
import de.naoth.rc.core.server.ResponseListener;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.stream.Collectors;
import javafx.application.Platform;
import javafx.beans.property.ListProperty;
import javafx.beans.property.SimpleListProperty;
import javafx.collections.FXCollections;
import javafx.fxml.FXML;
import javafx.scene.control.Button;
import javafx.scene.control.ComboBox;
import javafx.scene.control.ListView;
import javafx.scene.control.TreeView;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class ModuleConfigurationController implements ResponseListener
{
    /** The controlling instance representing the connection to the robot. */
    private RobotControl control;
    
    private final Command cmd_modules_cognition = new Command("Cognition:modules:list");
    private final Command cmd_modules_cognition_store = new Command("Cognition:modules:store");
    private final Command cmd_modules_motion = new Command("Motion:modules:list");
    private final Command cmd_modules_motion_store = new Command("Motion:modules:store");
    
    /** The ui tree of the available debug requests */
    @FXML private Button btnSave;
    @FXML private Button btnExport;
    @FXML private TreeView<String> moduleTree;
    @FXML private ListView<String> infoList;
    @FXML private ComboBox<Module> module;
    @FXML private ComboBox<Representation> representation;
    
    private final ListProperty<Module> moduleList = new SimpleListProperty<>(FXCollections.observableArrayList());
    private final ListProperty<Representation> representationList = new SimpleListProperty<>(FXCollections.observableArrayList());
    
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
        
        module.setItems(moduleList.sorted());
        module.disableProperty().bind(moduleList.emptyProperty());
        representation.setItems(representationList.sorted());
        representation.disableProperty().bind(representationList.emptyProperty());
    }
    
    /**
     * Sets the controlling instance of the robot.
     * @param control 
     */
    public void setRobotControl(RobotControl control) {
        this.control = control;
    }

    /**
     * Handles the update button click.
     */
    @FXML
    private void updateModules() {
        if (control != null && control.checkConnected()) {
            moduleList.clear();
            representationList.clear();
            
            control.getMessageServer().executeCommand(this, cmd_modules_cognition);
            control.getMessageServer().executeCommand(this, cmd_modules_motion);
        }
    }

    @FXML
    public void saveModules() {
        if (control != null && control.checkConnected()) {
            control.getMessageServer().executeCommand(this, cmd_modules_cognition_store);
            control.getMessageServer().executeCommand(this, cmd_modules_motion_store);
        }
    }
    
    @Override
    public void handleResponse(byte[] result, Command command) {
        Platform.runLater(() -> {
            if(command.equals(cmd_modules_cognition)) {
                handleModuleResponse("Cognition", result);
            } else if(command.equals(cmd_modules_motion)) {
                handleModuleResponse("Motion", result);
            } else {
                Logger.getLogger(getClass().getName()).log(Level.SEVERE, "Unknown command response: {0}", command);
            }
        });
    }

    @Override
    public void handleError(int code) {
    }
    
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
            Logger.getLogger(ModuleConfigurationController.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
    
    class Module implements Comparable<Module>
    {
        private final String type;
        private final String name;
        private final String path;
        
        private boolean active = false;
        
        private final List<Representation> require = new ArrayList<>();
        private final List<Representation> provide = new ArrayList<>();

        public Module(String type, String name, String path, boolean active) {
            this.type = type;
            this.name = name;
            this.path = path;
            this.active = active;
        }

        @Override
        public String toString() {
            return name + " [" + type + "]";
        }
        
        public void addRequire(Representation r) {
            require.add(r);
        }
        
        public void addProvide(Representation r) {
            provide.add(r);
        }

        @Override
        public int compareTo(Module o) {
            return name.compareToIgnoreCase(o.name);
        }

        public String getName() {
            return name;
        }
    }
    
    class Representation implements Comparable<Representation>
    {
        private final String name;

        private final List<Module> required = new ArrayList<>();
        private final List<Module> provided = new ArrayList<>();
        
        public Representation(String name) {
            this.name = name;
        }

        @Override
        public String toString() {
            return name;
        }
        
        public void addProvider(Module m) {
            provided.add(m);
        }
        
        public void addRequirer(Module m) {
            required.add(m);
        }

        @Override
        public int compareTo(Representation o) {
            return name.compareToIgnoreCase(o.name);
        }
        
        public String getName() {
            return name;
        }
    }
}
