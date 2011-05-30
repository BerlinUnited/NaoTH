/*
 * 
 */

package de.hu_berlin.informatik.ki.nao.manager;

import de.hu_berlin.informatik.ki.nao.RobotControl;
import de.hu_berlin.informatik.ki.nao.server.MessageServer;
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
