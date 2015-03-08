/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc.dialogs;

import de.naoth.rc.Helper;
import de.naoth.rc.RobotControl;
import de.naoth.rc.components.videoanalyzer.ParseLogController;
import de.naoth.rc.components.videoanalyzer.VideoPlayerController;
import de.naoth.rc.core.dialog.AbstractJFXDialog;
import de.naoth.rc.core.dialog.DialogPlugin;
import de.naoth.rc.core.manager.SwingCommandExecutor;
import de.naoth.rc.dataformats.LogFile;
import de.naoth.rc.dialogs.VideoAnalyzer.Plugin;
import de.naoth.rc.logmanager.LogDataFrame;
import de.naoth.rc.logmanager.LogFileEventManager;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.nio.file.Path;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Properties;
import java.util.TreeMap;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.beans.property.Property;
import javafx.beans.property.SimpleObjectProperty;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.fxml.FXMLLoader;
import javafx.scene.Node;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.ChoiceBox;
import javafx.scene.control.Slider;
import javafx.scene.control.TextField;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.Priority;
import javafx.scene.layout.VBox;
import javafx.scene.text.Text;
import javafx.stage.FileChooser;
import javafx.stage.Stage;
import javafx.util.StringConverter;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 *
 * @author thomas
 */
public class VideoAnalyzer extends AbstractJFXDialog
{

  @PluginImplementation
  public static class Plugin extends DialogPlugin<VideoAnalyzer>
  {

    @InjectPlugin
    public static RobotControl parent;
    @InjectPlugin
    public static SwingCommandExecutor commandExecutor;
    @InjectPlugin
    public static LogFileEventManager logFileEventManager;

    @Override
    public String getDisplayName()
    {
      return "Video Analyzer";
    }
  }

  public static class GameStateChange
  {

    public double time;
    public String state;

    @Override
    public String toString()
    {
      return state + " (at " + time + " seconds)";
    }
  }
  
  public final static String KEY_VIDEO_FILE = "video-file";
  public final static String KEY_SYNC_TIME_VIDEO = "sync-time-video";
  public final static String KEY_SYNC_TIME_LOG = "sync-time-log";

  private VideoPlayerController videoController;
    
  private TextField txtOffset;
  private ChoiceBox<GameStateChange> cbSyncLog;
  private Button btSyncPointVideo;
  private final FileChooser fileChooser = new FileChooser();

  private final Property<Double> timeOffset = new SimpleObjectProperty<>(0.0);
  private Double syncTimeLog;
  private Double syncTimeVideo;
  
  private Slider frameSlider;

  /**
   * Maps a second (fractioned) to a log frame number
   */
  private TreeMap<Double, Integer> time2LogFrame = new TreeMap<>();
  private TreeMap<Integer, Double> logFrame2Time;

  private LogFile logfile;
  
  private final Properties propLogfile = new Properties();
  
  private final ChangeListener<Number> frameChangeListener = new ChangeListener<Number>()
  {

    @Override
    public void changed(ObservableValue<? extends Number> observable, Number oldValue, Number newValue)
    {
      setVideoTimeFromLogFrame();
    }

  };

  public VideoAnalyzer()
  {
    timeOffset.addListener(new ChangeListener<Double>()
    {

      @Override
      public void changed(ObservableValue<? extends Double> observable, Double oldValue, Double newValue)
      {
        setLogFrameFromVideo();
      }
    });
  }

  public void setLogFrameFromVideo()
  {
    if (videoController != null)
    {
      double searchVal = videoController.getElapsedSeconds() + timeOffset.getValue();

      Map.Entry<Double, Integer> frame = time2LogFrame.floorEntry(searchVal);
      if (frame != null)
      {
        if(frameSlider.valueProperty().get() != frame.getValue())
        {
          sendLogFrame(frame.getValue());
          frameSlider.valueProperty().removeListener(frameChangeListener);
          frameSlider.valueProperty().set((double) frame.getValue());
          frameSlider.valueProperty().addListener(frameChangeListener);
        }
      }
    }
  }
  
  private void setVideoTimeFromLogFrame()
  {
    if(videoController != null && logFrame2Time != null)
    {
      int searchVal = frameSlider.valueProperty().intValue();
      Map.Entry<Integer, Double> time = logFrame2Time.floorEntry(searchVal);
      if (time != null)
      {
        videoController.setTime(time.getValue() - timeOffset.getValue());
      }
    }
  }

  private Node createControls()
  {
    Button btLoadLog = new Button("Log");
    btLoadLog.setOnAction(new EventHandler<ActionEvent>()
    {

      @Override
      public void handle(ActionEvent event)
      {
        File result = fileChooser.showOpenDialog(null);
        if (result != null)
        {
          fileChooser.setInitialDirectory(result.getParentFile());
          openLogFile(result);
        }
      }
    });

    final Button btLoadVideo = new Button("Video");
    btLoadVideo.setOnAction(new EventHandler<ActionEvent>()
    {
      @Override
      public void handle(ActionEvent event)
      {
        btLoadVideo.getScene().getRoot().requestFocus();
        File result = fileChooser.showOpenDialog(null);
        if (result != null)
        {
          fileChooser.setInitialDirectory(result.getParentFile());
          setMedia(result);
        }
      }
    });

    Text lblOffset = new Text("Offset:");
    txtOffset = new TextField("0.0");
    txtOffset.textProperty().bindBidirectional(timeOffset, new DoubleConverter());

    btSyncPointVideo = new Button("Video Sync point");
    btSyncPointVideo.setOnAction(new EventHandler<ActionEvent>()
    {
      @Override
      public void handle(ActionEvent event)
      {
        if (videoController != null)
        {
          syncTimeVideo =  videoController.getElapsedSeconds();
          updateOffset(true);
        }
      }
    });
    cbSyncLog = new ChoiceBox<>();
    cbSyncLog.valueProperty().addListener(new ChangeListener<GameStateChange>()
    {

      @Override
      public void changed(ObservableValue<? extends GameStateChange> observable, GameStateChange oldValue, GameStateChange newValue)
      {
        syncTimeLog = newValue.time;
        updateOffset(true);
      }
    });

    frameSlider = new Slider(0.0, 0.0, 0.0);
    frameSlider.setShowTickLabels(false);
    frameSlider.setShowTickMarks(true);
    frameSlider.setMinorTickCount(0);
    frameSlider.setBlockIncrement(1.0);
    frameSlider.setMajorTickUnit(100.0);
    
    HBox.setHgrow(frameSlider, Priority.ALWAYS);

    HBox upper = new HBox(btLoadLog, btLoadVideo);
    HBox lower = new HBox(btSyncPointVideo, cbSyncLog, lblOffset, txtOffset);
    HBox sliderBox = new HBox(frameSlider);
    
    return new VBox(upper, lower, sliderBox);
  }

  @Override
  public Scene createScene()
  {
//    com.sun.media.jfxmedia.logging.Logger.setLevel(com.sun.media.jfxmedia.logging.Logger.DEBUG);

    BorderPane rootPane = new BorderPane();
    Scene scene = new Scene(rootPane);
    
    try
      {
        FXMLLoader loader = new FXMLLoader(VideoPlayerController.class.getResource("VideoPlayer.fxml"));
        loader.load();
        videoController = loader.<VideoPlayerController>getController();
        videoController.setAnalyzer(this);
        
        rootPane.setCenter(loader.<Node>getRoot());
        videoController.initAccelerators();
        
      } catch (IOException ex)
      {
        Logger.getLogger(VideoAnalyzer.class.getName()).log(Level.SEVERE, null, ex);
      }

    
    Node controls = createControls();
    controls.prefWidth(Double.MAX_VALUE);
    rootPane.setTop(controls);

    return (scene);
  }
  
  private void openLogFile(File f)
  {
    if (f != null && f.isFile())
    {
      propLogfile.clear();
      try
      {
        FXMLLoader loader = new FXMLLoader(ParseLogController.class.getResource("ParseLog.fxml"));
        loader.load();
        ParseLogController controller = loader.<ParseLogController>getController();
        controller.setLogFile(f);
        controller.setParent(this);

        Scene scene = new Scene(loader.<Parent>getRoot());
        Stage stage = new Stage();
        stage.setScene(scene);
        stage.show();
      } catch (IOException ex)
      {
        Logger.getLogger(VideoAnalyzer.class.getName()).log(Level.SEVERE, null, ex);
      }

    }
  }

  private void setMedia(File file)
  {
    if(videoController != null)
    {
      if(logfile != null && logfile.getOriginalFile() != null)
      {
        Path videoPath = file.getAbsoluteFile().toPath();
        Path basePath = logfile.getOriginalFile().getParentFile().toPath();
        Path relativeVideoPath = basePath.relativize(videoPath);
        propLogfile.setProperty(KEY_VIDEO_FILE, relativeVideoPath.toString());
      }
      else
      {
        propLogfile.setProperty(KEY_VIDEO_FILE, file.getAbsolutePath());
      }
      saveLogfileProperties();
      videoController.open(file);
    }
  }

  private void updateOffset(boolean saveProperties)
  {
    if (syncTimeLog != null && syncTimeVideo != null)
    {
      timeOffset.setValue(syncTimeLog - syncTimeVideo);
      
      if(saveProperties)
      {
        propLogfile.setProperty(KEY_SYNC_TIME_LOG, syncTimeLog.toString());
        propLogfile.setProperty(KEY_SYNC_TIME_VIDEO, syncTimeVideo.toString());
        saveLogfileProperties();
      }
    }
  }

  public void setParseResult(TreeMap<Double, Integer> time2LogFrame, 
    TreeMap<Integer, Double> logFrame2Time)
  {
    this.time2LogFrame = time2LogFrame;
    this.logFrame2Time = logFrame2Time;
  }

  public void setLogfile(LogFile logfile)
  {
    this.logfile = logfile;
    
    frameSlider.valueProperty().removeListener(frameChangeListener);
    
    frameSlider.setMin(0.0);
    frameSlider.setMax(logfile.getFrameCount());
    frameSlider.setValue(0.0);
    sendLogFrame(0);
    
    frameSlider.valueProperty().addListener(frameChangeListener);
    
    loadLogfileProperties();
  }
  
  public void setGameStateChanges(List<GameStateChange> newVal)
  {
    cbSyncLog.getItems().clear();
    cbSyncLog.getItems().addAll(newVal);
    
    if(syncTimeLog != null)
    {
      for(GameStateChange s : newVal)
      {
        if(s.time == syncTimeLog)
        {
          cbSyncLog.valueProperty().set(s);
          break;
        }
      }
    }
  }

  public void sendLogFrame(final int frameIdx)
  {
    if (logfile != null && Plugin.logFileEventManager != null)
    {
      try
      {
        final HashMap<String, LogDataFrame> frame = logfile.readFrame(frameIdx);
        
        Plugin.logFileEventManager.fireLogFrameEvent(frame.values());

      } catch (IOException ex)
      {
        Helper.handleException(ex);
      }
    }
  }
  
  private void loadLogfileProperties()
  {
    if(logfile != null)
    {
      File origFile = logfile.getOriginalFile();
      if(origFile != null)
      {
        File propFile = new File(origFile.getParent(), origFile.getName() + ".videoanalyzer.properties");
        if(propFile.isFile())
        {
          try(FileReader propFileReader = new FileReader(propFile))
          {
            propLogfile.load(propFileReader);

            // set the values stored in the file
            if(propLogfile.containsKey(KEY_VIDEO_FILE))
            {
              Path basePath = origFile.getParentFile().toPath();
              Path videoPath = basePath.resolve(propLogfile.getProperty(KEY_VIDEO_FILE));
              setMedia(videoPath.toFile());
            }
            if(propLogfile.containsKey(KEY_SYNC_TIME_LOG) && propLogfile.containsKey(KEY_SYNC_TIME_VIDEO))
            {
              syncTimeLog = Double.parseDouble(propLogfile.getProperty(KEY_SYNC_TIME_LOG, "0.0"));
              syncTimeVideo = Double.parseDouble(propLogfile.getProperty(KEY_SYNC_TIME_VIDEO, "0.0"));
              updateOffset(false);
            }
          }
          catch(IOException ex)
          {
            Helper.handleException(ex);
          }
        }
      }
    }
  }
  
  private void saveLogfileProperties()
  {
    if(logfile != null)
    {
      File origFile = logfile.getOriginalFile();
      if(origFile != null)
      {
        File propFile = new File(origFile.getParent(), origFile.getName() + ".videoanalyzer.properties");
        try(FileWriter propFileWriter = new FileWriter(propFile))
        {
          propLogfile.store(propFileWriter, null);
        }
        catch(IOException ex)
        {
          Helper.handleException(ex);
        }
      }
    }
  }

  private static class DoubleConverter extends StringConverter<Double>
  {

    @Override
    public String toString(Double object)
    {
      if (object == null)
      {
        return "0.0";
      } else
      {
        return object.toString();
      }
    }

    @Override
    public Double fromString(String string)
    {
      return Double.parseDouble(string);
    }

  }

}
