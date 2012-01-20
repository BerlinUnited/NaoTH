/*
 * 
 */

package de.naoth.rc.manager;

import de.naoth.rc.RobotControl;
import de.naoth.rc.server.MessageServer;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 *
 * @author Heinrich Mellmann
 */
public abstract class AbstractManagerPlugin<T> extends AbstractManager<T>
{
  @InjectPlugin
  public RobotControl robotControl;

  @Override
  public MessageServer getServer()
  {
    if(robotControl == null)
    {
      return null;
    }
    else
    {
      return robotControl.getMessageServer();
    }
  }
}//end AbstractManagerPlugin
