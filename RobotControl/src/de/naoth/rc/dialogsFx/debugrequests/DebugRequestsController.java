package de.naoth.rc.dialogsFx.debugrequests;

import com.google.protobuf.InvalidProtocolBufferException;
import de.naoth.rc.componentsFx.CheckableTreeCell;
import de.naoth.rc.componentsFx.TreeNode;
import de.naoth.rc.core.messages.Messages;
import de.naoth.rc.core.server.Command;
import de.naoth.rc.core.server.MessageServer;
import de.naoth.rc.core.server.ResponseListener;
import java.util.List;
import java.util.function.BiConsumer;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.application.Platform;
import javafx.fxml.FXML;
import javafx.scene.control.TreeView;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class DebugRequestsController implements ResponseListener
{
    /** The message server, where the debug requests should be send to */
    private MessageServer server;
    
    /** The ui tree of the available debug requests */
    @FXML protected TreeView<String> debugTree;
    
    /** The debug command for retrieving all cognitinon debug requests */
    private final Command cmd_debug_cognition = new Command("Cognition:representation:get").addArg("DebugRequest");
    
    /** The debug command for retrieving all motion debug requests */
    private final Command cmd_debug_motion = new Command("Motion:representation:get").addArg("DebugRequest");
    
    /** The debug command for activating/deactivating a cognition debug request */
    private final String cmd_debug_cognition_set = "Cognition:representation:set";
    
    /** The debug command for activating/deactivating a motion debug request */
    private final String cmd_debug_motion_set = "Motion:representation:set";

    /** Callback for (de-)activating a cognition debug request */
    private final BiConsumer<String, Boolean> cognitionDebugRequest = (name, active) -> {
        if (server != null) {
            Command request = new Command(cmd_debug_cognition_set).addArg("DebugRequest",
                Messages.DebugRequest.newBuilder().addRequests(
                    Messages.DebugRequest.Item.newBuilder().setName(name).setValue(active)
                ).build().toByteArray());
            server.executeCommand(this, request);
        }
    };
    /** Callback for (de-)activating a motion debug request */
    private final BiConsumer<String, Boolean> motionDebugRequest = (name, active) -> {
        if (server != null) {
            Command request = new Command(cmd_debug_motion_set).addArg("DebugRequest",
                Messages.DebugRequest.newBuilder().addRequests(
                    Messages.DebugRequest.Item.newBuilder().setName(name).setValue(active)
                ).build().toByteArray());
            server.executeCommand(this, request);
        }
    };

    /**
     * Default constructor for the FXML loader.
     */
    public DebugRequestsController() {}
    
    /**
     * Constructor for custom initialization.
     */
    public DebugRequestsController(MessageServer server) {
        setMessageServer(server);
    }
    
    /**
     * Gets called, after the FXML file was loaded.
     */
    @FXML
    private void initialize() 
    {
        // setup ui
        debugTree.setCellFactory(e -> new CheckableTreeCell<>());
        debugTree.setRoot(new TreeNode<>());
        debugTree.getRoot().setExpanded(true);
    }
    
    /**
     * Sets the message server.
     * @param server 
     */
    public void setMessageServer(MessageServer server) {
        this.server = server;
    }

    /**
     * Handles the update button click.
     */
    @FXML
    private void updateRequests() {
        if (server != null && server.isConnected()) {
            server.executeCommand(this, cmd_debug_cognition);
            server.executeCommand(this, cmd_debug_motion);
        }
    }
    
    /**
     * Handles the response of a debug request and calls the appropiate method to handle the response data.
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
            TreeNode root;
            if(((TreeNode)debugTree.getRoot()).hasChildren(type)) {
                root = ((TreeNode)debugTree.getRoot()).getChildren(type);
            } else {
                root = new TreeNode(type);
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
    private void createDebugRequestTree(Messages.DebugRequest request, TreeNode<String> root, BiConsumer<String, Boolean> debugRequest)
    {
        request.getRequestsList().forEach((r) -> {
            int pos = r.getName().lastIndexOf(':');
            String name = r.getName().substring(pos+1);
            String path = r.getName().substring(0, pos);
            
            // create the tree path to the item
            TreeNode current_root = root;
            for (String part : path.split(":")) {
                if(!current_root.hasChildren(part)) {
                    TreeNode treePartNew = new TreeNode(part);
                    current_root.getChildren().add(treePartNew);
                    current_root = treePartNew;
                } else {
                    current_root = current_root.getChildren(part);
                }
            }
            
            // add this item to the module tree
            TreeNode item = new TreeNode(name);
            current_root.getChildren().add(item);

            // set the selected state AFTER adding it to its parent
            item.setSelected(r.getValue());
            // set the callback for (de-)activating this module (after selecting it!)
            item.selectedProperty().addListener((ob, o, n) -> { debugRequest.accept(r.getName(), n); });
        });
    }
}
