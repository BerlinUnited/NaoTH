package de.naoth.rc.dialogs.multiagentconfiguration.ui;

import de.naoth.rc.dialogs.multiagentconfiguration.Parameter;
import de.naoth.rc.dialogs.multiagentconfiguration.Utils;
import java.io.File;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.beans.property.SimpleObjectProperty;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.Node;
import javafx.scene.control.Button;

import javafx.scene.control.CheckBoxTreeItem;
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
import javafx.stage.FileChooser;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class AllTab extends Tab
{
    @FXML protected AllTabConfigurationsTab configurationsTabViewController;
    
    @FXML protected TabPane tabs;
    @FXML protected SplitPane split;
    @FXML protected GridPane contentArea;
    @FXML protected ListView list;
    @FXML protected ChoiceBox type;
    @FXML protected Button btnUpdateList;

    public AllTab() {
        try {
            FXMLLoader loader = new FXMLLoader(getClass().getResource("/de/naoth/rc/dialogs/multiagentconfiguration/ui/AllTab.fxml"));
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
        CheckBoxTreeItem<String> motion_root = new CheckBoxTreeItem<>("Motion");
        motion_root.setExpanded(true);
        configurationsTabViewController.debugTree.getRoot().getChildren().add(motion_root);
        Utils.global_debug_requests.put(motion_root.getValue(), motion_root);

        CheckBoxTreeItem<String> cognition_root = new CheckBoxTreeItem<>("Cognition");
        cognition_root.setExpanded(true);
        configurationsTabViewController.debugTree.getRoot().getChildren().add(cognition_root);
        Utils.global_debug_requests.put(cognition_root.getValue(), cognition_root);

        CheckBoxTreeItem<String> motion_modules_root = new CheckBoxTreeItem<>("Motion");
        motion_modules_root.setExpanded(true);
        
        configurationsTabViewController.moduleTree.getRoot().getChildren().add(motion_modules_root);
        Utils.global_modules.put(motion_modules_root.getValue(), motion_modules_root);

        CheckBoxTreeItem<String> cognition_modules_root = new CheckBoxTreeItem<>("Cognition");
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
            FXMLLoader loader = new FXMLLoader(getClass().getResource("/de/naoth/rc/dialogs/multiagentconfiguration/ui/RepresentationsContent.fxml"));
            loader.setController(rcc);
            contentArea.add(loader.load(), col, row); // column, row
            
            rcc.setName(name);
            rcc.getTextProperty().bind(t.textProperty());
        } catch (IOException ex) {
            Logger.getLogger(AllTab.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
    
    public void removeRepresentationsView(String name) {
        for (Node node : contentArea.getChildren()) {
            System.out.println(node);
            // TODO: remove Node if it matches?!
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
            FileChooser fileChooser = new FileChooser();
            fileChooser.setTitle("Open Behavior File");
            fileChooser.setInitialFileName("behavior-ic.dat");
            fileChooser.setInitialDirectory(new File("../../NaoTHSoccer/Config/"));
            fileChooser.getExtensionFilters().addAll(
                    new FileChooser.ExtensionFilter("Behavior Files", "*.dat"),
                    new FileChooser.ExtensionFilter("All Files", "*.*"));
            File selectedFile = fileChooser.showOpenDialog(getTabPane().getScene().getWindow());
            if (selectedFile != null) {
                behaviorFile.set(selectedFile);
            }
        }

        @FXML
        private void saveParameters() { /* not used in the all tab (hidden) */ }

        @FXML
        private void updateParameters() { /* agent tab did bind itself to this button */ }

        @FXML
        private void updateModules() { /* agent tab did bind itself to this button */ }

        @FXML
        private void updateDebugRequests() { /* agent tab did bind itself to this button */ }

        @FXML
        private void saveParametersMotion() { /* not used in the all tab (hidden) */ }

        @FXML
        private void saveParametersCognition() { /* not used in the all tab (hidden) */ }

        @FXML
        private void saveModules() { /* agent tab did bind itself to this button */ }
    }
}
