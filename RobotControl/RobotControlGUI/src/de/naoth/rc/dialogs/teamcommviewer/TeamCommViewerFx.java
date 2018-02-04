package de.naoth.rc.dialogs.teamcommviewer;

import de.naoth.rc.Helper;
import de.naoth.rc.RobotControl;
import de.naoth.rc.components.teamcomm.TeamCommManager;
import de.naoth.rc.components.teamcomm.TeamCommMessage;
import de.naoth.rc.components.teamcommviewer.RobotStatus;
import de.naoth.rc.components.teamcommviewer.RobotTeamCommListener;
import de.naoth.rc.components.teamcommviewer.TeamCommLogger;
import de.naoth.rc.core.dialog.AbstractJFXDialog;
import de.naoth.rc.core.dialog.DialogPlugin;
import de.naoth.rc.core.dialog.RCDialog;
import de.naoth.rc.drawingmanager.DrawingEventManager;
import de.naoth.rc.drawings.DrawingCollection;
import de.naoth.rc.math.Vector2D;
import java.awt.Color;
import java.io.File;
import java.io.IOException;
import java.lang.reflect.Field;
import java.net.URL;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.application.Platform;
import javafx.beans.binding.Bindings;
import javafx.beans.property.ReadOnlyObjectWrapper;
import javafx.beans.property.SimpleBooleanProperty;
import javafx.beans.property.SimpleStringProperty;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.event.Event;
import javafx.event.EventHandler;
import javafx.fxml.FXML;
import javafx.scene.control.Button;
import javafx.scene.control.ColorPicker;
import javafx.scene.control.Label;
import javafx.scene.control.Spinner;
import javafx.scene.control.TableCell;
import javafx.scene.control.TableColumn;
import javafx.scene.control.TableColumn.CellDataFeatures;
import javafx.scene.control.TableView;
import javafx.scene.control.ToggleButton;
import javafx.scene.control.Tooltip;
import javafx.scene.control.cell.CheckBoxTableCell;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.stage.FileChooser;
import javafx.stage.FileChooser.ExtensionFilter;
import javafx.util.Callback;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

// TODO: on close - save file if necessary?!
// TODO: save/restore column config?!
// TODO: Spinner editor / converter

/**
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class TeamCommViewerFx extends AbstractJFXDialog
{
    @RCDialog(category = RCDialog.Category.Tools, name = "TeamCommViewerFx")
    @PluginImplementation
    public static class Plugin extends DialogPlugin<TeamCommViewerFx> {
        @InjectPlugin
        public static RobotControl parent;
        @InjectPlugin
        public static DrawingEventManager drawingEventManager;
        @InjectPlugin
        public static TeamCommManager teamcommManager;
    }
    
    private Timer timerUpdateStatusTable;
    private Timer timerDrawRobots;
    
    /**
     * The robot-TeamCommProvider for our own team.
     */
    private final RobotTeamCommListener listenerOwn = new RobotTeamCommListener(false);
    
    /**
     * The robot-TeamCommProvider for our own team.
     */
    private final RobotTeamCommListener listenerOpponent = new RobotTeamCommListener(true);

    /**
     * Receiver for the TeamMessages.
     */
    private final TeamCommMessageListener teamcommListener = new TeamCommMessageListener();
    
    /**
     * Container for storing the current status of each robot.
     */
    private final ObservableList<RobotStatus> robots = FXCollections.observableArrayList();
    
    /**
     * Logger object, for logging TeamCommMessages.
     */
    private final TeamCommLogger log = new TeamCommLogger();

    @FXML
    private TableView<RobotStatus> statusTable;
    
    @FXML
    private Spinner ownPort;
    
    @FXML
    private Spinner oppPort;
    
    @FXML
    private ToggleButton btnListen;
    
    @FXML
    private ToggleButton btnRecord;
    
    @FXML
    private Button btnStop;
    
    @FXML
    private ColorPicker teamColorPicker;
    
    private final SimpleBooleanProperty recording = new SimpleBooleanProperty(false);
    private final SimpleStringProperty recordingFile = new SimpleStringProperty("<not set>");
    private final FileChooser fileChooser = new FileChooser();
    
    @Override
    protected boolean isSelfController() {
        return true;
    }

    @Override
    public URL getFXMLRessource() {
        return getClass().getResource("TeamCommViewerFx.fxml");
    }

    @Override
    public void afterInit() {
        // connect ui updates
        ownPort.disableProperty().bind(btnListen.selectedProperty());
        oppPort.disableProperty().bind(btnListen.selectedProperty());
        ownPort.setTooltip(new Tooltip("Own team port number"));
        oppPort.setTooltip(new Tooltip("Opponent team port number"));
        
        btnListen.setOnAction(new ListenButtonEventHandler());
        
        // prepare status table
        Label l = new Label();
        l.textProperty().bind(Bindings.when(btnListen.selectedProperty()).then("No data available ...").otherwise("Not connected ..."));
        l.setStyle("-fx-font-size: 16px; -fx-alignment: CENTER; -fx-border-style: dashed; -fx-border-width: 2px; -fx-background-color: lightgrey; -fx-pref-width: 400px; -fx-pref-height: 200px;");
        statusTable.setPlaceholder(l);
        statusTable.setItems(robots);
        
        // available columns
        Column[] cols = {
            new Column<> ("#TN",            "teamNum", (p) -> new ColoredTableCell()),
            new Column<> ("#PN",             "playerNum", null),
            new Column<> ("IP",              "ipAddress", null),
            new Column<> ("msg/s",           "msgPerSecond", (p) -> new PingTableCell()),
            new Column<> ("BallAge (s)",     "ballAge", null),
            new Column<> ("State",           "fallen", (p) -> new StateTableCell()),
            new Column<> ("Temperature",     "temperature", (p) -> new TemperatureTableCell()),
            new Column<> ("CPU-Temperature", "cpuTemperature", (p) -> new TemperatureTableCell()),
            new Column<> ("Battery",         "batteryCharge", (p) -> new BatteryTableCell()),
            new Column<> ("TimeToBall",      "timeToBall", null),
            new Column<> ("wantToBeStriker", "wantsToBeStriker", (p) -> new CheckBoxTableCell<>()),
            new Column<> ("wasStriker",      "wasStriker", (p) -> new CheckBoxTableCell<>()),
            new Column<> ("isPenalized",     "isPenalized", (p) -> new CheckBoxTableCell<>()),
            new Column<> ("whistleDetected", "whistleDetected", (p) -> new CheckBoxTableCell<>()),
            new Column<> ("teamBall",        "teamBall", (p) -> new Vector2DTableCell()),
            new Column<> ("show on field",   "showOnField", (p) -> new CheckBoxTableCell<>()),
            new Column<> ("Connect to",      "isConnected", (p) -> new ButtonTableCell()),
        };
        
        // add columns to table
        statusTable.getColumns().addAll(cols);
        
        // init filechooser
        fileChooser.setTitle("Log file location");
        fileChooser.getExtensionFilters().addAll(
            new ExtensionFilter("Log Files", "*.log"),
            new ExtensionFilter("Text Files", "*.txt"),
            new ExtensionFilter("All Files", "*.*")
        );
        fileChooser.setInitialFileName((new SimpleDateFormat("yyyy-MM-dd")).format(new Date())+"_TeamComm.log");
        fileChooser.setInitialDirectory(new File(System.getProperty("user.home")));
        
        btnRecord.setGraphic(new ImageView(new Image("/de/naoth/rc/res/media-record.png")));
        btnRecord.setOnAction((evt) -> {
            // record-button got pressed
            if(btnRecord.isSelected()) {
                // log file already set; just enable logging
                if(this.log.isActive()) {
                    this.log.resumeLogging();
                    btnRecord.setSelected(true);
                } else {
                    // select log file and enable logging!
                     File selectedFile = fileChooser.showSaveDialog(null);
                     if (selectedFile != null && selectedFile.isFile()) {
                         fileChooser.setInitialDirectory(selectedFile.getParentFile());
                        File dfile = (selectedFile.getName().lastIndexOf(".") == -1) ? new File(selectedFile+".log") : selectedFile;

                        // set log file, start logging and update ui
                        if(this.log.setLogFile(dfile)) {
                            this.log.startLogging();
                            btnRecord.setSelected(true);
                            recordingFile.set("Recording to " + dfile.getName());
                            recording.set(true);
                        }
                     } else {
                         btnRecord.setSelected(false);
                     }
                }
            } else {
                // release button
                this.log.pauseLogging();
            }
        });
        btnRecord.setTooltip(new Tooltip());
        btnRecord.tooltipProperty().get().textProperty().bind(Bindings.when(recording).then(recordingFile).otherwise("Start new recording"));
        btnStop.setGraphic(new ImageView(new Image("/toolbarButtonGraphics/media/Stop24.gif", 16, 16, true, true)));
        btnStop.setTooltip(new Tooltip("Stops recording and closes log file."));
        btnStop.disableProperty().bind(recording.not());
        btnStop.setOnAction((evt) -> {
            // stop log file recording, flush and close logfile
            this.log.stopLogging();
            // ... update UI
            recordingFile.set("<not set>");
            recording.set(false);
            btnRecord.setSelected(false);
        });
        
        // init team color picker
        teamColorPicker.setTooltip(new Tooltip("Change color of teams."));
        teamColorPicker.setOnAction((t) -> {
            javafx.scene.paint.Color fx = teamColorPicker.getValue();
            Color c = new Color((float)fx.getRed(), (float)fx.getGreen(), (float)fx.getBlue(), (float)fx.getOpacity());
            byte tn = statusTable.getSelectionModel().getSelectedItem().teamNum;
            robots.stream().filter((rs) -> { return rs.teamNum == tn;}).forEach((rs) -> { rs.robotColor = c; });
        });
        statusTable.getSelectionModel().selectedItemProperty().addListener((ov, o, n) -> {
            if(n != null) {
                float[] c = n.robotColor.getRGBComponents(null);
                Platform.runLater(() -> {
                    teamColorPicker.setDisable(false);
                    teamColorPicker.setValue(javafx.scene.paint.Color.color(c[0], c[1], c[2], c[3]));
                });
            } else {
                Platform.runLater(() -> { teamColorPicker.setDisable(true); });
            }
        });
    } // afterInit()
    
    /**
     * The handler for the listen button.
     * Start/Stops all listener and updates the ui accordingly.
     */
    private class ListenButtonEventHandler implements EventHandler
    {
        @Override
        public void handle(Event ev) {
            if(btnListen.isSelected()) {
                int port_own = (int) ownPort.getValue();
                int port_opp = (int) oppPort.getValue();
                // establish connection
                if(!connectListener(listenerOwn, port_own) && !connectListener(listenerOpponent, port_opp)) {
                    Platform.runLater(() -> { btnListen.setSelected(false); });
                } else {
                    // listen to TeamCommMessages
                    Plugin.teamcommManager.addListener(teamcommListener);
                    // start/schedule FX UI-updater
                    timerUpdateStatusTable = new Timer();
                    timerUpdateStatusTable.scheduleAtFixedRate(new TimerTask() {
                        @Override
                        public void run() {
                            Platform.runLater(() -> { statusTable.refresh(); });
                        }
                    }, 100, 250);
                    // start/schedule robots field drawer
                    timerDrawRobots = new Timer();
                    timerDrawRobots.scheduleAtFixedRate(new TimerTask() {
                        @Override
                        public void run() {
                            DrawingCollection drawings = new DrawingCollection();
                            // if enabled, draw robots on the FieldViewer otherwise not
                            robots.stream()
                                  .filter((robotStatus) -> (robotStatus.showOnField))
                                  .forEach((robotStatus) -> {
                                        robotStatus.lastMessage.draw(drawings, robotStatus.robotColor, robotStatus.isOpponent);
                                  }); 
                            Plugin.drawingEventManager.fireDrawingEvent(drawings, this);
                        }
                    }, 100, 33);
                }
            } else {
                // disconnect
                try {
                    listenerOwn.disconnect();
                    listenerOpponent.disconnect();
                } catch (IOException | InterruptedException ex) {
                    Helper.handleException("Error disconnecting to ports", ex);
                }
                // remove TeamCommMessage listener
                Plugin.teamcommManager.removeListener(teamcommListener);
                // stop UI-updater
                if(timerDrawRobots != null) {
                    timerDrawRobots.cancel();
                    timerDrawRobots.purge();
                    timerDrawRobots = null;
                }
                if(timerUpdateStatusTable != null) {
                    timerUpdateStatusTable.cancel();
                    timerUpdateStatusTable.purge();
                    timerUpdateStatusTable = null;
                }
                // update fx ui
                Platform.runLater(() -> {
                    robots.clear();
                    statusTable.refresh();
                });
            } // end if/else
        } // end handle()

        /**
         * Connects the RobotTeamCommListener listener to the given port.
         * 
         * @param listener RobotTeamCommListener
         * @param port the port where the listener should connect to
         * @return true on success, false otherwise
         */
        private boolean connectListener(RobotTeamCommListener listener, int port) {
            if(port != 0) {
                try {
                    listener.connect(port);
                    return true;
                } catch (IOException | InterruptedException ex) {
                    Helper.handleException("Error connecting to ports", ex);
                }
            }
            return false;
        }
    } // end ListenButtonEventHandler
    
    private class Column<T> extends TableColumn {
        public Field field;

        public Column(String name, String field, Callback cellFactory) {
            super(name);
            try {
                this.field = RobotStatus.class.getDeclaredField(field);
            } catch (NoSuchFieldException | SecurityException ex) {
                // should never happen
                Logger.getLogger(TeamCommViewerFx.class.getName()).log(Level.SEVERE, null, ex);
            }
            setCellValueFactory((p) -> {
                try {
                    return new ReadOnlyObjectWrapper(Column.this.field.get(((CellDataFeatures<RobotStatus, T>) p).getValue()));
                } catch (IllegalArgumentException | IllegalAccessException ex) {
                    // should never happen
                    Logger.getLogger(TeamCommViewerFx.class.getName()).log(Level.SEVERE, null, ex);
                }
                return null;
            });
            if(cellFactory != null) {
                setCellFactory(cellFactory);
            }
        }
    }
    
    private class TemperatureTableCell extends TableCell {
        @Override
        protected void updateItem(Object item, boolean empty) {
            super.updateItem(item, empty);
            if(!empty && item != null) {
                float temp = (float) item;
                if (temp >= 75.0) { // 75 °C
                    java.awt.Color c = RobotStatus.COLOR_DANGER;
                    setStyle("-fx-background-color: rgba("+c.getRed()+","+c.getGreen()+","+c.getBlue()+","+c.getTransparency()+");");
                } else if (temp >= 60.0) { // 60 °C
                    java.awt.Color c = RobotStatus.COLOR_WARNING;
                    setStyle("-fx-background-color: rgba("+c.getRed()+","+c.getGreen()+","+c.getBlue()+","+c.getTransparency()+");");
                } else {
                    setStyle("");
                }
                setText(temp == -1 ? "?" : String.format(" %3.1f °C", temp));
            }
        }
    }
    
    private class BatteryTableCell extends TableCell {
        @Override
        protected void updateItem(Object item, boolean empty) {
            super.updateItem(item, empty);
            if(!empty && item != null) {
                double bat = ((float) item)/100.0;
                
                java.awt.Color c;
                if (bat <= 0.3) {
                    c = RobotStatus.COLOR_DANGER;
                } else if (bat <= 0.6) {
                    c = RobotStatus.COLOR_WARNING;
                } else {
                    c = RobotStatus.COLOR_INFO;
                }
//                setStyle("-fx-background-color: red|yellow|green");
                setStyle("-fx-background-color: rgba("+c.getRed()+","+c.getGreen()+","+c.getBlue()+","+c.getTransparency()+");");
                setText(bat == -1 ? "?" : String.format("%3.1f%%", bat));
            }
        }
    }
    
    private class PingTableCell extends TableCell {
        @Override
        protected void updateItem(Object item, boolean empty) {
            super.updateItem(item, empty);
            if(!empty && item != null) {
                setText(String.format("%4.2f", item));
            }
        }
    }
    
    private class Vector2DTableCell extends TableCell {
        @Override
        protected void updateItem(Object item, boolean empty) {
            super.updateItem(item, empty);
            if(!empty && item != null) {
                Vector2D vec = (Vector2D) item;
                setText(String.format("%4.2f / %4.2f", vec.x, vec.y));
            }
        }
    }
    
    private class ColoredTableCell extends TableCell {
        @Override
        protected void updateItem(Object item, boolean empty) {
            super.updateItem(item, empty);
            if(!empty && item != null) {
                RobotStatus rs = (RobotStatus) this.getTableRow().getItem();
                if(rs != null) {
                    setStyle("-fx-background-color: rgba("+rs.robotColor.getRed()+","+rs.robotColor.getGreen()+","+rs.robotColor.getBlue()+","+rs.robotColor.getTransparency()+");");
                }
                setText(item.toString());
            }
        }
    }
    
    private class StateTableCell extends TableCell {
        @Override
        protected void updateItem(Object item, boolean empty) {
            super.updateItem(item, empty);
            if(!empty && item != null) {
                RobotStatus rs = (RobotStatus) this.getTableRow().getItem();
                if(rs != null) {
                    setText(rs.isDead ? "DEAD" : (rs.fallen == 1 ? "FALLEN" : "NOT FALLEN"));
                }
            }
        }
    }
    
    private class ButtonTableCell extends TableCell
    {
        final Button btn = new Button("Connect");

        public ButtonTableCell() {
            super();
            btn.setOnAction(event -> {
                RobotStatus rs = (RobotStatus) this.getTableRow().getItem();
                rs.connect();
            });
            // make button smaller
            btn.setStyle("-fx-font-size: 10px; -fx-padding: .1em 0.5em .1em 0.5em;");
            
        }
        
        @Override
        protected void updateItem(Object item, boolean empty) {
            super.updateItem(item, empty);
            if(!empty) {
                btn.setDisable((boolean)item);
                setGraphic(btn);
            } else {
                setGraphic(null);
            }
            setText(null);
        }
    }
    
    /**
     * Waits for new TeamCommMessages and writes them to the "message-buffer".
     */
    public class TeamCommMessageListener implements de.naoth.rc.components.teamcomm.TeamCommListener {
        @Override
        public void newTeamCommMessages(List<TeamCommMessage> messages) {
            if (!messages.isEmpty()) {
                for (TeamCommMessage message : messages) {
                    // determine source address
                    String address = message.address == null ? message.message.teamNum+".0.0."+message.message.playerNum : message.address;
                    
                    // get robot with address from robots list
                    RobotStatus robotStatus = robots.stream().filter((t) -> { return t.ipAddress.equals(address); }).findFirst().orElse(null);
                    // if not found - add robot
                    if (robotStatus == null) {
                        robotStatus = new RobotStatus(Plugin.parent.getMessageServer(), address, message.isOpponent());
                        robotStatus.robotColor = message.isOpponent() ? Color.RED : Color.BLUE;
                        robots.add(robotStatus);
                    }
                    // updates the robotStatus
                    robotStatus.updateStatus(message.timestamp, message.message);
                } // end for
            } // end if
        } // end newTeamCommMessages()
    } // TeamCommMessageListener
}
