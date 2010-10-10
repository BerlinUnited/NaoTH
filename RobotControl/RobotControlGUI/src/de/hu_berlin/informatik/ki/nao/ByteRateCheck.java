/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.hu_berlin.informatik.ki.nao;

import de.hu_berlin.informatik.ki.nao.interfaces.ByteRateUpdateHandler;
import de.hu_berlin.informatik.ki.nao.interfaces.MessageServerProvider;
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
  public MessageServerProvider messageServer;
  @InjectPlugin
  public ByteRateUpdateHandler updateHandler;

  long oldRecivedSize = 0;
  long oldSentSize = 0;


  @Timer(period=1000)
  public void updateByteRateLabels()
  {
    if(updateHandler != null && messageServer != null && messageServer.getMessageServer() != null)
    {
      MessageServer server = messageServer.getMessageServer();

      double recivedKB = ((double)(server.getReceivedBytes()-oldRecivedSize))/1024.0;
      double sentKB = ((double)(server.getSentBytes()-oldSentSize))/1024.0;

      updateHandler.setReceiveByteRate(recivedKB);
      updateHandler.setSentByteRate(sentKB);
    }
  }

}
