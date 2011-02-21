package de.hu_berlin.informatik.ki.nao.manager;

import com.google.protobuf.InvalidProtocolBufferException;
import de.hu_berlin.informatik.ki.nao.messages.Messages.StopwatchItem;
import de.hu_berlin.informatik.ki.nao.messages.Messages.Stopwatches;
import de.hu_berlin.informatik.ki.nao.server.Command;
import java.util.HashMap;
import java.util.logging.Level;
import java.util.logging.Logger;
import net.xeoh.plugins.base.annotations.PluginImplementation;

/**
 * A manager for stopwatches
 * @author thomas
 */
@PluginImplementation
public class StopwatchManager extends AbstractManager<HashMap<String, Integer>>
{
  
  public StopwatchManager()
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
      Logger.getLogger(StopwatchManager.class.getName()).log(Level.SEVERE, null, ex);
    }

    return map;
  }

  @Override
  public Command getCurrentCommand()
  {
    return new Command("stopwatch");
  }
  
}
