package de.naoth.rc.manager;

import de.naoth.rc.server.Command;
import net.xeoh.plugins.base.annotations.PluginImplementation;

/**
 * A manager for stopwatches
 * @author thomas
 */
@PluginImplementation
public class StopwatchManagerImpl extends AbstractStopwatchManager
  implements StopwatchManager
{
  
  public StopwatchManagerImpl()
  {
  }


  @Override
  public Command getCurrentCommand()
  {
    return new Command("Cognition:representation:getbinary").addArg("StopwatchManager");
  }

}//end class StopwatchManagerImpl
