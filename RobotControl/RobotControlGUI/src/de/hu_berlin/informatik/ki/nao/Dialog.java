/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.hu_berlin.informatik.ki.nao;

import javax.swing.JPanel;

/**
 * Interface for all dialogs
 * @author thomas
 */
public interface Dialog 
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
   * Here the all the registered listener shold be unregistered.
   */
  public void dispose();
}
