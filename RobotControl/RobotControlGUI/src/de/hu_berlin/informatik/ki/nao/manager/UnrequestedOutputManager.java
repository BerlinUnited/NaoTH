/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.hu_berlin.informatik.ki.nao.manager;

import de.hu_berlin.informatik.ki.nao.server.Command;
import net.xeoh.plugins.base.Plugin;
import net.xeoh.plugins.base.annotations.PluginImplementation;

/**
 *
 * @author thomas
 */
@PluginImplementation
public class UnrequestedOutputManager extends AbstractManager<String> implements Plugin
{

  
  @Override
  public String convertByteArrayToType(byte[] result) throws IllegalArgumentException
  {
    return new String(result);
  }
  
  @Override
  public Command getCurrentCommand()
  {
    return new Command("latest_output_messages");
  }

}//end class UnrequestedOutputManager
