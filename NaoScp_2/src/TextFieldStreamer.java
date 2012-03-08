/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

import java.awt.event.*;
import java.io.*;
import javax.swing.*;


/**
 *
 * @author claas
 */
public class TextFieldStreamer extends InputStream implements ActionListener 
{

  private JTextField tf;
  private String str;
  private int pos;
  private boolean triggered;
  private boolean stopWork;

  public TextFieldStreamer(JTextField jtf) 
  {
    tf = jtf;
    str = null;
    pos = 0;
    triggered = false;
    stopWork = false;
  }

  public boolean wasTriggered()
  {
    return triggered;
  }
  
  public void resetTriggered()
  {
    triggered = false;
  }
  
  //gets triggered everytime that "Enter" is pressed on the textfield
  @Override
  public void actionPerformed(ActionEvent e) 
  {
    str = tf.getText() + "\n";
    pos = 0;
    tf.setText("");    
    synchronized (this) 
    {      
      //maybe this should only notify() as multiple threads may
      //be waiting for input and they would now race for input
      this.notifyAll();
      triggered = true;
    }
  }

  @Override
  public int read() 
  {
    //test if the available input has reached its end
    //and the EOS should be returned 
    if(str != null && pos == str.length())
    {
      str = null;
      //this is supposed to return -1 on "end of stream"
      //but I'm having a hard time locating the constant
      return java.io.StreamTokenizer.TT_EOF;
    }
    //no input available, block until more is available because that's
    //the behavior specified in the Javadocs
    while (!stopWork && (str == null || pos >= str.length()) )
    {
      try 
      {
        //according to the docs read() should block until new input is available
        synchronized (this) 
        {          
          this.wait();
        }
      } 
      catch (InterruptedException ex) 
      {
        ex.printStackTrace();
      }
    }
    if(stopWork)
    {
      try
      {
        close();
      }
      catch(IOException e)
      {}
    }
    //read an additional character, return it and increment the index
    return str.charAt(pos++);
  }
  
  public synchronized void stop()
  {
    stopWork = true;
    this.notifyAll();
  }
  
}