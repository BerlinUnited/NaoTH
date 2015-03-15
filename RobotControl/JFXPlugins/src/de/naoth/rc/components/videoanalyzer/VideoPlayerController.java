/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc.components.videoanalyzer;

import de.naoth.rc.Helper;
import java.io.File;
import java.net.URL;
import java.util.ResourceBundle;
import javafx.application.Platform;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.control.Label;
import javafx.scene.control.Slider;
import javafx.scene.control.ToggleButton;
import javafx.scene.control.Tooltip;
import javafx.scene.image.ImageView;
import javafx.scene.layout.StackPane;
import javafx.scene.media.Media;
import javafx.scene.media.MediaException;
import javafx.scene.media.MediaPlayer;
import javafx.scene.media.MediaView;
import javafx.util.Duration;
import javafx.util.StringConverter;

/**
 * FXML Controller class
 *
 * @author thomas
 */
public class VideoPlayerController implements Initializable
{

  public static final String FXML = "VideoPlayer.fxml";

  @FXML
  private MediaView mediaView;
  @FXML
  private StackPane mediaPane;

  @FXML
  private Slider timeSlider;

  @FXML
  private ToggleButton playButton;
  @FXML
  private Label timeCodeLabel;

  private Media media;
  private MediaPlayer player;

  private VideoAnalyzerController analyzer;

  private final SliderChangedListener sliderChangeListener = new SliderChangedListener();

  private final double MAX_FRAME_LENGTH = 60.0;

  /**
   * Initializes the controller class.
   */
  @Override
  public void initialize(URL url, ResourceBundle rb)
  {
    playButton.setGraphic(new ImageView(getClass().getResource("/de/naoth/rc/res/play.png").toString()));
    Tooltip.install(playButton, new Tooltip("Play/Pause"));

    timeSlider.setLabelFormatter(new TickFormatter());

    mediaView.fitHeightProperty().bind(mediaPane.heightProperty());
    mediaView.fitWidthProperty().bind(mediaPane.widthProperty());
  }

  public void togglePlay()
  {
    if (player != null)
    {
      if (player.getStatus() == MediaPlayer.Status.PLAYING)
      {
        pause();
      } else
      {
        play();
      }
    }
  }

  @FXML
  private void playPause(ActionEvent event)
  {
    if (player != null)
    {
      if (playButton.isSelected())
      {
        play();
      } else
      {
        pause();
      }
    }
  }

  public void setTime(double newTimeSeconds)
  {
    if (player != null)
    {
      pauseAndSeek(Duration.seconds(newTimeSeconds), true);
      updateGUIForTimeCode(Duration.seconds(newTimeSeconds));
    }
  }

  public void setAnalyzer(VideoAnalyzerController analyzer)
  {
    this.analyzer = analyzer;
  }

  public void open(File file)
  {
    media = new Media(file.toURI().toASCIIString());
    try
    {
      player = new MediaPlayer(media);
      player.setOnReady(new Runnable()
      {
        @Override
        public void run()
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
      });
      player.setOnEndOfMedia(new Runnable()
      {

        @Override
        public void run()
        {
          player.stop();
          pauseAndSeek(Duration.ZERO);
          updateGUIForTimeCode(Duration.ZERO);
        }
      });

      player.currentTimeProperty().addListener(new CurrentTimeListener());

      mediaView.setMediaPlayer(player);

      timeSlider.valueProperty().addListener(sliderChangeListener);

    } catch (MediaException ex)
    {
      Helper.handleException("Could not create the video output", ex);
    }
  }

  private void pause()
  {
    pauseAndSeek(null, false);
  }

  private void pauseAndSeek(final Duration seek)
  {
    pauseAndSeek(seek, false);
  }

  private void pauseAndSeek(final Duration seek, boolean fromExternal)
  {
    if (player != null)
    {
      boolean wasPaused = player.getStatus() == MediaPlayer.Status.PAUSED
        || player.getStatus() == MediaPlayer.Status.STOPPED
        || player.getStatus() == MediaPlayer.Status.READY;

      internalPrepareSeek(seek, wasPaused, fromExternal);

      if (!wasPaused)
      {
        player.pause();
      }

      playButton.setSelected(false);

    }
  }

  private void internalPrepareSeek(final Duration seek, boolean wasPaused, boolean fromExternal)
  {
    if (seek == null)
    {
      return;
    }

    if (!wasPaused)
    {
      player.setOnPaused(new Runnable()
      {

        @Override
        public void run()
        {
          player.seek(seek);
          if (analyzer != null)
          {
            analyzer.setLogFrameFromVideo(seek.toSeconds());
          }
          player.setOnPaused(null);
        }
      });
    } else
    {
      player.seek(seek);
      if (!fromExternal && analyzer != null)
      {
        analyzer.setLogFrameFromVideo(seek.toSeconds());
      }
    }
  }

  private void play()
  {
    if (player != null)
    {
      player.play();
      playButton.setSelected(true);
    }
  }

  private void updateGUIForTimeCode(Duration time)
  {
    double minutes = Math.floor(time.toMinutes());
    double seconds = time.toSeconds() - (minutes * 60);
    timeCodeLabel.setText(String.format("%02d:%05.2f", (int) minutes, seconds));
    timeSlider.setValue(time.toSeconds());
  }

  public double getElapsedSeconds()
  {
    if (player != null)
    {
      return player.getCurrentTime().toSeconds();
    }
    return 0.0;
  }

  public static class TickFormatter extends StringConverter<Double>
  {

    @Override
    public String toString(Double object)
    {
      Duration elapsed = Duration.seconds(object);
      double minutes = Math.floor(elapsed.toMinutes());
      double seconds = elapsed.toSeconds() - (minutes * 60);

      return String.format("%02d:%02.0f", (int) minutes, seconds);

    }

    @Override
    public Double fromString(String string)
    {
      throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }
  }

  private class SliderChangedListener implements ChangeListener<Number>
  {

    @Override
    public void changed(ObservableValue<? extends Number> observable, Number oldValue, Number newValue)
    {

      final Duration duration = Duration.seconds(newValue.doubleValue());
      
      if (player != null)
      {
        boolean wasChangedManually = false;
        Duration playerDuration = player.getCurrentTime();

        double diff = Math.abs(duration.toMillis() - playerDuration.toMillis());

        if (diff > MAX_FRAME_LENGTH)
        {
          wasChangedManually = true;
        }
        else if(player.getStatus() == MediaPlayer.Status.PAUSED || player.getStatus() == MediaPlayer.Status.READY)
        {
          wasChangedManually = true;
        }
        
        if(wasChangedManually)
        {
          pauseAndSeek(duration);
        }

      }
    }
  }

  private class CurrentTimeListener implements ChangeListener<Duration>
  {

    @Override
    public void changed(ObservableValue<? extends Duration> observable, Duration oldValue, Duration newValue)
    {
      Platform.runLater(new Runnable()
      {
        @Override
        public void run()
        {
          updateGUIForTimeCode(newValue);
 
          if (analyzer != null && player.statusProperty().get() == MediaPlayer.Status.PLAYING)
          {
            analyzer.setLogFrameFromVideo(newValue.toSeconds());
          }
        }
      });
    }

  }
}
