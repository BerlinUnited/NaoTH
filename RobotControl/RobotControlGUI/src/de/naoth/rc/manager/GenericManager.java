/*
 * 
 */
package de.naoth.rc.manager;

import de.naoth.rc.core.manager.AbstractManager;
import de.naoth.rc.server.Command;
import de.naoth.rc.server.MessageServer;

/**
 *
 * @author mellmann
 */
public class GenericManager extends AbstractManager<byte[]>
{

  private final Command command;
  private final MessageServer messageServer;

  public GenericManager(MessageServer messageServer, Command command)
  {
    super();
    this.command = command;
    this.messageServer = messageServer;
  }

  @Override
  public byte[] convertByteArrayToType(byte[] result) throws IllegalArgumentException
  {
    return result;
  }

  @Override
  public Command getCurrentCommand()
  {
    return this.command;
  }

  @Override
  public MessageServer getServer()
  {
    return this.messageServer;
  }
}//end class GenericManager

