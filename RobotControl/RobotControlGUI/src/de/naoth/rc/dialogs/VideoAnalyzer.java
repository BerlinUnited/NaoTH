/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc.dialogs;

import de.naoth.rc.Helper;
import de.naoth.rc.RobotControl;
import de.naoth.rc.core.dialog.AbstractJFXDialog;
import de.naoth.rc.core.dialog.DialogPlugin;
import de.naoth.rc.core.manager.SwingCommandExecutor;
import de.naoth.rc.dialogs.VideoAnalyzer.Plugin;
import de.naoth.rc.logmanager.LogDataFrame;
import de.naoth.rc.logmanager.LogFileEventManager;
import java.io.EOFException;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.RandomAccessFile;
import java.util.ArrayList;
import java.util.HashMap;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.scene.Node;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.Slider;
import javafx.scene.input.MouseButton;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.Priority;
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

  private MediaPlayer player;
  private MediaView mediaView;
  private Slider frameSlider;
  private Slider timeSlider;
  private Text txtTime;
  private final FileChooser fileChooser = new FileChooser();

  private LogFile logfile;

  public VideoAnalyzer()
  {
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

    frameSlider = new Slider();
    frameSlider.valueProperty().addListener(new ChangeListener<Number>()
    {

      @Override
      public void changed(ObservableValue<? extends Number> observable, Number oldValue, Number newValue)
      {
        if (logfile != null && Plugin.logFileEventManager != null)
        {
          try {
            int frameNumber =  (int) frameSlider.getValue();
            HashMap<String,LogDataFrame> frame = logfile.readFrame(frameNumber);
            Plugin.logFileEventManager.fireLogFrameEvent(frame.values(), logfile.getFrameNumber(frameNumber));
          } catch (IOException ex) {
            Helper.handleException(ex);
          }
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

    HBox.setHgrow(frameSlider, Priority.ALWAYS);
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

    txtTime = new Text("mm:ss");

    return new HBox(btLoadLog, frameSlider, btLoadVideo, timeSlider, txtTime);
  }

  @Override
  public Scene createScene()
  {
//    Logger.setLevel(Logger.DEBUG);

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
        timeSlider.setMin(0.0);
        timeSlider.setMax(logfile.getFrameCount() - 1);
        //timeSlider.

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
        txtTime.setText(formatTime(newValue, true));
      }
    });

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
    double seconds
      = minutes == 0.0 ? elapsed.toSeconds()
        : elapsed.divide(minutes * 60000.0).toSeconds();
    if (withDecimal)
    {
      return String.format("%02d:%05.2f", (int) minutes, seconds);
    } else
    {
      return String.format("%02d:%02.0f", (int) minutes, seconds);
    }
  }

  private class LogFile
  {

    private int minFrame = Integer.MAX_VALUE;
    private int maxFrame = Integer.MIN_VALUE;
    private ArrayList<Frame> frameList = new ArrayList<Frame>();
    private HashMap<Integer, Integer> framePosition = new HashMap<Integer, Integer>();
    private BasicReader reader = null;

    public LogFile(File file) throws IOException
    {
      //FileInputStream file_input = new FileInputStream (file);
      //DataInputStream inputStream = new DataInputStream (file_input);
      reader = new BasicReader(new RandomAccessFile(file, "r"));
      scan(reader);
      //this.raf = new RandomAccessFile(this.openedFile, "r");
    }

    private void scan(BasicReader data_in) throws IOException
    {
      int currentFrameNumber = -1;
      int currentFrameSize = 0;
      int currentFramePos = 0;

      while (true)
      {
        try
        {
          int fragmentFrameSize = 0;

          int frameNumber = data_in.readInt();
          fragmentFrameSize += 4;

          // plausibility check
          if (currentFrameNumber > -1 && (frameNumber < currentFrameNumber || frameNumber < 0))
          {
            throw new IOException("corrupt frame number: " + frameNumber + " after " + currentFrameNumber);
          }

          if (frameNumber - currentFrameNumber > 30)
          {
            System.out.println("frame jump: " + currentFrameNumber + " -> " + frameNumber);
          }

          String currentName = data_in.readString();
          fragmentFrameSize += currentName.length() + 1;

          int currentSize = data_in.readInt();
          fragmentFrameSize += 4;
          fragmentFrameSize += currentSize;

          this.minFrame = Math.min(this.minFrame, frameNumber);
          this.maxFrame = Math.max(this.maxFrame, frameNumber);

          if (currentFrameNumber != frameNumber && currentFrameNumber != -1)
          {
            Frame frame = new Frame(currentFrameNumber, currentFrameSize, currentFramePos);

            framePosition.put(frameList.size(), currentFrameNumber);
            frameList.add(frame);

            currentFramePos += currentFrameSize;
            currentFrameSize = 0;
          }//end if

          currentFrameSize += fragmentFrameSize;
          currentFrameNumber = frameNumber;

          long skippedSize = data_in.skip(currentSize);
        } catch (EOFException eof)
        {
          System.out.println("End of File");
          break;
        }
      }//end while
    }//end parseLogFile

    private HashMap<String, LogDataFrame> readFrame(int frameId) throws IOException
    {
      Frame frame = this.frameList.get(frameId);
      if (frame == null)
      {
        return null;
      }

      // jump to the begin of the frame
      reader.seek(frame.position);

      int numberOfReadBytes = 0;

      HashMap<String, LogDataFrame> currentFrame = new HashMap<>();

      while (numberOfReadBytes < frame.size)
      {
        int frameNumber = reader.readInt();
        numberOfReadBytes += 4;

        if (frameNumber != frame.number)
        {
          throw new IOException("corrupt frame number: " + frameNumber + " expected " + frame.number);
        }

        String currentName = reader.readString();
        numberOfReadBytes += currentName.length() + 1;
        int currentSize = reader.readInt();
        numberOfReadBytes += 4;

        byte[] buffer = new byte[currentSize];
        numberOfReadBytes += reader.read(buffer);

        LogDataFrame logDataFrame = new LogDataFrame(frameNumber, currentName, buffer);
        currentFrame.put(currentName, logDataFrame);
      }//end while

      return currentFrame;
    }//end readFrame

    class Frame
    {

      final int number;
      final int size;
      final int position;

      public Frame(int number, int size, int position)
      {
        this.number = number;
        this.size = size;
        this.position = position;
      }
    }//end class Frame

    public int getMinFrame()
    {
      return minFrame;
    }

    public int getMaxFrame()
    {
      return maxFrame;
    }

    public int getFrameCount()
    {
      return frameList.size();
    }

    public int getFrameNumber(int n)
    {
      Frame frame = this.frameList.get(n);
      if (frame == null)
      {
        return -1;
      }
      return frame.number;
    }
  }

  class BasicReader
  {

    private InputStream streamReader = null;
    private RandomAccessFile rafReader = null;

    public BasicReader(InputStream streamReader)
    {
      this.streamReader = streamReader;
    }

    public BasicReader(RandomAccessFile rafReader)
    {
      this.rafReader = rafReader;
    }

    public int read(byte[] buffer) throws IOException
    {
      return (streamReader == null) ? this.rafReader.read(buffer) : this.streamReader.read(buffer);
    }

    public int read() throws IOException
    {
      return (streamReader == null) ? this.rafReader.read() : this.streamReader.read();
    }

    public long skip(int size) throws IOException
    {
      return (streamReader == null) ? this.rafReader.skipBytes(size) : this.streamReader.skip(size);
    }

    public void seek(long size) throws IOException
    {
      this.rafReader.seek(size);
    }

    public String readString() throws IOException, EOFException
    {
      StringBuilder sb = new StringBuilder();
      int c = this.read();
      while (((char) c) != '\0')
      {
        if (c == -1)
        {
          throw new EOFException();
        }
        sb.append((char) c);
        c = this.read();
      }

      return sb.toString();
    }

    public int readInt() throws IOException
    {
      byte[] buffer = new byte[4];
      int numberOfBytes = this.read(buffer);

      if (numberOfBytes != 4)
      {
        throw new EOFException();
      }

      int result
        = (0xff000000 & buffer[3] << 32)
        | (0x00ff0000 & buffer[2] << 16)
        | (0x0000ff00 & buffer[1] << 8)
        | (0x000000ff & buffer[0]);

      return result;
    }
  }//end class BasicReader

}
