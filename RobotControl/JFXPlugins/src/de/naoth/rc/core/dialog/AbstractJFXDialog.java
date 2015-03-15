/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc.core.dialog;

import java.awt.BorderLayout;
import java.io.IOException;
import java.net.URL;
import java.util.HashMap;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.application.Platform;
import javafx.embed.swing.JFXPanel;
import javafx.fxml.FXMLLoader;
import javafx.scene.Scene;
import javafx.scene.control.Label;
import javafx.scene.input.KeyCombination;

/**
 *
 * @author thomas
 */
public abstract class AbstractJFXDialog extends AbstractDialog implements Dialog
{

  private final JFXPanel container = new JFXPanel();
  private FXMLLoader loader;

  public AbstractJFXDialog()
  {
    Platform.setImplicitExit(false);

    setLayout(new BorderLayout());
    add(container, BorderLayout.CENTER);
  }

  private void initFX()
  {
    // JavaFX needs to run in its own thread environment
    Platform.runLater(new Runnable()
    {

      @Override
      public void run()
      {
        Scene scene = createScene();
        container.setScene(scene);
      }
    });
  }

  public abstract URL getFXMLRessource();

  private Scene createScene()
  {
    //    com.sun.media.jfxmedia.logging.Logger.setLevel(com.sun.media.jfxmedia.logging.Logger.DEBUG);

    try
    {
      loader = new FXMLLoader(getFXMLRessource());
      loader.setClassLoader(AbstractJFXDialog.class.getClassLoader());
      loader.load();

      Scene scene = new Scene(loader.getRoot());
      
      scene.getAccelerators().putAll(getGlobalShortcuts());
      
      return scene;

    } catch (IOException ex)
    {
      Logger.getLogger(AbstractJFXDialog.class.getName()).log(Level.SEVERE, null, ex);
    }
    return new Scene(new Label("Loading failed"));
  }

  public Map<KeyCombination, Runnable> getGlobalShortcuts()
  {
    return new HashMap<>();
  }

  @Override
  public void init()
  {
    initFX();
  }
  
  public<T> T getController()
  {
    if(loader != null)
    {
      return loader.<T>getController();
    }
    return null;
  }

}
