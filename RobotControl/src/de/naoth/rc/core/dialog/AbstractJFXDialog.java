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
        scene.getAccelerators().putAll(getGlobalShortcuts());
        if (getTheme() != null) { scene.getStylesheets().add(getTheme()); }
        afterInit();
      }
    });
  }

  public abstract URL getFXMLRessource();
  
  /**
   * Indicates, whether the dialog itself acts as JavaFx-Controller or if a separate controller exists.
   * Extending classes should override this method and return 'true' if the dialog should be the
   * javafx controller.
   * 
   * @return true, if this dialog acts as javafx controller, false otherwise
   */
  protected boolean isSelfController() {
      return false;
  }
  
  /**
   * Gets called, after the fxml file was loaded and an appropiate scene is created.
   * This method is especially usefull, if the dialog itself act as javafx controller. When this
   * method is called, all javafx elements are initialized and can be used for setting up bindings, 
   * action listeners and other stuff, which needs accessing the javafx components.
   */
  public void afterInit() {
  }

  private Scene createScene()
  {
//    com.sun.media.jfxmedia.logging.Logger.setLevel(com.sun.media.jfxmedia.logging.Logger.DEBUG);

    try
    {
      loader = new FXMLLoader(getFXMLRessource());
      loader.setClassLoader(AbstractJFXDialog.class.getClassLoader());
      // checks if this dialog should be the controller for this fxml file
      if(isSelfController()) {
        loader.setController(this);
      }
      loader.load();

      Scene scene = new Scene(loader.getRoot());
      
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

  protected Scene getScene() {
      return container.getScene();
  }

  /**
   * Returns the theme stylesheet, which should be used or null, if there's no
   * specific theme.
   *
   * This must be overwritten by the child classes, if they want to use the global
   * theme!
   *
   * @return path to the themes stylesheet file
   */
  protected String getTheme() {
    return null;
  }
}
