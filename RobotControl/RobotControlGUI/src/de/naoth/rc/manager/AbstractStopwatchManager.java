/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package de.naoth.rc.manager;

import com.google.protobuf.InvalidProtocolBufferException;
import de.naoth.rc.messages.Messages;
import java.util.HashMap;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author Thomas Krause <krauseto@hu-berlin.de>
 */
public abstract class AbstractStopwatchManager extends AbstractManagerPlugin<HashMap<String, Integer>>
{

  public AbstractStopwatchManager()
  {
  }

  @Override
  public HashMap<String, Integer> convertByteArrayToType(byte[] result) throws IllegalArgumentException
  {
    HashMap<String, Integer> map = new HashMap<String, Integer>();
    try
    {
      Messages.Stopwatches stopwatches = Messages.Stopwatches.parseFrom(result);
      for(Messages.StopwatchItem i : stopwatches.getStopwatchesList())
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
  
}
