package de.naoth.rc.dialogsFx.debugrequests;

import com.google.protobuf.InvalidProtocolBufferException;
import de.naoth.rc.RobotControl;
import de.naoth.rc.core.dialog.AbstractJFXDialog;
import de.naoth.rc.core.dialog.DialogPlugin;
import de.naoth.rc.core.dialog.RCDialog;
import de.naoth.rc.componentsFx.CheckableTreeCell;
import de.naoth.rc.componentsFx.TreeNode;
import de.naoth.rc.core.manager.SwingCommandExecutor;
import de.naoth.rc.core.messages.Messages;
import de.naoth.rc.core.server.Command;
import de.naoth.rc.core.server.ResponseListener;
import java.net.URL;
import java.util.List;
import java.util.function.BiConsumer;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.application.Platform;
import javafx.fxml.FXML;
import javafx.scene.control.TreeView;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class DebugRequestsFx extends AbstractJFXDialog implements ResponseListener
{
    @RCDialog(category = RCDialog.Category.Debug, name = "DebugRequest (FX)")
    @PluginImplementation
    public static class Plugin extends DialogPlugin<DebugRequestsFx> {
        @InjectPlugin
        public static RobotControl parent;
        @InjectPlugin
        public static SwingCommandExecutor commandExecutor;
    }
    
    @FXML protected TreeView<String> debugTree;
    
    private final Command cmd_debug_cognition = new Command("Cognition:representation:get").addArg("DebugRequest");
    private final Command cmd_debug_motion = new Command("Motion:representation:get").addArg("DebugRequest");
    private final String cmd_debug_cognition_set = "Cognition:representation:set";
    private final String cmd_debug_motion_set = "Motion:representation:set";

    private final BiConsumer<String, Boolean> cognitionDebugRequest = (r, b) -> {
        Command request = new Command(cmd_debug_cognition_set).addArg("DebugRequest",
            Messages.DebugRequest.newBuilder().addRequests(
                Messages.DebugRequest.Item.newBuilder().setName(r).setValue(b)
            ).build().toByteArray());
        Plugin.parent.getMessageServer().executeCommand(this, request);
    };
    private final BiConsumer<String, Boolean> motionDebugRequest = (r, b) -> {
        Command request = new Command(cmd_debug_motion_set).addArg("DebugRequest",
            Messages.DebugRequest.newBuilder().addRequests(
                Messages.DebugRequest.Item.newBuilder().setName(r).setValue(b)
            ).build().toByteArray());
        Plugin.parent.getMessageServer().executeCommand(this, request);
    };
    
    @Override
    protected boolean isSelfController() {
        return true;
    }

    @Override
    public URL getFXMLRessource() {
        return getClass().getResource("DebugRequestsFx.fxml");
    }
    
    @Override
    public void afterInit() {
        // setup ui
        debugTree.setCellFactory(e -> new CheckableTreeCell<>());
        debugTree.setRoot(new TreeNode<>());
        debugTree.getRoot().setExpanded(true);
    }

    @FXML
    private void updateRequests() {
        if (Plugin.parent.checkConnected()) {
            Plugin.parent.getMessageServer().executeCommand(this, cmd_debug_cognition);
            Plugin.parent.getMessageServer().executeCommand(this, cmd_debug_motion);
        }
    }
    
    @Override
    public void handleResponse(byte[] result, Command command) {
        Platform.runLater(() -> {
            if(command.equals(cmd_debug_cognition)) {
                handleDebugResponse("Cognition", 1, result, cognitionDebugRequest);
            } else if(command.equals(cmd_debug_motion)) {
                handleDebugResponse("Motion", 0, result, motionDebugRequest);
            } else if(command.getName().equals(cmd_debug_cognition_set) || command.getName().equals(cmd_debug_motion_set)) {
                // NOTE: currently nothing to show or handle
            } else {
                Logger.getLogger(getClass().getName()).log(Level.SEVERE, "Unknown command response: {0}", command);
            }
        });
    }

    @Override
    public void handleError(int code) {
        // currently ignoring - what else could we do?
    }
    
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
                // 
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
    
    private void createDebugRequestTree(Messages.DebugRequest request, TreeNode<String> root, BiConsumer<String, Boolean> debugRequest)
    {
        String root_name = root.getValue();
        request.getRequestsList().forEach((r) -> {
            int pos = r.getName().lastIndexOf(':');
            String id = root_name + ":" + r.getName(); // the identifier of this debug request
            String name = r.getName().substring(pos+1);
            String path = r.getName().substring(0, pos);

            RequestTreeItem item = new RequestTreeItem(path, name);
            item.setRequest(r.getName());
            
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
            current_root.getChildren().add(item); // add this item to the module tree

            // set the selected state AFTER adding it to its parent
            item.setSelected(r.getValue());
            // set the callback for (de-)activating this module (after selecting it!)
            item.selectedProperty().addListener((ob, o, n) -> { debugRequest.accept(r.getName(), n); });
        });
    }
}
