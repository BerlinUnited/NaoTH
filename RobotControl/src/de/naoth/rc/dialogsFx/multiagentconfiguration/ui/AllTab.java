package de.naoth.rc.dialogsFx.multiagentconfiguration.ui;

import de.naoth.rc.dialogsFx.multiagentconfiguration.components.TreeNode;
import de.naoth.rc.dialogsFx.multiagentconfiguration.components.CheckableTreeCell;
import de.naoth.rc.dialogsFx.multiagentconfiguration.Parameter;
import de.naoth.rc.dialogsFx.multiagentconfiguration.Utils;
import java.io.File;
import java.io.IOException;
import java.util.Optional;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.beans.property.SimpleObjectProperty;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.Node;
import javafx.scene.control.Button;

import javafx.scene.control.ChoiceBox;
import javafx.scene.control.ComboBox;
import javafx.scene.control.Label;
import javafx.scene.control.ListView;
import javafx.scene.control.SplitMenuButton;
import javafx.scene.control.SplitPane;
import javafx.scene.control.Tab;
import javafx.scene.control.TabPane;
import javafx.scene.control.TextArea;
import javafx.scene.control.TreeItem;
import javafx.scene.control.TreeTableColumn;
import javafx.scene.control.TreeTableView;
import javafx.scene.control.TreeView;
import javafx.scene.control.cell.TreeItemPropertyValueFactory;
import javafx.scene.layout.GridPane;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class AllTab extends Tab
{
    @FXML protected AllTabConfigurationsTab configurationsTabViewController;
    
    @FXML protected TabPane tabs;
    @FXML protected SplitPane split;
    @FXML protected GridPane contentArea;
    @FXML protected ListView<String> list;
    @FXML protected ChoiceBox type;
    @FXML protected Button btnUpdateList;

    public AllTab() {
        try {
            FXMLLoader loader = new FXMLLoader(getClass().getResource("AllTab.fxml"));
            loader.setController(this);
            loader.setControllerFactory((type) -> {
                try {
                    if(type == ConfigurationsTab.class) {
                        return new AllTabConfigurationsTab();
                    }
                    // default implementation:
                    return type.newInstance();
                } catch (Exception exc) {
                    // this is pretty much fatal...
                    throw new RuntimeException(exc);
                }
            });
            setContent(loader.load());
        } catch (IOException ex) {
            Logger.getLogger(getClass().getName()).log(Level.SEVERE, null, ex);
        }
        setText("All");
        //setDisable(true);
    }
    
    @FXML
    public void initialize() {
        TreeNode<String> motion_root = new TreeNode("Motion");
        motion_root.setExpanded(true);
        configurationsTabViewController.debugTree.getRoot().getChildren().add(motion_root);
        Utils.global_debug_requests.put(motion_root.getValue(), motion_root);

        TreeNode<String> cognition_root = new TreeNode("Cognition");
        cognition_root.setExpanded(true);
        configurationsTabViewController.debugTree.getRoot().getChildren().add(cognition_root);
        Utils.global_debug_requests.put(cognition_root.getValue(), cognition_root);

        TreeNode<String> motion_modules_root = new TreeNode("Motion");
        motion_modules_root.setExpanded(true);
        
        configurationsTabViewController.moduleTree.getRoot().getChildren().add(motion_modules_root);
        Utils.global_modules.put(motion_modules_root.getValue(), motion_modules_root);

        TreeNode<String> cognition_modules_root = new TreeNode("Cognition");
        cognition_modules_root.setExpanded(true);
        configurationsTabViewController.moduleTree.getRoot().getChildren().add(cognition_modules_root);
        Utils.global_modules.put(cognition_modules_root.getValue(), cognition_modules_root);

        configurationsTabViewController.agentList.setItems(Utils.global_agent_list);
        TreeItem<Parameter> motion_params = new TreeItem<>(new Parameter("Motion", null));
        motion_params.setExpanded(true);
        configurationsTabViewController.parameterTree.getRoot().getChildren().add(motion_params);
        Utils.global_parameters.put(motion_params.getValue().getName(), motion_params);
        
        TreeItem<Parameter> cognition_params = new TreeItem<>(new Parameter("Cognition", null));
        cognition_params.setExpanded(true);
        configurationsTabViewController.parameterTree.getRoot().getChildren().add(cognition_params);
        Utils.global_parameters.put(cognition_params.getValue().getName(), cognition_params);
        
        type.valueProperty().addListener((ob) -> {
            list.getItems().clear();
        });
        
        list.setItems(Utils.global_representations_list);
        list.selectionModelProperty().addListener((ob, o, n) -> {
            if(n != null) {
                
            }
        });
        
        split.setDividerPosition(0, 0.2);
        // hide parameter save button - it doesn't make sense to save the parameter of all robots at once!
        configurationsTabViewController.btnSaveParameters.setVisible(false);
    }
    
    public void addRepresentationsView(TextArea t, String name) {
        int col = contentArea.getChildren().size() % 3;
        int row = contentArea.getChildren().size() / 3;
        try {
            RepresentationsContent rcc = new RepresentationsContent();
            FXMLLoader loader = new FXMLLoader(getClass().getResource("RepresentationsContent.fxml"));
            loader.setController(rcc);
            
            Node n = loader.load();
            n.setUserData(name); // to identify the node on removal
            contentArea.add(n, col, row); // column, row
            
            rcc.setName(name);
            // replace old content with the new (and preserve scrollbar position)
            t.textProperty().addListener((ob, ov, nv) -> {
                rcc.content.replaceText(0, rcc.content.getText().length(), nv);
            });
        } catch (IOException ex) {
            Logger.getLogger(AllTab.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
    
    public void removeRepresentationsView(String name) {
        Optional<Node> node = contentArea.getChildren().stream().filter((n) -> { return n.getUserData().equals(name); }).findFirst();
        if (node.isPresent()) {
            contentArea.getChildren().remove(node.get());
        }
    }
    
    class AllTabConfigurationsTab
    {
        @FXML
        protected TreeView<String> debugTree;
        @FXML
        protected TreeView<String> moduleTree;
        @FXML
        protected TreeTableView<Parameter> parameterTree;
        @FXML
        protected SplitMenuButton btnSaveParameters;
        @FXML
        protected Button btnSaveModules;
        @FXML
        protected SplitPane splitPane;
        @FXML
        protected Button btnUpdateDebug;
        @FXML
        protected Button btnUpdateModules;
        @FXML
        protected Button btnUpdateParameters;
        @FXML
        protected Button btnSendBehavior;
        @FXML
        protected ComboBox<String> agentList;
        @FXML
        protected Label lblScheme;
        
        protected final SimpleObjectProperty<File> behaviorFile = new SimpleObjectProperty<>(null);

        @FXML
        public void initialize() {
            // setup ui
            debugTree.setCellFactory(e -> new CheckableTreeCell());
            debugTree.setRoot(new TreeItem());
            debugTree.getRoot().setExpanded(true);

            moduleTree.setCellFactory(e -> new CheckableTreeCell());
            moduleTree.setRoot(new TreeItem());
            moduleTree.getRoot().setExpanded(true);

            parameterTree.setShowRoot(false);

            TreeItem<Parameter> parameterTreeRoot = new TreeItem<>(new Parameter("root", null));

            parameterTree.setRoot(parameterTreeRoot);
            parameterTree.getRoot().setExpanded(true);

            TreeTableColumn<Parameter, String> col_name = (TreeTableColumn<Parameter, String>) parameterTree.getColumns().get(0);
            TreeTableColumn<Parameter, String> col_value = (TreeTableColumn<Parameter, String>) parameterTree.getColumns().get(1);
            col_name.setCellValueFactory(new TreeItemPropertyValueFactory("name"));
            col_value.setCellValueFactory(new TreeItemPropertyValueFactory("value"));
            col_value.setCellFactory(list -> new ParameterTreeTableCell());

            col_value.setOnEditCommit((evt) -> {
                if (!evt.getRowValue().isLeaf()) {
                    return;
                }
                evt.getRowValue().getValue().setValue(evt.getNewValue());
                evt.getTreeTableView().requestFocus();
            });
        }

        @FXML
        private void selectBehaviorFile() {
            File selectedFile = Utils.showBehaviorFileDialog(getTabPane().getScene().getWindow());
            if (selectedFile != null) {
                behaviorFile.set(selectedFile);
            }
        }

        @FXML
        private void saveParameters() { /* not used in the all tab (hidden) */ }

        @FXML
        private void updateParameters() {
            // remove existing nodes of cognition & motion debug request, but keep root nodes.
            Utils.global_parameters.clear();
            configurationsTabViewController.parameterTree.getRoot().getChildren().forEach((n) -> {
                n.getChildren().clear();
                Utils.global_parameters.put(n.getValue().getName(), n);
            });
        }

        @FXML
        private void updateModules() {
            // remove existing nodes of cognition & motion modules, but keep root nodes.
            Utils.global_modules.clear();
            configurationsTabViewController.moduleTree.getRoot().getChildren().forEach((n) -> {
                n.getChildren().clear();
                Utils.global_modules.put(n.getValue(), (TreeNode) n);
            });
        }

        @FXML
        private void updateDebugRequests() {
            // remove existing nodes of cognition & motion debug request, but keep root nodes.
            Utils.global_debug_requests.clear();
            configurationsTabViewController.debugTree.getRoot().getChildren().forEach((n) -> {
                n.getChildren().clear();
                Utils.global_debug_requests.put(n.getValue(), (TreeNode) n);
            });
        }

        @FXML
        private void saveParametersMotion() { /* not used in the all tab (hidden) */ }

        @FXML
        private void saveParametersCognition() { /* not used in the all tab (hidden) */ }

        @FXML
        private void saveModules() { /* agent tab did bind itself to this button */ }
    }
}
