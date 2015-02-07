/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc.core.dialog;

import java.awt.BorderLayout;
import javafx.application.Platform;
import javafx.embed.swing.JFXPanel;
import javafx.scene.Scene;

/**
 *
 * @author thomas
 */
public abstract class AbstractJFXDialog extends AbstractDialog implements Dialog
{
  private final JFXPanel container = new JFXPanel();
  
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
  
  public abstract Scene createScene();

  @Override
  public void init()
  {
    initFX();
  }
  
}
