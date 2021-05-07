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
import javafx.beans.property.SimpleStringProperty;
import javafx.beans.property.StringProperty;
import javafx.collections.FXCollections;
import javafx.util.StringConverter;

/**
 * This class represents the current module configuration on the robot.
 * Therefore it must query all ModuleManager in order to get the "full picture",
 * which modules exists and how they depend on each other.
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
    
    /** A message from the responses */
    private final StringProperty flashMessage = new SimpleStringProperty();

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
     * Returns the flash message property.
     * The flash message contains infos of different responses (save, enable)
     * 
     * @return flash message property
     */
    public StringProperty getFlashMessageProperty() {
        return flashMessage;
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
            updateFlashMessage(new String(result));
        } else if (command.getName().endsWith("modules:set")) {
            updateFlashMessage(new String(result));
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
                    mod.getActiveProperty().addListener((ob, o, n) -> { enableModule(mod, n); });
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
    
    /**
     * Updates the flash message in the javafx thread.
     * 
     * @param msg the message to update
     */
    private void updateFlashMessage(String msg) {
        Platform.runLater(() -> { flashMessage.set(msg); });
    }
    
    /**
     * Factory for generating module string converter depending on the current
     * modules list.
     * 
     * @return the string converter for modules
     */
    public StringConverter<Module> getModuleConverter() {
        return new StringModuleConverter();
    }
    
    /**
     * Factory for generating representation string converter depending on the current
     * representations list.
     * 
     * @return the string converter for modules
     */
    public StringConverter<Representation> getRepresentationConverter() {
        return new StringRepresentationConverter();
    }
    
    /**
     * The string converter for modules based on the current modules list.
     */
    class StringModuleConverter extends StringConverter<Module>
    {
        /**
         * Returns the name of the module or an empty string on null.
         * 
         * @param t a module
         * @return  the name of the module
         */
        @Override
        public String toString(Module t) {
            return t == null ? "" : t.toString();
        }

        /**
         * Searches the list of modules and returns a matching module or null if
         * none could be found.
         * 
         * @param string    the modules name to search for
         * @return          the found module or null
         */
        @Override
        public Module fromString(String string) {
            return moduleList.stream().filter((m) -> { return m.toString().compareToIgnoreCase(string) == 0; }).findFirst().orElse(null);
        }
    }

    /**
     * The string converter for representation based on the current modules list.
     */
    class StringRepresentationConverter extends StringConverter<Representation>
    {
        /**
         * Returns the name of the module or an empty string on null.
         * 
         * @param t a module
         * @return  the name of the module
         */
        @Override
        public String toString(Representation t) {
            return t == null ? "" : t.toString();
        }

        /**
         * Searches the list of representations and returns a matching 
         * representation or null if none could be found.
         * 
         * @param string    the representation name to search for
         * @return          the found representation or null
         */
        @Override
        public Representation fromString(String string) {
            return representationList.stream().filter((m) -> { return m.toString().compareToIgnoreCase(string) == 0; }).findFirst().orElse(null);
        }
    }
}
