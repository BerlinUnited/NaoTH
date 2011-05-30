/*
 * 
 */

package de.hu_berlin.informatik.ki.nao.manager;

import de.hu_berlin.informatik.ki.nao.RobotControl;
import de.hu_berlin.informatik.ki.nao.server.Command;
import de.hu_berlin.informatik.ki.nao.server.MessageServer;
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

  private HashMap<Command, GenericManager> mapOfGenericManager;


  public GenericManagerFactoryImpl()
  {
    this.mapOfGenericManager = new HashMap<Command, GenericManager>();
  }

  @Override
  public GenericManager getManager(Command command)
  {
    if(mapOfGenericManager.get(command) == null)
      mapOfGenericManager.put(command, new GenericManager(getServer(), command));
    return mapOfGenericManager.get(command);
  }//end getManager

  
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
  }//end getServer
}//end GenericManagerFactoryImpl
