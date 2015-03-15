/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc.components.videoanalyzer;

import de.naoth.rc.Helper;
import static de.naoth.rc.components.videoanalyzer.VideoAnalyzer.KEY_SYNC_TIME_LOG;
import static de.naoth.rc.components.videoanalyzer.VideoAnalyzer.KEY_SYNC_TIME_VIDEO;
import static de.naoth.rc.components.videoanalyzer.VideoAnalyzer.KEY_VIDEO_FILE;
import de.naoth.rc.dataformats.LogFile;
import de.naoth.rc.logmanager.LogDataFrame;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.net.URL;
import java.nio.file.Path;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Properties;
import java.util.ResourceBundle;
import java.util.TreeMap;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.beans.property.Property;
import javafx.beans.property.SimpleObjectProperty;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.fxml.Initializable;
import javafx.scene.Node;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.ChoiceBox;
import javafx.scene.control.Label;
import javafx.scene.control.Slider;
import javafx.scene.layout.Pane;
import javafx.stage.FileChooser;
import javafx.stage.Stage;
import javafx.util.StringConverter;

/**
 * FXML Controller class
 *
 * @author thomas
 */
public class VideoAnalyzerController implements Initializable
{

  private LogFile logfile;
  private final FileChooser fileChooser = new FileChooser();

  private final Properties propLogfile = new Properties();

  /**
   * Maps a second (fractioned) to a log frame number
   */
  private TreeMap<Double, Integer> time2LogFrame = new TreeMap<>();
  private TreeMap<Integer, Double> logFrame2Time = new TreeMap<>();

  private Double syncTimeLog;
  private Double syncTimeVideo;

  @FXML
  private Button btLoadVideo;
  @FXML
  private ChoiceBox<VideoAnalyzer.GameStateChange> cbSyncLog;
  @FXML
  private Label lblOffset;
  @FXML
  private Slider frameSlider;
  @FXML
  private Node video;
  @FXML
  private VideoPlayerController videoController;

  @FXML
  private Pane syncPane;
  
  private final ChangeListener<Number> frameChangeListener = new ChangeListener<Number>()
  {

    @Override
    public void changed(ObservableValue<? extends Number> observable, Number oldValue, Number newValue)
    {
      setVideoTimeFromLogFrame();
    }
  };

  private final Property<Double> timeOffset = new SimpleObjectProperty<>(0.0);

  /**
   * Initializes the controller class.
   */
  @Override
  public void initialize(URL url, ResourceBundle rb)
  {
    cbSyncLog.valueProperty().addListener(new ChangeListener<VideoAnalyzer.GameStateChange>()
    {

      @Override
      public void changed(ObservableValue<? extends VideoAnalyzer.GameStateChange> observable, VideoAnalyzer.GameStateChange oldValue, VideoAnalyzer.GameStateChange newValue)
      {
        if(newValue != null)
        {
          syncTimeLog = newValue.time;
          updateOffset(true);
        }
      }
    });
    
    
    lblOffset.textProperty().bindBidirectional(timeOffset, new DoubleConverter());
    
    videoController.setAnalyzer(this);
  }
  
  public void togglePlay()
  {
    videoController.togglePlay();
  }
  

  public void openLogfile(ActionEvent event)
  {
    File f = fileChooser.showOpenDialog(null);
    if (f != null)
    {
      fileChooser.setInitialDirectory(f.getParentFile());

      if (f.isFile())
      {
        propLogfile.clear();
        try
        {
          FXMLLoader loader = new FXMLLoader(ParseLogController.class.getResource("ParseLog.fxml"));
          loader.setClassLoader(VideoAnalyzer.class.getClassLoader());
          loader.load();
          ParseLogController controller = loader.<ParseLogController>getController();
          controller.setLogFile(f);
          controller.setParent(this);

          Scene scene = new Scene(loader.<Parent>getRoot());
          Stage stage = new Stage();
          stage.setScene(scene);
          stage.show();
          
          btLoadVideo.setDisable(false);
          
        } catch (IOException ex)
        {
          Logger.getLogger(VideoAnalyzer.class.getName()).log(Level.SEVERE, null, ex);
        }

      }

    }
  }
  
  public void openVideo(ActionEvent event)
  {
    btLoadVideo.getScene().getRoot().requestFocus();
    File result = fileChooser.showOpenDialog(null);
    if (result != null)
    {
      fileChooser.setInitialDirectory(result.getParentFile());
      setMedia(result);
    }
  }
  
  public void videoSyncPointSelected(ActionEvent event)
  {
    if (videoController != null)
    {
      syncTimeVideo = videoController.getElapsedSeconds();
      updateOffset(true);
    }
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

  public void sendLogFrame(final int frameIdx)
  {
    if (logfile != null && VideoAnalyzer.Plugin.logFileEventManager != null)
    {
      try
      {
        final HashMap<String, LogDataFrame> frame = logfile.readFrame(frameIdx);
        if (frame != null)
        {
          VideoAnalyzer.Plugin.logFileEventManager.fireLogFrameEvent(frame.values());
        }
      } catch (Exception ex)
      {
        Helper.handleException(ex);
      }
    }
  }

  public void setLogFrameFromVideo()
  {
    if (videoController != null)
    {
      double searchVal = videoController.getElapsedSeconds() + timeOffset.getValue();

      Map.Entry<Double, Integer> frame = time2LogFrame.floorEntry(searchVal);
      if (frame != null)
      {
        if (frameSlider.valueProperty().get() != frame.getValue())
        {
          sendLogFrame(frame.getValue());
          frameSlider.valueProperty().removeListener(frameChangeListener);
          frameSlider.valueProperty().set((double) frame.getValue());
          frameSlider.valueProperty().addListener(frameChangeListener);
        }
      }
    }
  }

  public void setParseResult(TreeMap<Double, Integer> time2LogFrame,
    TreeMap<Integer, Double> logFrame2Time)
  {
    this.time2LogFrame = time2LogFrame;
    this.logFrame2Time = logFrame2Time;
  }

  public void setGameStateChanges(List<VideoAnalyzer.GameStateChange> newVal)
  {
    cbSyncLog.getItems().clear();
    cbSyncLog.getItems().addAll(newVal);

    if (syncTimeLog != null)
    {
      for (VideoAnalyzer.GameStateChange s : newVal)
      {
        if (s.time == syncTimeLog)
        {
          cbSyncLog.valueProperty().set(s);
          break;
        }
      }
    }
  }

  private void setVideoTimeFromLogFrame()
  {
    if (videoController != null && logFrame2Time != null)
    {
      int searchVal = frameSlider.valueProperty().intValue();
      Map.Entry<Integer, Double> time = logFrame2Time.floorEntry(searchVal);
      if (time != null)
      {
        videoController.setTime(time.getValue() - timeOffset.getValue());
      }
    }
  }

  private void updateOffset(boolean saveProperties)
  {
    if (syncTimeLog != null && syncTimeVideo != null)
    {
      timeOffset.setValue(syncTimeLog - syncTimeVideo);

      if (saveProperties)
      {
        propLogfile.setProperty(KEY_SYNC_TIME_LOG, syncTimeLog.toString());
        propLogfile.setProperty(KEY_SYNC_TIME_VIDEO, syncTimeVideo.toString());
        saveLogfileProperties();
      }
    }
  }

  private void loadLogfileProperties()
  {
    if (logfile != null)
    {
      File origFile = logfile.getOriginalFile();
      if (origFile != null)
      {
        File propFile = new File(origFile.getParent(), origFile.getName() + ".videoanalyzer.properties");
        if (propFile.isFile())
        {
          try (FileReader propFileReader = new FileReader(propFile))
          {
            propLogfile.load(propFileReader);

            // set the values stored in the file
            if (propLogfile.containsKey(KEY_VIDEO_FILE))
            {
              Path basePath = origFile.getParentFile().toPath();
              Path videoPath = basePath.resolve(propLogfile.getProperty(KEY_VIDEO_FILE));
              setMedia(videoPath.toFile());
            }
            if (propLogfile.containsKey(KEY_SYNC_TIME_LOG) && propLogfile.containsKey(KEY_SYNC_TIME_VIDEO))
            {
              syncTimeLog = Double.parseDouble(propLogfile.getProperty(KEY_SYNC_TIME_LOG, "0.0"));
              syncTimeVideo = Double.parseDouble(propLogfile.getProperty(KEY_SYNC_TIME_VIDEO, "0.0"));
              updateOffset(false);
            }
          } catch (IOException ex)
          {
            Helper.handleException(ex);
          }
        }
      }
    }
  }

  private void saveLogfileProperties()
  {
    if (logfile != null)
    {
      File origFile = logfile.getOriginalFile();
      if (origFile != null)
      {
        File propFile = new File(origFile.getParent(), origFile.getName() + ".videoanalyzer.properties");
        try (FileWriter propFileWriter = new FileWriter(propFile))
        {
          propLogfile.store(propFileWriter, null);
        } catch (IOException ex)
        {
          Helper.handleException(ex);
        }
      }
    }
  }

  private void setMedia(File file)
  {
    if (videoController != null)
    {
      if (logfile != null && logfile.getOriginalFile() != null)
      {
        Path videoPath = file.getAbsoluteFile().toPath();
        Path basePath = logfile.getOriginalFile().getParentFile().toPath();
        Path relativeVideoPath = basePath.relativize(videoPath);
        propLogfile.setProperty(KEY_VIDEO_FILE, relativeVideoPath.toString());
      } else
      {
        propLogfile.setProperty(KEY_VIDEO_FILE, file.getAbsolutePath());
      }
      saveLogfileProperties();
      videoController.open(file);
      
      syncPane.setDisable(false);
      
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
