package de.naoth.rc.manager;

import de.naoth.rc.server.Command;
import net.xeoh.plugins.base.annotations.PluginImplementation;

/**
 * A manager for stopwatches
 * @author thomas
 */
@PluginImplementation
public class ModuleStopwatchManagerImpl extends AbstractStopwatchManager
  implements ModuleStopwatchManager
{
  
  public ModuleStopwatchManagerImpl()
  {
  }

  @Override
  public Command getCurrentCommand()
  {
    return new Command("Cognition:modules:stopwatch");
  }

}//end class StopwatchManagerImpl
