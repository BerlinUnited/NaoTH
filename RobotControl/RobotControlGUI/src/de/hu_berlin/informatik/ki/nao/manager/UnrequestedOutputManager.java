/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.hu_berlin.informatik.ki.nao.manager;

import de.hu_berlin.informatik.ki.nao.server.Command;
import de.hu_berlin.informatik.ki.nao.server.MessageServer;

/**
 *
 * @author thomas
 */
public class UnrequestedOutputManager extends AbstractManager<String>
{
  
  public UnrequestedOutputManager(MessageServer server)
  {
    super(server);
  }

  
  @Override
  public String convertByteArrayToType(byte[] result) throws IllegalArgumentException
  {
    return new String(result);
  }

  
  
  public Command getCurrentCommand()
  {
    return new Command("latest_output_messages");
  }

}//end class UnrequestedOutputManager
