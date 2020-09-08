package de.naoth.rc.dialogsFx.debugrequests;

import com.google.protobuf.InvalidProtocolBufferException;
import de.naoth.rc.RobotControl;
import de.naoth.rc.componentsFx.TreeNodeCell;
import de.naoth.rc.componentsFx.TreeNodeItem;
import de.naoth.rc.core.messages.Messages;
import de.naoth.rc.core.server.Command;
import de.naoth.rc.core.server.ResponseListener;
import java.util.List;
import java.util.function.BiConsumer;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.application.Platform;
import javafx.fxml.FXML;
import javafx.scene.control.TreeView;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyCodeCombination;
import javafx.scene.input.KeyCombination;
import javafx.scene.input.KeyEvent;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class DebugRequestsTree implements ResponseListener
{
    /** The controlling instance representing the connection to the robot. */
    private RobotControl control;
    
    /** The ui tree of the available debug requests */
    @FXML private TreeView<String> debugTree;
    
    /** Some key shortcut definitions */
    private final KeyCombination shortcutUpdate = new KeyCodeCombination(KeyCode.F5);
    private final KeyCombination shortcutEnableEnter = new KeyCodeCombination(KeyCode.ENTER);
    private final KeyCombination shortcutEnableSpace = new KeyCodeCombination(KeyCode.SPACE);
    
    /** The debug command for retrieving all cognition debug requests */
    private final Command cmd_debug_cognition = new Command("Cognition:representation:get").addArg("DebugRequest");
    
    /** The debug command for retrieving all motion debug requests */
    private final Command cmd_debug_motion = new Command("Motion:representation:get").addArg("DebugRequest");
    
    /** The debug command for activating/deactivating a cognition debug request */
    private final String cmd_debug_cognition_set = "Cognition:representation:set";
    
    /** The debug command for activating/deactivating a motion debug request */
    private final String cmd_debug_motion_set = "Motion:representation:set";

    /** Callback for (de-)activating a cognition debug request */
    private final BiConsumer<String, Boolean> cognitionDebugRequest = (name, active) -> {
        if (control != null && control.checkConnected()) {
            Command request = new Command(cmd_debug_cognition_set).addArg("DebugRequest",
                Messages.DebugRequest.newBuilder().addRequests(
                    Messages.DebugRequest.Item.newBuilder().setName(name).setValue(active)
                ).build().toByteArray());
            control.getMessageServer().executeCommand(this, request);
        }
    };
    /** Callback for (de-)activating a motion debug request */
    private final BiConsumer<String, Boolean> motionDebugRequest = (name, active) -> {
        if (control != null && control.checkConnected()) {
            Command request = new Command(cmd_debug_motion_set).addArg("DebugRequest",
                Messages.DebugRequest.newBuilder().addRequests(
                    Messages.DebugRequest.Item.newBuilder().setName(name).setValue(active)
                ).build().toByteArray());
            control.getMessageServer().executeCommand(this, request);
        }
    };

    /**
     * Default constructor for the FXML loader.
     */
    public DebugRequestsTree() {}
    
    /**
     * Constructor for custom initialization.
     * @param control
     */
    public DebugRequestsTree(RobotControl control) {
        setRobotControl(control);
    }
    
    /**
     * Gets called, after the FXML file was loaded.
     */
    @FXML
    private void initialize() {
        // setup ui
        debugTree.setCellFactory((p) -> new TreeNodeCell<>());
        debugTree.setRoot(new TreeNodeItem<>());
        debugTree.getRoot().setExpanded(true);
    }
    
    /**
     * Sets the controlling instance of the robot.
     * @param control 
     */
    public void setRobotControl(RobotControl control) {
        this.control = control;
    }

    /**
     * Schedules the debug request retrieving command.
     */
    public void updateRequests() {
        if (control != null && control.checkConnected()) {
            control.getMessageServer().executeCommand(this, cmd_debug_cognition);
            control.getMessageServer().executeCommand(this, cmd_debug_motion);
        }
    }
    
    /**
     * Is called, when key event is triggered and handles SPACE/ENTER/F5 key press 
     * to enable a selected debug request or update the request list.
     * 
     * @param k the triggered key event
     */
    @FXML
    private void fxDebugTreeShortcuts(KeyEvent k) {
        // handle space/enter key press and toggle selected state
        if (shortcutEnableEnter.match(k) || shortcutEnableSpace.match(k)) {
            // HACK: make sure that a focused item is also selected
            //       especially the space key toggles the selected state, but
            //       keeps the focus on the item and therefore requires two keystrokes!
            TreeNodeItem i = (TreeNodeItem) debugTree.getSelectionModel().getSelectedItem();
            if (i == null && debugTree.getFocusModel().getFocusedItem() != null) {
                i = (TreeNodeItem) debugTree.getFocusModel().getFocusedItem();
                debugTree.getSelectionModel().select(i);
            }

            if (i != null && i.isLeaf()) {
                i.setSelected(!i.isSelected());
            }
        } else if (shortcutUpdate.match(k)) {
            updateRequests();
        }
    }
    
    /**
     * Handles the response of a debug request and calls the appropriate method to handle the response data.
     * @param result    the response data
     * @param command   the command of the response
     */
    @Override
    public void handleResponse(byte[] result, Command command) {
        Platform.runLater(() -> {
            if(command.equals(cmd_debug_cognition)) {
                handleDebugResponse("Cognition", 0, result, cognitionDebugRequest);
            } else if(command.equals(cmd_debug_motion)) {
                handleDebugResponse("Motion", 1, result, motionDebugRequest);
            } else if(command.getName().equals(cmd_debug_cognition_set) || command.getName().equals(cmd_debug_motion_set)) {
                // NOTE: currently nothing to show or handle
            } else {
                Logger.getLogger(getClass().getName()).log(Level.SEVERE, "Unknown command response: {0}", command);
            }
        });
    }

    /**
     * Prints the error code, if an error occurred.
     * @param code the error code
     */
    @Override
    public void handleError(int code) {
        System.err.println("Robot detected an error: error code " + code);
    }
    
    /**
     * Helper method to handle the response of available debug requests.
     * @param type          cognition|motion
     * @param treeIdx       position of the subtree (0|1)
     * @param response      response data
     * @param debugRequest  callback for the tree leaf (activating/deactivating debug request)
     */
    private void handleDebugResponse(String type, int treeIdx, byte[] response, BiConsumer<String, Boolean> debugRequest) {
        try {
            // parse data
            Messages.DebugRequest request = Messages.DebugRequest.parseFrom(response);
            // get the cognition debug request root item or create a new, if it doesn't exists
            TreeNodeItem root;
            if(((TreeNodeItem)debugTree.getRoot()).hasChildren(type)) {
                root = ((TreeNodeItem)debugTree.getRoot()).getChildren(type);
            } else {
                root = new TreeNodeItem(type);
                root.setExpanded(true);
                // inserts the tree node at the index position or at the end
                if(debugTree.getRoot().getChildren().size() > treeIdx) {
                    debugTree.getRoot().getChildren().add(treeIdx, root);
                } else {
                    debugTree.getRoot().getChildren().add(root);
                }
            }
            List<String> t = root.getExpandedNodes();
            // remove the old subtree
            root.getChildren().clear();
            // create the new subtree
            createDebugRequestTree(request, root, debugRequest);
            root.expandNodes(t);
            // TODO: handle scrollbar -> sometimes after an update the scrollbar isn't shown!
            debugTree.scrollTo(1);
        } catch (InvalidProtocolBufferException ex) {
            Logger.getLogger(getClass().getName()).log(Level.SEVERE, null, ex);
        }
    }
    
    /**
     * Helper method to (re-)create the subtree.
     * @param request       the response data
     * @param root          the root node where the subtree should be appended at
     * @param debugRequest  callback for the tree leaf (activating/deactivating debug request)
     */
    private void createDebugRequestTree(Messages.DebugRequest request, TreeNodeItem<String> root, BiConsumer<String, Boolean> debugRequest)
    {
        request.getRequestsList().forEach((r) -> {
            int pos = r.getName().lastIndexOf(':');
            String name = r.getName().substring(pos+1);
            String path = r.getName().substring(0, pos);
            
            String tooltip = "NO COMMENT";
            if(r.hasDescription() && r.getDescription().length() > 0) {
                tooltip = r.getDescription();
            }
            
            // create the tree path to the item
            TreeNodeItem current_root = root;
            for (String part : path.split(":")) {
                if(!current_root.hasChildren(part)) {
                    TreeNodeItem treePartNew = new TreeNodeItem(part);
                    current_root.getChildren().add(treePartNew);
                    current_root = treePartNew;
                } else {
                    current_root = current_root.getChildren(part);
                }
            }
            
            // add this item to the module tree
            TreeNodeItem item = new TreeNodeItem(name, tooltip);
            current_root.getChildren().add(item);

            // set the selected state AFTER adding it to its parent
            item.setSelected(r.getValue());
            // set the callback for (de-)activating this module (after selecting it!)
            item.selectedProperty().addListener((ob, o, n) -> { debugRequest.accept(r.getName(), n); });
        });
    }
}
