/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc.components.videoanalyzer;

import de.naoth.rc.dialogs.VideoAnalyzer;
import java.io.File;
import java.net.URL;
import java.util.ResourceBundle;
import javafx.application.Platform;
import javafx.beans.InvalidationListener;
import javafx.beans.Observable;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.Node;
import javafx.scene.control.Label;
import javafx.scene.control.Slider;
import javafx.scene.control.ToggleButton;
import javafx.scene.layout.Pane;
import javafx.scene.media.Media;
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

  @FXML
  private Pane rootPane;

  @FXML
  private MediaView mediaView;
  @FXML
  private Slider timeSlider;
  @FXML
  private ToggleButton playButton;
  @FXML
  private Label timeCodeLabel;

  private MediaPlayer player;

  private VideoAnalyzer analyzer;
  
  private SliderChangedListener sliderChangeListener = new SliderChangedListener();

  /**
   * Initializes the controller class.
   */
  @Override
  public void initialize(URL url, ResourceBundle rb)
  {
    timeSlider.valueProperty().addListener(sliderChangeListener);
    timeSlider.setLabelFormatter(new TickFormatter());
    mediaView.fitWidthProperty().bind(rootPane.widthProperty());

  }

  private void seek(double pos)
  {
    if (player != null)
    {
      player.pause();
      player.seek(Duration.seconds(pos));
    }
  }

  @FXML
  private void playPause(ActionEvent event)
  {
    if (playButton.isSelected())
    {
      timeSlider.valueProperty().removeListener(sliderChangeListener);
      player.play();
      playButton.setText("Pause");
    } else
    {
      player.pause();
      timeSlider.valueProperty().addListener(sliderChangeListener);
      playButton.setText("Play");
    }
  }

  public void setAnalyzer(VideoAnalyzer analyzer)
  {
    this.analyzer = analyzer;
  }

  public void open(File file)
  {
    Media media = new Media(file.toURI().toASCIIString());
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
    player.currentTimeProperty().addListener(new CurrentTimeListener());
    mediaView.setMediaPlayer(player);
  }

  public double getElapsedSeconds()
  {
    return player.getCurrentTime().toSeconds();
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
      Duration duration = Duration.seconds(newValue.doubleValue());
      if (player != null && !duration.equals(player.getCurrentTime()))
      {
        seek(newValue.doubleValue());
      }
    }
  }

  private class CurrentTimeListener implements InvalidationListener
  {

    @Override
    public void invalidated(Observable observable)
    {
      Platform.runLater(new Runnable()
      {
        @Override
        public void run()
        {
          Duration elapsed = player.getCurrentTime();
          double minutes = Math.floor(elapsed.toMinutes());
          double seconds = elapsed.toSeconds() - (minutes * 60);
          timeCodeLabel.setText(String.format("%02d:%05.2f", (int) minutes, seconds));
          timeSlider.setValue(elapsed.toSeconds());

          if (analyzer != null)
          {
            analyzer.setLogFrameFromVideo();
          }
        }
      });

    }

  }

}
