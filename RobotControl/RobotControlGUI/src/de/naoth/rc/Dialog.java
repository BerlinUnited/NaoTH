/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.naoth.rc;

import javax.swing.JPanel;
import net.xeoh.plugins.base.Plugin;

/**
 * Interface for all dialogs
 * @author thomas
 */
public interface Dialog extends Plugin
{
  
  /**
   * Returns this as panel.<br><br>
   * 
   * Mostly this just should return <code>this</code>
   * @return
   */
  public JPanel getPanel();

  /**
   * Get the name that should be used e.g. for the menu entries.
   * @return
   */
  public String getDisplayName();

  /**
   * This method is called when the dialog is closed.
   * Here the all the registered listener should be unregistered.
   */
  public void dispose();
  
  
  public void destroy();

}
