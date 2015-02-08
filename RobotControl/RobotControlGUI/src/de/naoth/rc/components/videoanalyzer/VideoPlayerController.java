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
import javafx.event.EventHandler;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.geometry.Rectangle2D;
import javafx.scene.Group;
import javafx.scene.Node;
import javafx.scene.control.Label;
import javafx.scene.control.ScrollPane;
import javafx.scene.control.Slider;
import javafx.scene.control.ToggleButton;
import javafx.scene.control.Tooltip;
import javafx.scene.image.ImageView;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyCodeCombination;
import javafx.scene.input.MouseButton;
import javafx.scene.input.MouseEvent;
import javafx.scene.input.ScrollEvent;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.GridPane;
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
  private BorderPane rootPane;

  @FXML
  private MediaView mediaView;
  @FXML
  private ScrollPane mediaPane;
  
  @FXML
  private Slider timeSlider;
  @FXML
  private ToggleButton playButton;
  @FXML
  private Label timeCodeLabel;
  
  @FXML
  private Slider zoomSlider;
  
  @FXML
  private Label labelDebugPos;
  
  private Media media;
  private MediaPlayer player;

  private VideoAnalyzer analyzer;
  
  private Double lastX;
  private Double lastY;

  private final SliderChangedListener sliderChangeListener = new SliderChangedListener();

  /**
   * Initializes the controller class.
   */
  @Override
  public void initialize(URL url, ResourceBundle rb)
  {
    playButton.setGraphic(new ImageView(getClass().getResource("/de/naoth/rc/res/play.png").toString()));
    Tooltip.install(playButton, new Tooltip("Play/Pause"));

    timeSlider.valueProperty().addListener(sliderChangeListener);
    timeSlider.setLabelFormatter(new TickFormatter());

    mediaView.fitHeightProperty().bind(mediaPane.heightProperty());
    mediaView.fitWidthProperty().bind(mediaPane.widthProperty());
    
    zoomSlider.valueProperty().addListener(new ChangeListener<Number>()
    {
      @Override
      public void changed(ObservableValue<? extends Number> observable, Number oldValue, Number newValue)
      {
        double origHeight = media.getHeight();
        double origWidth = media.getWidth();
        
        double newHeight = origHeight / zoomSlider.getValue();
        double newWidth = origWidth / zoomSlider.getValue();
        
        double newX = (origWidth - newWidth) / 2.0;
        double newY = (origHeight - newHeight) / 2.0;
        
        mediaView.setViewport(new Rectangle2D(newX, newY, newWidth, newHeight));
      }
    });
    
    mediaView.setOnMouseClicked(new EventHandler<MouseEvent>()
    {

      @Override
      public void handle(MouseEvent event)
      {
        if(event.getClickCount() == 2 && event.getButton() == MouseButton.PRIMARY)
        {
          mediaView.setViewport(new Rectangle2D(0.0, 0.0, media.getWidth(), media.getHeight()));
        }
      }
    });
    
    mediaView.setOnMouseMoved(new EventHandler<MouseEvent>()
    {

      @Override
      public void handle(MouseEvent event)
      {
        lastX = event.getX();
        lastY = event.getY();
        labelDebugPos.setText(""+lastX+"x"+lastY);
      }
    });
    mediaView.setOnMouseExited(new EventHandler<MouseEvent>()
    {

      @Override
      public void handle(MouseEvent event)
      {
        lastX = null;
        lastY = null;
      }
    });
    
    mediaView.setOnScroll(new EventHandler<ScrollEvent>()
    {

      @Override
      public void handle(ScrollEvent event)
      {
        double diff = event.getDeltaY()/100.0;
        if(lastX != null && lastY != null)
        {

        }
        zoomSlider.setValue(zoomSlider.getValue()+diff);
//        zoomSlider.valueProperty().add(diff);
      }
    });
  }
  
  

  public void initAccelerators()
  {
    rootPane.getScene().getAccelerators().put(new KeyCodeCombination(KeyCode.SPACE),
      new Runnable()
      {

        @Override
        public void run()
        {
          playButton.fire();
        }
      });
  }

  @FXML
  private void playPause(ActionEvent event)
  {
    if (player != null)
    {
      if (playButton.isSelected())
      {
        player.play();
      } else
      {
        player.pause();
      }
    }
  }

  public void setAnalyzer(VideoAnalyzer analyzer)
  {
    this.analyzer = analyzer;
  }

  public void open(File file)
  {
    media = new Media(file.toURI().toASCIIString());
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
    timeSlider.valueProperty().addListener(sliderChangeListener);
    timeSlider.setOnMouseClicked(new EventHandler<MouseEvent>()
    {

      @Override
      public void handle(MouseEvent event)
      {
        if (player != null)
        {
          player.pause();
          playButton.setSelected(false);
        }
      }
    });
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
      if (player != null
        && player.getStatus() == MediaPlayer.Status.PLAYING)
      {
        return;
      }
      final Duration duration = Duration.seconds(newValue.doubleValue());
      if (player != null)
      {
        player.seek(duration);
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
