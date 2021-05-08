package de.naoth.rc.dialogsFx.multiagentconfiguration.ui;

import de.naoth.rc.core.manager.ObjectListener;
import de.naoth.rc.dialogsFx.multiagentconfiguration.Utils;
import de.naoth.rc.core.server.Command;
import de.naoth.rc.core.server.ResponseListener;
import java.util.Arrays;
import java.util.List;
import javafx.application.Platform;
import javafx.fxml.FXML;
import javafx.scene.control.ChoiceBox;
import javafx.scene.control.ListView;
import javafx.scene.control.SplitPane;
import javafx.scene.control.TextArea;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class RepresentationsTab implements ResponseListener
{
    @FXML ChoiceBox<Command> type;
    @FXML ListView<String> list;
    @FXML TextArea content;
    @FXML SplitPane split;
    
    private AgentTab parent;
    private DataHandlerPrint print;
    
    private final Command cmd_list_cognition = new ListCommand("Cognition", "Cognition:representation:list");
    private final Command cmd_list_motion = new ListCommand("Motion", "Motion:representation:list");
    
    @FXML
    public void initialize() {
        type.getItems().add(cmd_list_cognition);
        type.getItems().add(cmd_list_motion);
        
        type.setValue(cmd_list_cognition);
        type.setOnAction((e) -> { typeChanged(); });
        
        list.getSelectionModel().selectedItemProperty().addListener((ob, o, n) -> {
            if(n != null) {
                unsubscribeRepresentations();
                subscribeRepresentations(n);
            }
        });
    }
    
    public void setParent(AgentTab t) {
        parent = t;
    }
    
    public void show(boolean visible) {
        if(visible) {
            if(list.getItems().isEmpty()) {
                typeChanged();
            } else if(!list.getSelectionModel().isEmpty()) {
                subscribeRepresentations(list.getSelectionModel().getSelectedItem());
            }
        } else {
            unsubscribeRepresentations();
        }
    }
    
    private void subscribeRepresentations(String n) {
        print = new DataHandlerPrint();
        Command cmd = new Command(type.getValue().toString() + ":representation:print").addArg(n);
        parent.subscribeCommand(cmd, print);
    }
    
    private void unsubscribeRepresentations() {
        if(print != null) {
            parent.unsubscribeCommand(print);
            print = null;
        }
    }
    
    private void typeChanged() {
        unsubscribeRepresentations();
        if(!parent.sendCommand(type.getValue(), this)) {
            System.err.println("Unable to change representation type on " + parent.getText());
        }
    }
    
    @FXML
    public void updateList() {
        typeChanged();
    }

    @Override
    public void handleResponse(byte[] result, Command command) {
        if(command.equals(cmd_list_cognition) || command.equals(cmd_list_motion)) {
            Platform.runLater(() -> {
                List<String> items = Arrays.asList(new String(result).split("\n"));
                list.getItems().setAll(items);
                // avoid duplicates in global list
                items.forEach((s) -> { if(!Utils.global_representations_list.contains(s)) { Utils.global_representations_list.add(s); } });
            });
        }
    }

    @Override
    public void handleError(int code) {
        System.err.println("Got error response: " + code);
    }
    
    class ListCommand extends Command {
        private final String listName;

        public ListCommand(String listName, String commandName) {
            super(commandName);
            this.listName = listName;
        }

        @Override
        public String toString() {
            return listName;
        }
    }
    
    class DataHandlerPrint implements ObjectListener<byte[]>
    {
        @Override
        public void newObjectReceived(byte[] object) {
            Platform.runLater(() -> {
                content.setText(new String(object));
            });
        }

        @Override
        public void errorOccured(String cause) {
            Platform.runLater(() -> {
                content.setPromptText(cause);
                content.clear();
            });
        }
    }
}
