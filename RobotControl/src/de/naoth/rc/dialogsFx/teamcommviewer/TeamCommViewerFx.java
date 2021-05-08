package de.naoth.rc.dialogsFx.teamcommviewer;

import de.naoth.rc.Helper;
import de.naoth.rc.RobotControl;
import de.naoth.rc.components.gamecontroller.GameControllerEventListener;
import de.naoth.rc.components.gamecontroller.GameControllerManager;
import de.naoth.rc.components.gamecontroller.event.GameControllerConnectionEvent;
import de.naoth.rc.components.gamecontroller.event.GameControllerDataEvent;
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
import java.io.File;
import java.io.IOException;
import java.lang.reflect.Field;
import java.net.URL;
import java.text.NumberFormat;
import java.text.ParsePosition;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;
import java.util.function.UnaryOperator;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.stream.Collectors;
import javafx.animation.Animation;
import javafx.animation.Transition;
import javafx.application.Platform;
import javafx.beans.binding.Bindings;
import javafx.beans.property.SimpleBooleanProperty;
import javafx.beans.property.SimpleStringProperty;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.geometry.Insets;
import javafx.scene.Node;
import javafx.scene.control.Button;
import javafx.scene.control.ColorPicker;
import javafx.scene.control.Label;
import javafx.scene.control.Spinner;
import javafx.scene.control.TableCell;
import javafx.scene.control.TableColumn;
import javafx.scene.control.TableView;
import javafx.scene.control.TextField;
import javafx.scene.control.TextFormatter;
import javafx.scene.control.ToggleButton;
import javafx.scene.control.Tooltip;
import javafx.scene.control.cell.CheckBoxTableCell;
import javafx.scene.control.cell.PropertyValueFactory;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.layout.Background;
import javafx.scene.layout.BackgroundFill;
import javafx.scene.layout.CornerRadii;
import javafx.stage.FileChooser;
import javafx.stage.FileChooser.ExtensionFilter;
import javafx.util.Callback;
import javafx.util.Duration;
import javafx.util.converter.IntegerStringConverter;
import javax.swing.JFrame;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class TeamCommViewerFx extends AbstractJFXDialog
{
    @RCDialog(category = RCDialog.Category.Team, name = "TeamCommViewer (FX)")
    @PluginImplementation
    public static class Plugin extends DialogPlugin<TeamCommViewerFx> {
        @InjectPlugin
        public static RobotControl parent;
        @InjectPlugin
        public static DrawingEventManager drawingEventManager;
        @InjectPlugin
        public static TeamCommManager teamcommManager;
        @InjectPlugin
        public static GameControllerManager gamecontroller;
        
        @Override
        public String getDisplayName() { return "TeamCommViewer (FX)"; }
    }

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
    private Label statusTablePlaceholder;
    
    @FXML
    private Label multipleNumberWarning;
    
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
    
    @FXML
    private Label gamecontrollerState;
    
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

    /**
     * Returns the global theme.
     * @return path to the global theme stylesheet
     */
    @Override
    protected String getTheme() {
        return Plugin.parent.getTheme();
    }
    
    @Override
    public void afterInit() {
        initStatusTable();
        
        // setup port Bindings
        ownPort.disableProperty().bind(btnListen.selectedProperty());
        oppPort.disableProperty().bind(btnListen.selectedProperty());
        // set 'safer' string to integer converter for port spinner
        ownPort.getValueFactory().setConverter(new IntegerStringConverterSafe());
        oppPort.getValueFactory().setConverter(new IntegerStringConverterSafe());
        // port submission/validation
        // hook in a formatter with the same properties as the factory
        TextFormatter ownPortFormatter = new TextFormatter(ownPort.getValueFactory().getConverter(), ownPort.getValueFactory().getValue(), new PortInputValidator(ownPort));
        ownPort.getEditor().setTextFormatter(ownPortFormatter);
        ownPort.getValueFactory().valueProperty().bindBidirectional(ownPortFormatter.valueProperty()); // bidi-bind the values
        TextFormatter oppPortFormatter = new TextFormatter(oppPort.getValueFactory().getConverter(), oppPort.getValueFactory().getValue(), new PortInputValidator(oppPort));
        oppPort.getEditor().setTextFormatter(oppPortFormatter);
        oppPort.getValueFactory().valueProperty().bindBidirectional(oppPortFormatter.valueProperty()); // bidi-bind the values
        // establish connection, when pressing return/enter
        ownPort.getEditor().setOnAction((t) -> {
            ownPort.getEditor().commitValue();
            btnListen.fire();
        });
        oppPort.getEditor().setOnAction((t) -> {
            oppPort.getEditor().commitValue();
            btnListen.fire();
        });
        
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
        btnRecord.tooltipProperty().get().textProperty().bind(Bindings.when(recording).then(recordingFile).otherwise("Start new teamcomm recording"));
        
        btnStop.setGraphic(new ImageView(new Image("/toolbarButtonGraphics/media/Stop24.gif", 16, 16, true, true)));
        btnStop.disableProperty().bind(recording.not());
        
        // init team color picker
        teamColorPicker.setOnAction((t) -> {
            int tn = statusTable.getSelectionModel().getSelectedItem().getTeamNum();
            Platform.runLater(() -> {
                robots.stream().filter((rs) -> { return rs.getTeamNum() == tn;}).forEach((rs) -> { rs.robotColor.set(teamColorPicker.getValue()); });
                statusTable.refresh();
            });
        });
        
        // on window close, stops teamcomm listener and logging thread
        ((JFrame)Plugin.parent).addWindowListener(new java.awt.event.WindowAdapter() {
            @Override
            public void windowClosing(java.awt.event.WindowEvent windowEvent) {
                stopLogging();
            }
        });
        
        // handle gamecontroller infos
        Plugin.gamecontroller.addGameListener(new GameControllerUpdater());
        gamecontrollerState.setTooltip(new Tooltip("GameController info"));
    } // afterInit()
    
    /**
     * Initializes the status table.
     * Sets the available columns, connects properties and sets some listeners.
     */
    private void initStatusTable() {
        // binding for displaying the tables placeholder label
        statusTablePlaceholder.textProperty().bind(Bindings.when(btnListen.selectedProperty()).then("No data available ...").otherwise("Not connected ..."));
        
        // available columns
        Column[] cols = {
            new Column<> ("#TN",            "teamNum",          (p) -> new ColoredTableCell()),
            new Column<> ("#PN",             "playerNum"),
            new Column<> ("IP",              "ipAddress"),
            new Column<> ("msg/s",           "msgPerSecond",    (p) -> new PingTableCell()),
            new Column<> ("BallAge (s)",     "ballAge"),
            new Column<> ("State",           "fallen",          (p) -> new StateTableCell()),
            new Column<> ("Role",            "robotRoleStatic"),
            new Column<> ("dyn. Role",       "robotRoleDynamic"),
            new Column<> ("Temperature",     "temperature",     (p) -> new TemperatureTableCell()),
            new Column<> ("CPU-Temperature", "cpuTemperature",  (p) -> new TemperatureTableCell()),
            new Column<> ("Battery",         "batteryCharge",   (p) -> new BatteryTableCell()),
            new Column<> ("TimeToBall",      "timeToBall"),
            new Column<> ("wantToBeStriker", "wantsToBeStriker",(p) -> new CheckBoxTableCell<>()),
            new Column<> ("wasStriker",      "wasStriker",      (p) -> new CheckBoxTableCell<>()),
            new Column<> ("RobotState",      "robotState"),
            new Column<> ("whistleDetected", "whistleDetected", (p) -> new CheckBoxTableCell<>()),
            new Column<> ("teamBall",        "teamBall",        (p) -> new Vector2DTableCell()),
            new Column<> ("show on field",   "showOnField",     (p) -> new CheckBoxTableCell<>(), true),
            new Column<> ("Connect to",      "isConnected",     (p) -> new ButtonTableCell()),
        };
        
        // add columns to table
        statusTable.getColumns().addAll(cols);
        
        // in order to make the columns editable, the tableview has to be editable too!
        statusTable.setEditable(true);
        
        // listener for changing the teamcolor of a selected robot
        statusTable.getSelectionModel().selectedItemProperty().addListener((ov, o, n) -> {
//            System.out.println("selected: " + n);
            if(n != null) {
                Platform.runLater(() -> {
                    teamColorPicker.setDisable(false);
                    teamColorPicker.setValue(n.robotColor.get());
                    // NOTE: sometimes the colopicker is displayed as "disabled" despite its not?!
//                    System.out.println("disabled: " + teamColorPicker.isDisabled());
                });
            } else {
                Platform.runLater(() -> { teamColorPicker.setDisable(true); });
            }
        });
        
        // (empty) table data
        statusTable.setItems(robots);
        
        // restores column configuration
        String columnConfigStr = Plugin.parent.getConfig().getProperty(this.getClass().getName()+".ColumnConfig", "");
        List<String> columnConfig = columnConfigStr.isEmpty() ? null : Arrays.asList(columnConfigStr.split("\\|"));
        if(columnConfig != null) {
            statusTable.getColumns().forEach((column) -> {
                if(!columnConfig.contains(column.getText())) {
                    column.setVisible(false);
                }
            });
        }
        
        // adds listener for column visiblity changes
        statusTable.getColumns().forEach((c) -> {
            c.visibleProperty().addListener((o) -> {
                saveColumnConfiguration();
            });
        });
    } // initStatusTable
    
    /**
     * On dialog close, stops teamcomm listener and logging thread
     */
    @Override
    public void dispose() {
        stopLogging();
    }

    /**
     * Saves the currently visible columns of the status table.
     */
    private void saveColumnConfiguration() {
        ArrayList<String> items = new ArrayList<>();
        statusTable.getColumns().forEach((column) -> {
            if(column.isVisible()) {
                items.add(column.getText());
            }
        });
        Plugin.parent.getConfig().setProperty(this.getClass().getName()+".ColumnConfig", items.stream().collect(Collectors.joining("|")));
    }
    
    /**
     * Checks, whether the available playernumbers are unique.
     * If not, a warning with the ip adresses of the robots with the same playernumber is shown.
     */
    private void checkPlayerNumbers() {
        String warningText = "";
        HashMap<String, RobotStatus> numbers = new HashMap<>();
        for (RobotStatus status : robots) {
            String key = status.getTeamNum() + "_" + status.getPlayerNum();
            if(!numbers.containsKey(key)) {
                numbers.put(key, status);
            } else {
                warningText += "\t" + status.getPlayerNum() + " -> " + status.getIpAddress() + " / " + numbers.get(key).getIpAddress() + "\n";
            }
        }
        if(!warningText.isEmpty()) {
            multipleNumberWarning.setText("The following robots have the same player number:\n" + warningText);
            multipleNumberWarning.setVisible(true);
        } else {
            multipleNumberWarning.setVisible(false);
        }
    }
    
    /**
     * Stops the teamcomm listener and logging thread - if currently running.
     */
    private void stopLogging() {
        // stop listener threads!
        try {
            listenerOwn.disconnect();
        } catch (IOException | InterruptedException ex) {}
        try {
            listenerOpponent.disconnect();
        } catch (IOException | InterruptedException ex) {}
        
        this.log.stopLogging();
    }

    /**
     * The handler for the listen button.
     * Start/Stops all listener and updates the ui accordingly.
     */
    @FXML
    private void actionListen() {
        if(btnListen.isSelected()) {
            int port_own = (int) ownPort.getValue();
            int port_opp = (int) oppPort.getValue();
            // establish connection
            if(connectPortListener(listenerOwn, port_own) && connectPortListener(listenerOpponent, port_opp)) {
                // listen to TeamCommMessages
                Plugin.teamcommManager.addListener(teamcommListener);
                // start/schedule robots field drawer
                timerDrawRobots = new Timer();
                timerDrawRobots.scheduleAtFixedRate(new TimerTask() {
                    @Override
                    public void run() {
                        DrawingCollection drawings = new DrawingCollection();
                        // if enabled, draw robots on the FieldViewer otherwise not
                        robots.stream()
                              .filter((robotStatus) -> (robotStatus.getShowOnField()))
                              .forEach((robotStatus) -> {
                                  if(robotStatus.lastMessage != null) {
                                    robotStatus.lastMessage.draw(drawings, robotStatus.getRobotColorAwt(), robotStatus.isOpponent);
                                  }
                              }); 
                        Plugin.drawingEventManager.fireDrawingEvent(drawings, this);
                    }
                }, 100, 33);
            } else {
                Platform.runLater(() -> { btnListen.setSelected(false); });
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
            // update fx ui
            Platform.runLater(() -> {
                multipleNumberWarning.setVisible(false);
                robots.clear();
                statusTable.refresh();
            });
        } // end if/else
    } // actionListen()
    
    /**
     * Connects the RobotTeamCommListener listener to the given port.
     * 
     * @param listener RobotTeamCommListener
     * @param port the port where the listener should connect to
     * @return true on success, false otherwise
     */
    private boolean connectPortListener(RobotTeamCommListener listener, int port) {
        if(port != 0) {
            try {
                listener.connect(port);
            } catch (IOException | InterruptedException | IllegalArgumentException ex) {
                Helper.handleException("Error connecting to ports", ex);
                return false;
            }
        }
        return true;
    } // connectPortListener()
    
    /**
     * Handles the record button (click) event.
     * Starts/pauses recording of teamcomm log file.
     */
    @FXML
    private void actionRecord() {
        // record-button got pressed
        if(btnRecord.isSelected()) {
            // log file already set; just enable logging
            if(this.log.isActive()) {
                this.log.resumeLogging();
                btnRecord.setSelected(true);
            } else {
                // select log file and enable logging!
                 File selectedFile = fileChooser.showSaveDialog(null);
                 if (selectedFile != null) {
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
    }
    
    /**
     * Stops the log file recording, flush and close logfile.
     */
    @FXML
    private void actionStop() {
        this.log.stopLogging();
        // ... update UI
        recordingFile.set("<not set>");
        recording.set(false);
        btnRecord.setSelected(false);
    }
    
    /**
     * Helper class.
     * Thereby the column definition can be realized much shorter and compact. The column just needs
     * to know the title of the column, the RobotStatus attribute (field) which should be shown, a
     * object how the column is displayed and whether or not the column should be editable (or not).
     * The RobotStatus field is accessed via reflection - the developer has to make sure, that the 
     * equivalent field exits!
     * 
     * @param <T> 
     */
    private class Column<T> extends TableColumn
    {
        private Field field;
        
        public Column(String name, String field) {
            this(name, field, null);
        }

        public Column(String name, String field, Callback cellFactory) {
            this(name, field, cellFactory, false);
        }

        public Column(String name, String field, Callback cellFactory, boolean editable) {
            super(name);
            try {
                this.field = RobotStatus.class.getDeclaredField(field);
            } catch (NoSuchFieldException | SecurityException ex) {
                // should never happen
                Logger.getLogger(TeamCommViewerFx.class.getName()).log(Level.SEVERE, null, ex);
                this.field = null;
            }
            setCellValueFactory(new PropertyValueFactory(field));
            if(cellFactory != null) {
                setCellFactory(cellFactory);
            }
            setEditable(editable);
        }
    }
    
    /**
     * Changes the background color based on the contained value, reflecting the temperature of the player.
     * The value is also 'properly' formatted.
     */
    private class TemperatureTableCell extends TableCell {
        @Override
        protected void updateItem(Object item, boolean empty) {
            super.updateItem(item, empty);
            if(!empty && item != null) {
                double temp = (double) item;
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
    
    /**
     * Changes the background color based on the contained value, reflecting the battery status of the player.
     * The value is also 'properly' formatted.
     */
    private class BatteryTableCell extends TableCell {
        @Override
        protected void updateItem(Object item, boolean empty) {
            super.updateItem(item, empty);
            if(!empty && item != null) {
                double bat = ((double) item)/100.0;
                
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
                setText(bat == -1 ? "?" : String.format("%3.1f%%", bat*100.0));
            }
        }
    }
    
    /**
     * Shows the received messages per second of the player or "DEAD" if there was no messages received
     * for a certain amount of time.
     */
    private class PingTableCell extends TableCell {
        @Override
        protected void updateItem(Object item, boolean empty) {
            super.updateItem(item, empty);
            if(!empty && item != null) {
                setText((double)item == 0.0 ?"DEAD":String.format("%4.2f", item));
            }
        }
    }
    
    /**
     * Shows the content of a Vedtor2D. (e.g. teamball)
     */
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
    
    /**
     * Shows a colored background cell, based on the players (team) color.
     * Therefor a color binding is created once and changes were immediatly applied.
     */
    private class ColoredTableCell extends TableCell {
        HashMap<Integer, Background> teamColors = new HashMap<>();
        
        @Override
        protected void updateItem(Object item, boolean empty) {
            super.updateItem(item, empty);
            if(!empty && item != null) {
                RobotStatus rs = (RobotStatus) this.getTableRow().getItem();
                if(rs != null) {
                    // save team color, if not set
                    if(!teamColors.containsKey(rs.teamNumProperty().get())) {
                        teamColors.put(rs.teamNumProperty().get(), new Background(new BackgroundFill(rs.robotColor.get(), CornerRadii.EMPTY, Insets.EMPTY)));
                    }
                    // update team color, if (eg.) ordering changed
                    setBackground(teamColors.get(rs.teamNumProperty().get()));
                }
                setText(item.toString());
            }
        }
    }
    
    /**
     * Shows the state of the player based on the contained boolean value. ("Fallen"/"Not Fallen")
     */
    private class StateTableCell extends TableCell {
        @Override
        protected void updateItem(Object item, boolean empty) {
            super.updateItem(item, empty);
            if(!empty && item != null) {
                setText((Boolean)item ? "FALLEN" : "NOT FALLEN");
            }
        }
    }
    
    /**
     * Shows a 'connect' button for each player.
     */
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
                Platform.runLater(() -> {
                    for (TeamCommMessage message : messages) {
                        // determine source address
                        String address = message.address == null ? message.message.teamNum+".0.0."+message.message.playerNum : message.address;

                        // get robot with address from robots list
                        RobotStatus robotStatus = robots.stream().filter((t) -> { return t.getIpAddress().equals(address); }).findFirst().orElse(null);
                        // if not found - add robot
                        if (robotStatus == null) {
                            robotStatus = new RobotStatus(Plugin.parent.getMessageServer(), address, message.isOpponent(), true);
                            robotStatus.robotColor.set(message.isOpponent() ? javafx.scene.paint.Color.RED : javafx.scene.paint.Color.BLUE);
                            robotStatus.playerNumProperty().addListener((o) -> { checkPlayerNumbers(); }); // re-check player numbers if the number changes
                            robotStatus.updateStatus(message.timestamp, message.message); // must be updated before adding to list!
                            robots.add(robotStatus);
                            // after adding robot to list, check if playernumber is already used!
                            checkPlayerNumbers();
                        } else {
                            // updates the robotStatus
                            robotStatus.updateStatus(message.timestamp, message.message);
                        }
                    } // end for
                });
            } // end if
        } // end newTeamCommMessages()
    } // TeamCommMessageListener

    /**
     * Validator for checking the inserted characters.
     * If validation fails, an error animation is played.
     */
    class PortInputValidator implements UnaryOperator<TextFormatter.Change>
    {
        private final Animation animation;
        
        public PortInputValidator(TextField tf) {
            animation = new PortErrorAnimation(tf);
        }
        
        public PortInputValidator(Spinner s) {
            animation = new PortErrorAnimation(s.getEditor());
        }

        @Override
        public TextFormatter.Change apply(TextFormatter.Change c) {
            if (c.isContentChange()) {
                ParsePosition parsePosition = new ParsePosition(0);
                // NumberFormat evaluates the beginning of the text
                NumberFormat.getIntegerInstance().parse(c.getControlNewText(), parsePosition);
                // reject parsing the complete text failed
                if (!c.getControlNewText().isEmpty() && (parsePosition.getIndex() == 0 || parsePosition.getIndex() < c.getControlNewText().length())) {
                    Platform.runLater(() -> { animation.play(); });
                    return null;
                }
            }
            return c;
        }
    }
    
    /**
     * Animation for flashing the background of a port input field.
     * From red to white in 300ms.
     */
    class PortErrorAnimation extends Transition
    {
        javafx.scene.paint.Color from = javafx.scene.paint.Color.RED;
        javafx.scene.paint.Color to = javafx.scene.paint.Color.WHITE;
        Node node;
        
        public PortErrorAnimation(Node n) {
            setCycleDuration(Duration.millis(300));
            node = n;
        }
        
        @Override
        protected void interpolate(double frac) {
            javafx.scene.paint.Color c = from.interpolate(to, frac);
            node.setStyle("-fx-control-inner-background: rgba(" + ((int) (c.getRed() * 255)) + "," + ((int) (c.getGreen() * 255)) + "," + ((int) (c.getBlue() * 255)) + ");");
        }
    } // PortErrorAnimation
    
    /**
     * Safer string to integer converter.
     * On empty strings the converter returns zero.
     */
    class IntegerStringConverterSafe extends IntegerStringConverter
    {
        @Override
        public Integer fromString(String value) {
            return value.isEmpty() ? 0 : super.fromString(value);
        }
    }
    
    /**
     * Listener class for handling gamecontroller events.
     */
    class GameControllerUpdater implements GameControllerEventListener
    {
        @Override
        public void connectionChanged(GameControllerConnectionEvent e) {
            // remove shown gamecontroller infos
            if(e.state == GameControllerConnectionEvent.State.DISCONNECTED || e.state == GameControllerConnectionEvent.State.TIMEOUT) {
                setLabelText("");
            }
        }

        @Override
        public void newGameData(GameControllerDataEvent e) {
            // update gamecontroller infos
            String display = "";
            switch(e.data.gameState) {
                case 0: display += "INITIAL"; break;
                case 1: display += "READY: " + e.data.secondaryTime; break;
                case 2: display += "SET"; break;
                case 3: display += "PLAYING: " + formatSeconds(e.data.secsRemaining); break;
                case 4: display += "FINISHED: " + formatSeconds(e.data.secondaryTime); break;
                default: display += "UNKNOWN"; break;
            }
            
            // determine the team who as kickoff/freekick
            int kickingTeam = e.data.kickingTeam + 10000; // use the port for comparison
            String fk_team = kickingTeam == (int)ownPort.getValue() ? "OWN" : "OPP";
            
            switch(e.data.setPlay) {
                case 1: display += "; GOAL_FREE_KICK_"    + fk_team + ": " + e.data.secondaryTime; break;
                case 2: display += "; PUSHING_FREE_KICK_" + fk_team + ": " + e.data.secondaryTime; break;
            }
            
            setLabelText(display);
        }
        
        /**
         * Sets the icon of the gamecontroller label.
         * 
         * @param icon 
         */
        private void setLabelIcon(String icon) {
            Platform.runLater(() -> {
                gamecontrollerState.setGraphic(new ImageView(new Image(icon)));
            });
        }
        
        /**
         * Sets the string of the gamecontroller label.
         * 
         * @param txt the string to display as gamecontroller info
         */
        private void setLabelText(String txt) {
            Platform.runLater(() -> {
                gamecontrollerState.setText(txt);
            });
        }
        
        /**
         * Formats the given seconds to 'mm:ss'.
         * 
         * @param seconds the seconds which should be formatted.
         * @return the formated seconds as string
         */
        private String formatSeconds(int seconds) {
            return String.format("%2d:%02d", seconds/60, seconds%60);
        }
    }
}
