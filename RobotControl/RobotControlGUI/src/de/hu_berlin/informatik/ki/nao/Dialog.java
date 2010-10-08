/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.hu_berlin.informatik.ki.nao;

import javax.swing.JPanel;
import net.xeoh.plugins.base.Plugin;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 * Interface for all dialogs
 * @author thomas
 */
public abstract class Dialog extends javax.swing.JPanel implements Plugin
{

  /** 
   * Inits the dialog
   * 
   * @param parent The application "parent"
   */
  public abstract void init(Main parent);
  
  /**
   * Returns this as panel.<br><br>
   * 
   * Mostly this just should return <code>this</code>
   * @return
   */
  public JPanel getPanel()
  {
    return this;
  }

  /**
   * This method is called when the dialog is closed.
   * Here the all the registered listener shold be unregistered.
   */
  public abstract void dispose();
}
