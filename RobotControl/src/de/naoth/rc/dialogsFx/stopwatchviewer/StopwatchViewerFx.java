package de.naoth.rc.dialogsFx.stopwatchviewer;

import de.naoth.rc.RobotControl;
import de.naoth.rc.core.dialog.AbstractJFXDialog;
import de.naoth.rc.core.dialog.DialogPlugin;
import de.naoth.rc.core.dialog.RCDialog;
import de.naoth.rc.core.manager.ObjectListener;
import de.naoth.rc.manager.GenericManager;
import de.naoth.rc.manager.GenericManagerFactory;
import de.naoth.rc.core.server.Command;
import java.net.URL;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.beans.binding.Bindings;
import javafx.beans.binding.BooleanBinding;
import javafx.beans.property.DoubleProperty;
import javafx.beans.property.SimpleDoubleProperty;
import javafx.beans.property.SimpleStringProperty;
import javafx.beans.property.StringProperty;
import javafx.fxml.FXML;
import javafx.scene.control.Button;
import javafx.scene.control.ChoiceBox;
import javafx.scene.control.Label;
import javafx.scene.control.Spinner;
import javafx.scene.control.TableColumn;
import javafx.scene.control.TableView;
import javafx.scene.control.ToggleButton;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class StopwatchViewerFx extends AbstractJFXDialog {

    @RCDialog(category = RCDialog.Category.Status, name = "Stopwatch (FX)")
    @PluginImplementation
    public static class Plugin extends DialogPlugin<StopwatchViewerFx> {
        @InjectPlugin
        static public RobotControl parent;
        @InjectPlugin
        public static GenericManagerFactory genericManagerFactory;
    }
    
    @FXML
    private ToggleButton btnListen;
    
    @FXML
    private TableView<StopWatch> stopwatchTable;
    
    @FXML
    private ChoiceBox<String> stopwatchType;
    
    @FXML
    private Button stopwatchReset;
    
    @FXML
    private ToggleButton stopwatchSorting;

    @FXML
    private ToggleButton stopwatchModules;
    
    @FXML
    private Spinner stopwatchWarn;
    
    @FXML
    private Label stopwatchTablePlaceholder;
    
    GenericManager currentManager = null;
    
    final StopWatchListener stopWatchListener = new StopWatchListener();
    
    @Override
    protected boolean isSelfController() {
        return true;
    }

    @Override
    public URL getFXMLRessource() {
        return getClass().getResource("StopwatchViewerFx.fxml");
    }

    @Override
    public void afterInit() {
        initStatusTable();
        
        
    } // afterInit()
    
    /**
     * Initializes the status table.
     * Sets the available columns, connects properties and sets some listeners.
     */
    private void initStatusTable() {
        // binding for displaying the tables placeholder label
        stopwatchTablePlaceholder.textProperty().bind(Bindings.when(btnListen.selectedProperty()).then("No data available ...").otherwise("Not connected ..."));
        BooleanBinding condition = btnListen.selectedProperty().not();
        stopwatchType.disableProperty().bind(condition);
        stopwatchReset.disableProperty().bind(condition);
        stopwatchSorting.disableProperty().bind(condition);
        stopwatchModules.disableProperty().bind(condition);
        stopwatchWarn.disableProperty().bind(condition);
        
        // available columns
        TableColumn[] cols = {
            new TableColumn("Name"),
            new TableColumn("Value"),
            new TableColumn("Average"),
            new TableColumn("Min"),
            new TableColumn("Max")
        };
        
        // add columns to table
        stopwatchTable.getColumns().addAll(cols);
        stopwatchTable.setColumnResizePolicy(TableView.CONSTRAINED_RESIZE_POLICY);
        
        stopwatchType.getItems().addAll("Cognition", "Motion");
        stopwatchType.getSelectionModel().selectFirst();
        
        // (empty) table data
        //statusTable.setItems(robots);
    } // initStatusTable
    
    
    /**
     * The handler for the listen button.
     * Start/Stops all listener and updates the ui accordingly.
     */
    @FXML
    private void actionListen() {
        System.out.println("listen");
        registerListeners();
    } // actionListen()
    
    @FXML
    private void reset() {
        stopwatchTable.getItems().clear();
    }

    private void registerListeners() {
        if (!Plugin.parent.checkConnected()) {
            btnListen.setSelected(false);
            return;
        }

        if (currentManager != null) {
            currentManager.removeListener(this.stopWatchListener);
        }

        if (btnListen.isSelected()) {
            reset();

            String process = stopwatchType.getValue();

            Command command;
            if (stopwatchModules.isSelected()) {
                command = new Command(process + ":modules:stopwatch");
            } else {
                command = new Command(process + ":representation:get").addArg("StopwatchManager");
            }

            currentManager = Plugin.genericManagerFactory.getManager(command);
            currentManager.addListener(this.stopWatchListener);
        }
    }

    class StopWatch
    {
        private final StringProperty name = new SimpleStringProperty("");
        private final DoubleProperty value = new SimpleDoubleProperty(0.0);
        private final DoubleProperty avg = new SimpleDoubleProperty(0.0);
        private final DoubleProperty min = new SimpleDoubleProperty(0.0);
        private final DoubleProperty max = new SimpleDoubleProperty(0.0);
        //Name
        //Value
        //Average
        //Min
        //Max
        public void setName(String n) {
        }
    } // END StopWatch
    
    class StopWatchListener implements ObjectListener<byte[]>
    {
        @Override
        public void newObjectReceived(byte[] object) {
            /*
            SwingUtilities.invokeLater(()->{
                try
                {

                  Messages.Stopwatches stopwatches = Messages.Stopwatches.parseFrom(object);
                  // collect all times into a map
                  Map<String, Integer> map = stopwatches.getStopwatchesList().stream().collect(
                      Collectors.toMap(i->i.getName(), i->i.getTime())
                  );

                  // update the table
                  ((StopwatchTableModel)stopwatchTable.getModel()).update(map);
                  if (cbAutoSort.isSelected()) {
                    ((TableRowSorter<TableModel>)stopwatchTable.getRowSorter()).sort();
                  }
                  stopwatchTable.repaint();
                }
                catch(InvalidProtocolBufferException ex)
                {
                  Logger.getLogger(StopwatchViewer.class.getName()).log(Level.SEVERE, null, ex);

                  if (allAscii(object)) {
                    errorOccured(new String(object));
                  } else {
                    errorOccured("Unknown binary data was received");
                  }
                }
            });
            */
        }

        @Override
        public void errorOccured(String cause) {
            dispose();
            Logger.getAnonymousLogger().log(Level.SEVERE, cause);

//            JOptionPane.showMessageDialog(StopwatchViewer.this, cause, "Error", JOptionPane.ERROR_MESSAGE);
        }
  } // END StopWatchListener
}
