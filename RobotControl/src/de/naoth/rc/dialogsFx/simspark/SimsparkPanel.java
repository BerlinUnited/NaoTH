package de.naoth.rc.dialogsFx.simspark;

import de.naoth.rc.components.simspark.SimsparkManager;
import de.naoth.rc.components.simspark.SimsparkSceneListener;
import de.naoth.rc.components.simspark.SimsparkState;
import de.naoth.rc.components.simspark.SimsparkStateListener;
import de.naoth.rc.components.simspark.commands.BallCommand;
import de.naoth.rc.components.simspark.commands.DropBallCommand;
import de.naoth.rc.components.simspark.commands.SimsparkCommand;
import de.naoth.rc.components.simspark.scene.SimsparkScene;
import de.naoth.rc.drawingmanager.DrawingEventManager;
import de.naoth.rc.drawings.DrawingCollection;
import java.util.Map;
import java.util.Timer;
import java.util.TimerTask;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.util.stream.Collectors;
import javafx.animation.KeyFrame;
import javafx.animation.Timeline;
import javafx.beans.property.SimpleStringProperty;
import javafx.beans.property.StringProperty;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.scene.control.ListView;
import javafx.scene.control.TableColumn;
import javafx.scene.control.TableView;
import javafx.scene.control.TextField;
import javafx.scene.control.cell.PropertyValueFactory;
import javafx.util.Duration;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class SimsparkPanel implements SimsparkStateListener, SimsparkSceneListener
{
    /** The controlling instance representing the connection to the robot. */
    private SimsparkManager simsparkManager;
    private DrawingEventManager drawingEventManager;

    private SimsparkScene scene;
    private Timer timerDrawScene;
    
    @FXML private TableView<SimsparkData> stateData;
    @FXML private ListView<String> commandHistory;
    @FXML private TextField commandBox;

    /**
     * Some pre-defined data entries.
     */
    private final ObservableList<SimsparkData> simsparkData = FXCollections.observableArrayList(
            new SimsparkData("time",                ""),
            new SimsparkData("play_mode",           ""),
            new SimsparkData("score_left",          ""),
            new SimsparkData("score_right",         ""),
            new SimsparkData("half",                ""),
            new SimsparkData("FieldLength",         ""),
            new SimsparkData("FieldWidth",          ""),
            new SimsparkData("FieldHeight",         ""),
            new SimsparkData("GoalWidth",           ""),
            new SimsparkData("GoalDepth",           ""),
            new SimsparkData("GoalHeight",          ""),
            new SimsparkData("BorderSize",          ""),
            new SimsparkData("FreeKickDistance",    ""),
            new SimsparkData("WaitBeforeKickOff",   ""),
            new SimsparkData("AgentRadius",         ""),
            new SimsparkData("BallRadius",          ""),
            new SimsparkData("BallMass",            ""),
            new SimsparkData("RuleGoalPauseTime",   ""),
            new SimsparkData("RuleKickInPauseTime", ""),
            new SimsparkData("RuleHalfTime",        ""),
            new SimsparkData("play_modes",          "")
    );

    /**
     * Map the pre-defined data entries, so it is easier to update the values.
     */
    private final Map<String, StringProperty> simsparkDataMapping = simsparkData.stream().collect(Collectors.toMap((v) -> v.name.get(), (v) -> v.value));

    /**
     * Gets called, after the FXML file was loaded.
     */
    @FXML
    private void initialize()
    {
        TableColumn col1 = (TableColumn)stateData.getColumns().get(0);
        col1.setCellValueFactory(new PropertyValueFactory<>("name"));
        
        TableColumn col2 = ((TableColumn)stateData.getColumns().get(1));
        col2.setCellValueFactory(new PropertyValueFactory<>("value"));

        stateData.setItems(simsparkData);
    }

    /**
     * Sets the simspark manager.
     *
     * @param manager the (global) simspark manager
     */
    public void setSimsparkManager(SimsparkManager manager)
    {
        if (simsparkManager != null)
        {
            simsparkManager.removeSimsparkStateListener(this);
            commandBox.disableProperty().unbind();
            commandHistory.disableProperty().unbind();
        }

        simsparkManager = manager;

        simsparkManager.addSimsparkStateListener(this);
        simsparkManager.addSimsparkSceneListener(this);
        commandBox.disableProperty().bind(simsparkManager.isConnected().not());
        commandHistory.disableProperty().bind(simsparkManager.isConnected().not());
    }

    public void setDrawingEventManager(DrawingEventManager manager)
    {
        drawingEventManager = manager;
    }

    public void enableFieldDrawings()
    {
        // start/schedule robots field drawer
        timerDrawScene = new Timer();
        timerDrawScene.scheduleAtFixedRate(new TimerTask()
        {
            @Override
            public void run()
            {
                if (scene != null)
                {
                    DrawingCollection drawings = new DrawingCollection();
                    scene.data.forEach((o) ->
                    {
                        o.draw(drawings);
                    });
                    drawingEventManager.fireDrawingEvent(drawings, this);
                }
            }
        }, 100, 33);
    }

    public void disableFieldDrawings()
    {
        if (timerDrawScene != null)
        {
            timerDrawScene.cancel();
            timerDrawScene.purge();
            timerDrawScene = null;
        }
    }

    /**
     * Receives new Simspark states (listener).
     * @param state new/updated simspark state
     */
    @Override
    public void newSimsparkData(SimsparkState state)
    {
        // update existing data or add new
        state.data.forEach((k, v) ->
        {
            if (simsparkDataMapping.containsKey(k))
            {
                simsparkDataMapping.get(k).set((String) v.toString());
            }
            else
            {
                SimsparkData newData = new SimsparkData(k, v.toString());
                simsparkData.add(newData);
                simsparkDataMapping.put(newData.getName(), newData.value);
            }
        });
    }

    @Override
    public void newSimsparkScene(SimsparkScene scene)
    {
        this.scene = scene;
    }
    
    @FXML
    private void fxCommandBox(ActionEvent ev)
    {
        String command = commandBox.getText();
        commandBox.clear();
        
        // TODO: Check commands
        commandHistory.itemsProperty().get().add(command);
        
        System.out.println("Command: " + command);
    }

    public class SimsparkData
    {
        private StringProperty name = new SimpleStringProperty(this, "");
        private StringProperty value = new SimpleStringProperty(this, "");

        public SimsparkData(String name, String data)
        {
            this.name.set(name);
            this.value.set(data);
        }

        public String getName()
        {
            return nameProperty().get();
        }

        public StringProperty nameProperty()
        {
            return name;
        }

        public String getValue()
        {
            return valueProperty().get();
        }

        public StringProperty valueProperty()
        {
            return value;
        }

        public void setValue(String value)
        {
            valueProperty().set(value);
        }
    }
}
