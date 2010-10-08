/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.hu_berlin.informatik.ki.nao;

import javax.swing.JPanel;
import net.xeoh.plugins.base.Plugin;
import net.xeoh.plugins.base.annotations.events.Init;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 * Interface for all dialogs
 * @author thomas
 */
public interface Dialog extends  Plugin
{


  /** 
   * Inits the dialog
   * 
   * @param parent The application "parent"
   */
  public void init(Main parent);
  
  /**
   * Returns this as panel.<br><br>
   * 
   * Mostly this just should return <code>this</code>
   * @return
   */
  public JPanel getPanel();

  /**
   * This method is called when the dialog is closed.
   * Here the all the registered listener should be unregistered.
   */
  public void dispose();
}
