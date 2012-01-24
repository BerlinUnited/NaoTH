package de.naoth.rc.manager;

import com.google.protobuf.InvalidProtocolBufferException;
import de.naoth.rc.messages.Messages.StopwatchItem;
import de.naoth.rc.messages.Messages.Stopwatches;
import de.naoth.rc.server.Command;
import java.util.HashMap;
import java.util.logging.Level;
import java.util.logging.Logger;
import net.xeoh.plugins.base.annotations.PluginImplementation;

/**
 * A manager for stopwatches
 * @author thomas
 */
@PluginImplementation
public class StopwatchManagerImpl extends AbstractManagerPlugin<HashMap<String, Integer>>
  implements StopwatchManager
{
  
  public StopwatchManagerImpl()
  {
  }

  @Override
  public HashMap<String, Integer> convertByteArrayToType(byte[] result) throws IllegalArgumentException
  {
    HashMap<String, Integer> map = new HashMap<String, Integer>();
    try
    {
      Stopwatches stopwatches = Stopwatches.parseFrom(result);
      for(StopwatchItem i : stopwatches.getStopwatchesList())
      {
        map.put(i.getName(), i.getTime());
      }
    }
    catch(InvalidProtocolBufferException ex)
    {
      Logger.getLogger(StopwatchManagerImpl.class.getName()).log(Level.SEVERE, null, ex);
    }

    return map;
  }

  @Override
  public Command getCurrentCommand()
  {
    return new Command("stopwatch");
  }

}//end class StopwatchManagerImpl
