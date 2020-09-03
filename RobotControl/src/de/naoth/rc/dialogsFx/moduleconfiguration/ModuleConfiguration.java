package de.naoth.rc.dialogsFx.moduleconfiguration;

import com.google.protobuf.InvalidProtocolBufferException;
import de.naoth.rc.RobotControl;
import de.naoth.rc.core.messages.Messages;
import de.naoth.rc.core.server.Command;
import de.naoth.rc.core.server.ResponseListener;
import java.util.HashMap;
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

/**
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class ModuleConfiguration implements ResponseListener
{
    /** The controlling instance representing the connection to the robot. */
    private RobotControl control;
    
    /** The commands used in this dialog */
    private final Command cmd_modules_cognition = new Command("Cognition:modules:list");
    private final Command cmd_modules_cognition_store = new Command("Cognition:modules:store");
    private final Command cmd_modules_motion = new Command("Motion:modules:list");
    private final Command cmd_modules_motion_store = new Command("Motion:modules:store");
    
    /** The module/representation configuration and their dependencies */
    private final ListProperty<Module> moduleList = new SimpleListProperty<>(FXCollections.observableArrayList());
    private final ListProperty<Representation> representationList = new SimpleListProperty<>(FXCollections.observableArrayList());
    
    /** The currently active module/representation, which dependency graph is shown */
    private final ObjectProperty<Type> active = new SimpleObjectProperty<>();

    /**
     * Default constructor for the FXML loader.
     */
    public ModuleConfiguration() {}

    /**
     * Constructor for custom initialization.
     * @param control
     */
    public ModuleConfiguration(RobotControl control) {
        this.control = control;
    }
    
    /**
     * Sets the controlling instance of the robot.
     * @param control 
     */
    public void setRobotControl(RobotControl control) {
        this.control = control;
    }
    
    /**
     * Returns the modules list property.
     * 
     * @return modules list property
     */
    public ListProperty<Module> getModulesProperty() {
        return moduleList;
    }

    /**
     * Returns the representations list property.
     * 
     * @return representation list property
     */
    public ListProperty<Representation> getRepresentationsProperty() {
        return representationList;
    }

    /**
     * Returns the currently active type object property.
     * 
     * @return current active type property
     */
    public ObjectProperty<Type> getActiveProperty() {
        return active;
    }

    /**
     * Updates the modules and representation list.
     */
    public void updateModules() {
        if (control != null && control.checkConnected()) {
            moduleList.clear();
            representationList.clear();
            
            control.getMessageServer().executeCommand(this, cmd_modules_cognition);
            control.getMessageServer().executeCommand(this, cmd_modules_motion);
        }
    }
    
    /**
     * Saves the modules on the robot.
     */
    public void saveModules() {
        if (control != null && control.checkConnected()) {
            control.getMessageServer().executeCommand(this, cmd_modules_cognition_store);
            control.getMessageServer().executeCommand(this, cmd_modules_motion_store);
        }
    }
    
    /**
     * Enables or disables the given module on the robot.
     * 
     * @param module    the module to enable/disable
     * @param enable    true, if the module should be enabled, false otherwise
     */
    public void enableModule(Module module, boolean enable) {
        if (control != null && control.checkConnected()) {
            control.getMessageServer().executeCommand(this, new Command(module.getType() + ":modules:set").addArg(module.getName(), enable ? "on" : "off"));
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
        if(command.equals(cmd_modules_cognition)) {
            Platform.runLater(() -> { handleModuleResponse("Cognition", result); });
        } else if(command.equals(cmd_modules_motion)) {
            Platform.runLater(() -> { handleModuleResponse("Motion", result); });
        } else if (command.equals(cmd_modules_cognition_store) || command.equals(cmd_modules_motion_store)) {
            // TODO: show AlertDialog with error/info!
        } else if (command.getName().endsWith("modules:set")) {
            // TODO: show AlertDialog with error/info!
        } else {
            Logger.getLogger(getClass().getName()).log(Level.SEVERE, "Unknown command response: {0}", command);
        }
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
                    ml.put(mod.getName(), mod);
                } else {
                    System.err.println("Invalid module string: " + m.getName() + " [" + type + "]");
                }
            });
            
            // finally update the list
            moduleList.addAll(ml.values());
            representationList.clear(); // TODO: is there a better way to prevent adding the same rep. twice?
            representationList.addAll(rl.values());
        } catch (InvalidProtocolBufferException ex) {
            Logger.getLogger(getClass().getName()).log(Level.SEVERE, null, ex);
        }
    }
}
