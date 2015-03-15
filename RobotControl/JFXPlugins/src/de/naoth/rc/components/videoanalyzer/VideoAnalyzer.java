/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc.components.videoanalyzer;

import de.naoth.rc.RobotControl;
import de.naoth.rc.core.dialog.AbstractJFXDialog;
import de.naoth.rc.core.dialog.DialogPlugin;
import de.naoth.rc.core.manager.SwingCommandExecutor;
import de.naoth.rc.logmanager.LogFileEventManager;
import java.io.Serializable;
import java.net.URL;
import java.util.HashMap;
import java.util.Map;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyCodeCombination;
import javafx.scene.input.KeyCombination;
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

  public static class GameStateChange implements Serializable
  {

    public double time;
    public String state;

    @Override
    public String toString()
    {
      return String.format("%s (at %.0f sec)", state, time);
    }
  }

  public final static String KEY_VIDEO_FILE = "video-file";
  public final static String KEY_SYNC_TIME_VIDEO = "sync-time-video";
  public final static String KEY_SYNC_TIME_LOG = "sync-time-log";

  public VideoAnalyzer()
  {

  }

  @Override
  public URL getFXMLRessource()
  {
    return VideoPlayerController.class.getResource("VideoAnalyzer.fxml");
  }

  @Override
  public Map<KeyCombination, Runnable> getGlobalShortcuts()
  {
    HashMap<KeyCombination, Runnable> result = new HashMap<>();

    Runnable togglePlayRunnable = new Runnable()
    {

      @Override
      public void run()
      {
        VideoAnalyzerController c = VideoAnalyzer.this.<VideoAnalyzerController>getController();
        c.togglePlay();
      }
    };

    result.put(new KeyCodeCombination(KeyCode.SPACE), togglePlayRunnable);
    result.put(new KeyCodeCombination(KeyCode.P), togglePlayRunnable);

    return result;
  }

}
