package de.hu_berlin.informatik.ki.nao.server;


/**
 * Implemented by every class that wants to communicate with
 * the robot using the simple "command-response" schema.
 * @author thomas
 */
public interface CommandSender 
{
  /**
   * Handle a respond from the robot.
   * @param result The result.
   * @param originalCommand The command which was sent to the robot.
   */
  public void handleResponse(byte[] result, Command originalCommand);
  
  /**
   * Handle an error that occured when communicating to the robot.
   * @param code The error code that was sent by the robot.
   */
  public void handleError(int code);
  
  /**
   * Needed when sending a command repetitively to the robot.
   * @return The command string that should be used.
   */
  public Command getCurrentCommand();
  
}
