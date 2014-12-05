/*
 * 
 */

package de.naoth.rc.manager;

import de.naoth.rc.RobotControl;
import de.naoth.rc.server.Command;
import de.naoth.rc.server.MessageServer;
import java.util.HashMap;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 *
 * @author Heinrich Mellmann
 */
@PluginImplementation
public class GenericManagerFactoryImpl implements GenericManagerFactory
{
  @InjectPlugin
  public RobotControl robotControl;

  private final HashMap<Command, GenericManager> mapOfGenericManager;

  public GenericManagerFactoryImpl()
  {
    this.mapOfGenericManager = new HashMap<Command, GenericManager>();
  }

  @Override
  public GenericManager getManager(Command command)
  {
    if(mapOfGenericManager.get(command) == null) {
      mapOfGenericManager.put(command, new GenericManager(getServer(), command));
    }
    return mapOfGenericManager.get(command);
  }

  
  public MessageServer getServer()
  {
    if(robotControl == null) {
      return null;
    } else {
      return robotControl.getMessageServer();
    }
  }
}//end GenericManagerFactoryImpl
