/*
 * 
 */
package de.hu_berlin.informatik.ki.nao.manager;

import de.hu_berlin.informatik.ki.nao.server.Command;

/**
 *
 * @author mellmann
 */
public class GenericManager extends AbstractManager<byte[]>
{

  private final Command command;

  public GenericManager(Command command)
  {
    super();
    this.command = command;
  }

  @Override
  public byte[] convertByteArrayToType(byte[] result) throws IllegalArgumentException
  {
    return result;
  }

  public Command getCurrentCommand()
  {
    return this.command;
  }
}//end class GenericManager

