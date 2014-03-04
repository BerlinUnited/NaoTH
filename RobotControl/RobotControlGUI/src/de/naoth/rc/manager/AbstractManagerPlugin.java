/*
 * 
 */

package de.naoth.rc.manager;

import de.naoth.rc.RobotControl;
import de.naoth.rc.server.MessageServer;
import net.xeoh.plugins.base.annotations.events.PluginLoaded;

/**
 *
 * @author Heinrich Mellmann
 */
public abstract class AbstractManagerPlugin<T> extends AbstractManager<T> implements Manager<T>
{
  private MessageServer messageServer;
  
  @PluginLoaded
  public void setServer(RobotControl robotControl) {
      this.messageServer = robotControl.getMessageServer();
  }

  @Override
  public MessageServer getServer()
  {
    return messageServer;
  }
}//end AbstractManagerPlugin
