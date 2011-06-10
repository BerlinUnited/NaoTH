/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.hu_berlin.informatik.ki.nao;

import de.hu_berlin.informatik.ki.nao.interfaces.ByteRateUpdateHandler;
import de.hu_berlin.informatik.ki.nao.server.MessageServer;
import net.xeoh.plugins.base.Plugin;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.Timer;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 *
 * @author thomas
 */
@PluginImplementation
public class ByteRateCheck implements Plugin
{

  @InjectPlugin
  public RobotControl robotControl;
  @InjectPlugin
  public ByteRateUpdateHandler updateHandler;

  long oldRecivedSize = 0;
  long oldSentSize = 0;


  @Timer(period=1000)
  public void updateByteRateLabels()
  {
    if(updateHandler != null && robotControl != null && robotControl.getMessageServer() != null)
    {
      MessageServer server = robotControl.getMessageServer();

      double receivedKB = ((double)(server.getReceivedBytes()-oldRecivedSize))/1024.0;
      double sentKB = ((double)(server.getSentBytes()-oldSentSize))/1024.0;

      oldRecivedSize = server.getReceivedBytes();
      oldSentSize = server.getSentBytes();

      updateHandler.setReceiveByteRate(receivedKB);
      updateHandler.setSentByteRate(sentKB);
    }
  }

}//end ByteRateCheck
