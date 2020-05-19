package de.naoth.rc.server;


/**
 * Implemented by every class that wants to communicate with
 * the robot using the simple "command-response" schema.
 * @author thomas
 */
@Deprecated
public interface CommandSender extends ResponseListener
{  
  /**
   * Needed when sending a command repetitively to the robot.
   * @return The command string that should be used.
   */
  public Command getCurrentCommand();
  
}
