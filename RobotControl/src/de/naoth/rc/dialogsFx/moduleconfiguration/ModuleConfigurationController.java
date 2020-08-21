package de.naoth.rc.dialogsFx.moduleconfiguration;

import com.google.protobuf.InvalidProtocolBufferException;
import de.naoth.rc.RobotControl;
import de.naoth.rc.core.messages.Messages;
import de.naoth.rc.core.server.Command;
import de.naoth.rc.core.server.ResponseListener;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.application.Platform;
import javafx.fxml.FXML;
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
    @FXML protected TreeView<String> moduleTree;
    
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
                handleCognitionModulesResponse(result);
            } else if(command.equals(cmd_modules_motion)) {
                handleMotionModulesResponse(result);
            } else {
                Logger.getLogger(getClass().getName()).log(Level.SEVERE, "Unknown command response: {0}", command);
            }
        });
    }

    @Override
    public void handleError(int code) {
    }
    
    private void handleCognitionModulesResponse(byte[] result) {
        try {
            Messages.ModuleList list = Messages.ModuleList.parseFrom(result);
            // TODO:
            //moduleTree.getRoot().getChildren().add(Utils.createModulesCognition(list, cognitionModuleRequest));
        } catch (InvalidProtocolBufferException ex) {
            Logger.getLogger(ModuleConfigurationController.class.getName()).log(Level.SEVERE, null, ex);
        }

    }
    
    private void handleMotionModulesResponse(byte[] result) {
        try {
            Messages.ModuleList list = Messages.ModuleList.parseFrom(result);
            // TODO:
            //moduleTree.getRoot().getChildren().add(0, Utils.createModulesMotion(list, motionModuleRequest));
        } catch (InvalidProtocolBufferException ex) {
            Logger.getLogger(ModuleConfigurationController.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
}
