/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc.dialogs;

import de.naoth.rc.Helper;
import de.naoth.rc.RobotControl;
import de.naoth.rc.components.behaviorviewer.XABSLBehaviorFrame;
import de.naoth.rc.components.behaviorviewer.XABSLProtoParser;
import de.naoth.rc.components.behaviorviewer.model.Symbol;
import de.naoth.rc.core.dialog.AbstractJFXDialog;
import de.naoth.rc.core.dialog.DialogPlugin;
import de.naoth.rc.core.manager.SwingCommandExecutor;
import de.naoth.rc.dataformats.LogFile;
import de.naoth.rc.dialogs.VideoAnalyzer.Plugin;
import de.naoth.rc.logmanager.LogDataFrame;
import de.naoth.rc.logmanager.LogFileEventManager;
import de.naoth.rc.messages.FrameworkRepresentations.FrameInfo;
import de.naoth.rc.messages.Messages.BehaviorStateComplete;
import de.naoth.rc.messages.Messages.BehaviorStateSparse;
import java.io.File;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import java.util.TreeMap;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.beans.property.Property;
import javafx.beans.property.SimpleObjectProperty;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.scene.Node;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.ChoiceBox;
import javafx.scene.control.Slider;
import javafx.scene.control.TextField;
import javafx.scene.input.MouseButton;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.Priority;
import javafx.scene.layout.VBox;
import javafx.scene.media.Media;
import javafx.scene.media.MediaPlayer;
import javafx.scene.media.MediaView;
import javafx.scene.text.Text;
import javafx.stage.FileChooser;
import javafx.util.Duration;
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

  private MediaPlayer player;
  private MediaView mediaView;
  private Slider timeSlider;
  private Text lblTime;
  private TextField txtOffset;
  private ChoiceBox<GameStateChange> cbSyncLog;
  private Button btSyncPointVideo;
  private final FileChooser fileChooser = new FileChooser();

  private final Property<Double> timeOffset = new SimpleObjectProperty<>(0.0);
  private Double syncTimeLog;
  private Double syncTimeVideo;

  /**
   * Maps a second (fractioned) to a log frame number
   */
  private final TreeMap<Double, Integer> time2LogFrame = new TreeMap<>();

  private LogFile logfile;

  public VideoAnalyzer()
  {
    timeOffset.addListener(new ChangeListener<Double>()
    {

      @Override
      public void changed(ObservableValue<? extends Double> observable, Double oldValue, Double newValue)
      {
        setLogFrameFromCurrentTime();
      }
    });
  }

  private void setLogFrameFromCurrentTime()
  {
    if(player != null)
    {
      double searchVal = player.getCurrentTime().toSeconds() + timeOffset.getValue();

      Map.Entry<Double, Integer> frame = time2LogFrame.floorEntry(searchVal);
      if (frame != null)
      {
        sendLogFrame(frame.getValue());
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
          setLogFile(result);
        }
      }
    });

    Button btLoadVideo = new Button("Video");
    btLoadVideo.setOnAction(new EventHandler<ActionEvent>()
    {
      @Override
      public void handle(ActionEvent event)
      {
        File result = fileChooser.showOpenDialog(null);
        if (result != null)
        {
          fileChooser.setInitialDirectory(result.getParentFile());
          setMedia(new Media(result.toURI().toString()));
        }
      }
    });

    timeSlider = new Slider(0.0, 0.0, 0.0);
    timeSlider.setLabelFormatter(new StringConverter<Double>()
    {

      @Override
      public String toString(Double object)
      {
        return formatTime(Duration.seconds(object), false);
      }

      @Override
      public Double fromString(String string)
      {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
      }
    });

    HBox.setHgrow(timeSlider, Priority.ALWAYS);

    timeSlider.valueProperty().addListener(new ChangeListener<Number>()
    {

      @Override
      public void changed(ObservableValue<? extends Number> observable, Number oldValue, Number newValue)
      {
        if (timeSlider.isValueChanging())
        {
          seekFromSliderPos();
        }
      }
    });
    timeSlider.setOnMouseClicked(new EventHandler<MouseEvent>()
    {

      @Override
      public void handle(MouseEvent event)
      {
        if (event.getButton() == MouseButton.PRIMARY)
        {
          seekFromSliderPos();
        }
      }
    });

    lblTime = new Text("mm:ss");

    Text lblOffset = new Text("Offset:");
    txtOffset = new TextField("0.0");
    txtOffset.textProperty().bindBidirectional(timeOffset, new DoubleConverter());

    btSyncPointVideo = new Button("Video Sync point");
    btSyncPointVideo.setOnAction(new EventHandler<ActionEvent>()
    {
      @Override
      public void handle(ActionEvent event)
      {
        if (player != null)
        {
          syncTimeVideo = player.getCurrentTime().toSeconds();
          updateOffset();
        }
      }
    });
    cbSyncLog = new ChoiceBox<>();
    cbSyncLog.valueProperty().addListener(new ChangeListener<GameStateChange>()
    {

      @Override
      public void changed(ObservableValue<? extends GameStateChange> observable, GameStateChange oldValue, GameStateChange newValue)
      {
        if(player != null)
        {
          syncTimeLog = newValue.time;
          updateOffset();
        }
      }
    });

    HBox upper = new HBox(btLoadLog, btLoadVideo, timeSlider, lblTime);
    HBox lower = new HBox(btSyncPointVideo, cbSyncLog, lblOffset, txtOffset);

    return new VBox(upper, lower);
  }

  @Override
  public Scene createScene()
  {
//    com.sun.media.jfxmedia.logging.Logger.setLevel(com.sun.media.jfxmedia.logging.Logger.DEBUG);

    BorderPane rootPane = new BorderPane();
    Scene scene = new Scene(rootPane);

    mediaView = new MediaView();
    mediaView.setPreserveRatio(true);
    mediaView.fitWidthProperty().bind(rootPane.widthProperty());

    mediaView.setOnMouseClicked(new EventHandler<MouseEvent>()
    {

      @Override
      public void handle(MouseEvent event)
      {
        if (event.getButton() == MouseButton.PRIMARY)
        {
          if (player.statusProperty().get() == MediaPlayer.Status.PLAYING)
          {
            player.pause();
          } else
          {
            player.play();
          }
        }
      }
    });

    rootPane.setCenter(mediaView);
    Node controls = createControls();
    controls.prefWidth(Double.MAX_VALUE);
    rootPane.setTop(controls);

    return (scene);
  }

  private void setLogFile(File f)
  {
    if (f != null && f.isFile())
    {
      try
      {
        logfile = new LogFile(f);
        initFrameMap();
        sendLogFrame(0);

      } catch (IOException ex)
      {
        Helper.handleException("Could not open logfile " + f.getAbsolutePath(), ex);
      }

    }
  }

  private void setMedia(Media media)
  {
    if (media == null)
    {
      return;
    }
    player = null;
    timeSlider.setValue(0.0);

    player = new MediaPlayer(media);
    mediaView.setMediaPlayer(player);

    timeSlider.setMax(0.0);

    player.statusProperty().addListener(new ChangeListener<MediaPlayer.Status>()
    {

      @Override
      public void changed(ObservableValue<? extends MediaPlayer.Status> observable, MediaPlayer.Status oldValue, MediaPlayer.Status newValue)
      {
        if (newValue == MediaPlayer.Status.READY)
        {
          Duration total = player.getTotalDuration();
          if (total != Duration.UNKNOWN && total != Duration.INDEFINITE)
          {
            timeSlider.setMax(total.toSeconds());
            timeSlider.setShowTickMarks(true);
            timeSlider.setShowTickLabels(true);

            if (total.toMinutes() < 1.5)
            {
              timeSlider.setMajorTickUnit(5.0);
              timeSlider.setMinorTickCount(0);
            } else
            {
              timeSlider.setMajorTickUnit(60.0);
              timeSlider.setMinorTickCount(10);
            }
          }
        }
      }
    });

    player.currentTimeProperty().addListener(new ChangeListener<Duration>()
    {

      @Override
      public void changed(ObservableValue<? extends Duration> observable, Duration oldValue, Duration newValue)
      {
        timeSlider.setValue(newValue.toSeconds());
        lblTime.setText(formatTime(newValue, true));

        setLogFrameFromCurrentTime();
      }
    });

  }

  private void updateOffset()
  {
    if (syncTimeLog != null && syncTimeVideo != null)
    {
      timeOffset.setValue(syncTimeLog - syncTimeVideo);
    }
  }

  private void initFrameMap()
  {
    time2LogFrame.clear();
    
    XABSLProtoParser parser = new XABSLProtoParser();
    
    if (logfile != null)
    {
      try
      {
        String lastGameState = "";
        for (int i = 0; i < logfile.getFrameCount(); i++)
        {
          Double currentTime = null;
          Map<String, LogDataFrame> messages = logfile.readFrame(i);
          LogDataFrame frameRaw = messages.get("FrameInfo");
          if (frameRaw != null)
          {
            FrameInfo frame = FrameInfo.parseFrom(frameRaw.getData());
            currentTime = ((double) frame.getTime()) / 1000.0;
            time2LogFrame.put(currentTime, i);

            if (i == 0)
            {
              timeOffset.setValue(currentTime);
            }
          }
          XABSLBehaviorFrame behaviorFrame = null;
          // parse any behavior related messages
          LogDataFrame completeStatusRaw = messages.get("BehaviorStateComplete");
          if(completeStatusRaw != null)
          {
            BehaviorStateComplete completeStatus = BehaviorStateComplete.parseFrom(completeStatusRaw.getData());
            parser.parse(completeStatus);
          }
          LogDataFrame statusRaw = messages.get("BehaviorStateSparse");
          if(statusRaw != null)
          {
            BehaviorStateSparse status = BehaviorStateSparse.parseFrom(statusRaw.getData());
            behaviorFrame = parser.parse(status);
          }
          // try to detect game state changes 
          if(behaviorFrame != null && currentTime != null)
          {
            Symbol s = behaviorFrame.getSymbolByName("game.state");
            if(s != null)
            {
              if(!lastGameState.equals(s.getValueAsString()))
              {
                // CHANGE DETECTED!
                GameStateChange change = new GameStateChange();
                change.time = currentTime;
                change.state = s.getValueAsString();
                cbSyncLog.getItems().add(change);
                
                lastGameState = s.getValueAsString();
                
              }
            }
          }
          
        } // end for each frame
      } catch (IOException ex)
      {
        Logger.getLogger(VideoAnalyzer.class.getName()).log(Level.SEVERE, null, ex);
      }
    }
  }

  private void sendLogFrame(final int frameNumber)
  {
    if (logfile != null && Plugin.logFileEventManager != null)
    {
      try
      {
        final HashMap<String, LogDataFrame> frame = logfile.readFrame(frameNumber);
        Plugin.logFileEventManager.fireLogFrameEvent(frame.values(), logfile.getFrameNumber(frameNumber));

      } catch (IOException ex)
      {
        Helper.handleException(ex);
      }
    }
  }

  private void seekFromSliderPos()
  {
    if (player != null)
    {
      player.pause();
      player.seek(Duration.seconds(timeSlider.getValue()));
    }
  }

  private String formatTime(Duration elapsed, boolean withDecimal)
  {
    double minutes = Math.floor(elapsed.toMinutes());
    double seconds = elapsed.toSeconds() - (minutes * 60);

    if (withDecimal)
    {
      return String.format("%02d:%05.2f", (int) minutes, seconds);
    } else
    {
      return String.format("%02d:%02.0f", (int) minutes, seconds);
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
