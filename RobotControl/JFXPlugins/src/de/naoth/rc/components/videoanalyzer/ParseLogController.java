/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc.components.videoanalyzer;

import de.naoth.rc.Helper;
import de.naoth.rc.components.behaviorviewer.XABSLBehaviorFrame;
import de.naoth.rc.components.behaviorviewer.XABSLProtoParser;
import de.naoth.rc.components.behaviorviewer.model.Symbol;
import de.naoth.rc.dataformats.LogFile;
import de.naoth.rc.logmanager.LogDataFrame;
import de.naoth.rc.messages.FrameworkRepresentations;
import de.naoth.rc.messages.Messages;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;
import java.net.URL;
import java.util.ArrayList;
import java.util.Map;
import java.util.ResourceBundle;
import java.util.TreeMap;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.concurrent.Service;
import javafx.concurrent.Task;
import javafx.concurrent.WorkerStateEvent;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.ProgressBar;
import javafx.stage.Stage;

/**
 * FXML Controller class
 *
 * @author thomas
 */
public class ParseLogController implements Initializable
{

  private VideoAnalyzerController parent;
  private File rawFile;

  @FXML
  private ProgressBar progress;

  @FXML
  private Button btCancel;

  @FXML
  private Label lblMessage;

  private Service<ParseResult> service;

  /**
   * Initializes the controller class.
   */
  @Override
  public void initialize(URL url, ResourceBundle rb)
  {
    service = new Service<ParseResult>()
    {

      @Override
      protected Task<ParseResult> createTask()
      {
        return new Parse();
      }
    };
    progress.progressProperty().bind(service.progressProperty());
    lblMessage.textProperty().bind(service.messageProperty());
    service.start();
    service.setOnCancelled(new EventHandler<WorkerStateEvent>()
    {

      @Override
      public void handle(WorkerStateEvent event)
      {
        Stage stage = (Stage) lblMessage.getScene().getWindow();
        stage.close();
      }
    });
    service.setOnSucceeded(new EventHandler<WorkerStateEvent>()
    {

      @Override
      public void handle(WorkerStateEvent event)
      {
        parent.setLogfile(service.getValue().logfile);
        parent.setParseResult(service.getValue().time2LogFrame,
          service.getValue().logFrame2Time);
        parent.setGameStateChanges(service.getValue().changes);

        Stage stage = (Stage) lblMessage.getScene().getWindow();
        stage.close();
      }
    });

  }

  @FXML
  private void handleCancel(ActionEvent event)
  {
    service.cancel();
  }

  private class Parse extends Task<ParseResult>
  {

    @Override
    protected ParseResult call() throws Exception
    {
      ParseResult result = new ParseResult();
      try
      {
        File cachedFile = new File(rawFile.getParent(), rawFile.getName() + ".parsed");
        ParseResult cachedResult = getCachedParseResult(cachedFile);
        if (cachedResult == null)
        {
          // parse the log file
          updateMessage("Opening file");
          result.logfile = new LogFile(rawFile.getAbsolutePath());
          updateMessage("Mapping frames");
          updateProgress(0.0, result.logfile.getFrameCount());
          initFrameMap(result);

          // cache it
          updateMessage("Saving cached parsed log file");
          updateProgress(-1, -1);
          try (ObjectOutputStream oo = new ObjectOutputStream(new BufferedOutputStream(new FileOutputStream(cachedFile))))
          {
            oo.writeObject(result);

          } catch (Exception ex)
          {
            Logger.getLogger(ParseLogController.class.getName()).log(Level.WARNING, "Could not write cached parsed log file.", ex);
          }

        } else
        {
          result = cachedResult;
        }

      } catch (IOException ex)
      {
        Helper.handleException("Could not open logfile " + rawFile.getAbsolutePath(), ex);
      }
      return result;
    }

    private ParseResult getCachedParseResult(File cachedFile)
    {
      if (cachedFile.isFile())
      {
        // we have a cached parsed version, load it
        try (ObjectInputStream oi = new ObjectInputStream(new FileInputStream(cachedFile)))
        {
          updateMessage("Reading cached filed");
          return (ParseResult) oi.readObject();
        } catch (Exception ex)
        {
          Logger.getLogger(ParseLogController.class.getName()).log(Level.WARNING, "Could not read cached parsed log file.", ex);
        }
      }
      return null;
    }

    private void initFrameMap(ParseResult result)
    {
      result.time2LogFrame.clear();

      XABSLProtoParser parser = new XABSLProtoParser();

      if (result.logfile != null)
      {
        try
        {
          String lastGameState = "";
          for (int i = 0; i < result.logfile.getFrameCount(); i++)
          {
            Double currentTime = null;
            Map<String, LogDataFrame> messages = result.logfile.readFrame(i);
            LogDataFrame frameRaw = messages.get("FrameInfo");
            if (frameRaw != null)
            {
              FrameworkRepresentations.FrameInfo frame = FrameworkRepresentations.FrameInfo.parseFrom(frameRaw.getData());
              currentTime = ((double) frame.getTime()) / 1000.0;
              result.time2LogFrame.put(currentTime, i);
              result.logFrame2Time.put(i, currentTime);

              if (i == 0)
              {
                result.firstTime = currentTime;
              }
            }
            XABSLBehaviorFrame behaviorFrame = null;
            // parse any behavior related messages
            LogDataFrame completeStatusRaw = messages.get("BehaviorStateComplete");
            if (completeStatusRaw != null)
            {
              Messages.BehaviorStateComplete completeStatus = Messages.BehaviorStateComplete.parseFrom(completeStatusRaw.getData());
              parser.parseComplete(completeStatus);
            }
            LogDataFrame statusRaw = messages.get("BehaviorStateSparse");
            if (statusRaw != null)
            {
              Messages.BehaviorStateSparse status = Messages.BehaviorStateSparse.parseFrom(statusRaw.getData());
              behaviorFrame = parser.parseSparse(status);
            }
            // try to detect game state changes 
            if (behaviorFrame != null && currentTime != null)
            {
              Symbol s = behaviorFrame.getSymbolByName("game.state");
              if (s != null)
              {
                if (!lastGameState.equals(s.getValueAsString()))
                {
                  // CHANGE DETECTED!
                  VideoAnalyzer.GameStateChange change = new VideoAnalyzer.GameStateChange();
                  change.time = currentTime;
                  change.state = s.getValueAsString();
                  result.changes.add(change);

                  lastGameState = s.getValueAsString();

                }
              }
            }
            updateProgress(i, result.logfile.getFrameCount());
          } // end for each frame
        } catch (IOException ex)
        {
          Logger.getLogger(VideoAnalyzer.class.getName()).log(Level.SEVERE, null, ex);
        }
      }
    }

  }

  public void setParent(VideoAnalyzerController parent)
  {
    this.parent = parent;
  }

  public void setLogFile(File f)
  {
    this.rawFile = f;
  }

}
