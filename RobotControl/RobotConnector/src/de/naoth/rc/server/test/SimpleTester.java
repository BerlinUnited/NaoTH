/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc.server.test;

import de.naoth.rc.server.Command;
import de.naoth.rc.server.CommandSender;
import de.naoth.rc.server.MessageServer;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author thomas
 */
public class SimpleTester
{

  public SimpleTester()
  {
    MessageServer server = new MessageServer();
    try
    {
      server.connect("localhost", 5401);
      server.addCommandSender(new TestSender());
      try
      {
        Thread.sleep(5000000);

        server.disconnect();

      }
      catch (InterruptedException ex)
      {
        Logger.getLogger(SimpleTester.class.getName()).log(Level.SEVERE, null, ex);
      }

    }
    catch (IOException ex)
    {
      Logger.getLogger(SimpleTester.class.getName()).log(Level.SEVERE, null, ex);
    }
  }

  /**
   * @param args the command line arguments
   */
  public static void main(String[] args)
  {
    SimpleTester tester = new SimpleTester();
  }

  public class TestSender implements CommandSender
  {

    public void handleResponse(byte[] result, Command originalCommand)
    {
      System.out.println("got a response:");
      System.out.println(new String(result));
    }

    public void handleError(int code)
    {
      System.out.println("ok...");
    }

    public Command getCurrentCommand()
    {
      Command c = new Command();
      c.setName("image");
      return c;
    }
    
  }

}
