package de.naoth.rc.server;


/**
 * Implemented by every class that wants to communicate with
 * the robot using the simple "command-response" schema.
 * @author thomas
 */
public interface ResponseListener 
{
  /**
   * Handle a respond from the robot.
   * @param result The result.
   * @param command The command which was sent to the robot.
   */
  public void handleResponse(byte[] result, Command command);
  
  /**
   * Handle an error that occurred when communicating to the robot.
   * @param code The error code that was sent by the robot.
   */
  public void handleError(int code);
}
