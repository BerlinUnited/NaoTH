/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.naoth.rc.core.dialog;

import java.util.logging.Logger;
import javax.swing.JPanel;

/**
 *
 * @author thomas
 */
public class AbstractDialog extends JPanel implements Dialog
{
  @Override
  public JPanel getPanel()
  {
      return this;
  }

  @Override
  public String getDisplayName()
  {
    return this.getClass().getSimpleName();
  }

  @Override
  public void dispose()
  {
    // do nothing per default
  }
  
  @Override
  public void destroy()
  {
    // do nothing per default
  }
  
  
  public void init()
  {
      
  }
  
  // return a logger for this dialog
  protected final Logger getLogger() {
      return Logger.getLogger(this.getClass().getName());
  }
  
  @Override
  public String getCategory() { 
    return "Dialog";
  }

}//end AbstractDialog
