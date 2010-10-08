/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package de.hu_berlin.informatik.ki.nao.server.test;

import de.hu_berlin.informatik.ki.nao.server.Command;
import de.hu_berlin.informatik.ki.nao.server.CommandSender;
import de.hu_berlin.informatik.ki.nao.server.MessageServer;
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
      throw new UnsupportedOperationException("Not supported yet.");
    }

    public Command getCurrentCommand()
    {
      Command c = new Command();
      c.setName("ping");
      return c;
    }
    
  }

}
