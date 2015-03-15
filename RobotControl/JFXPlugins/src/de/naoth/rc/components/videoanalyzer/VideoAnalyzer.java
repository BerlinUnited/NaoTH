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
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.fxml.FXMLLoader;
import javafx.scene.Scene;
import javafx.scene.control.Label;
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
  

  public VideoAnalyzer()
  {
    
  }


  @Override
  public Scene createScene()
  {
//    com.sun.media.jfxmedia.logging.Logger.setLevel(com.sun.media.jfxmedia.logging.Logger.DEBUG);
    
    try
      {
        FXMLLoader loader = new FXMLLoader(VideoPlayerController.class.getResource("VideoAnalyzer.fxml"));
        loader.setClassLoader(VideoAnalyzer.class.getClassLoader());
        loader.load();
        
        Scene scene = new Scene(loader.getRoot());
        loader.<VideoAnalyzerController>getController().initAccelerators(scene);
        return scene;

        
      } catch (IOException ex)
      {
        Logger.getLogger(VideoAnalyzer.class.getName()).log(Level.SEVERE, null, ex);
      }
    return new Scene(new Label("Loading failed"));
  }
 
}
