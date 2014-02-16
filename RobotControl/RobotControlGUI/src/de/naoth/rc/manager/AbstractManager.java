/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.naoth.rc.manager;

import de.naoth.rc.server.Command;
import de.naoth.rc.server.CommandSender;
import de.naoth.rc.server.MessageServer;
import java.util.Collections;
import java.util.LinkedList;
import java.util.List;
import javax.swing.SwingUtilities;

/**
 * This is the basic class for every manager. It does a lot of work in order
 * to avoid deadlocks. It should be much easier to extend you manager from 
 * this class instead of implementing you own <code>CommandSender</code>. <br><br>
 * 
 * @param <T> The type <code>T</code> is the type of the (data) object that is managed.
 * @author thomas
 */
public abstract class AbstractManager<T> implements Manager<T>, CommandSender
{
  /**
   * When using this lock you should have understood the meaning of the
   * following caller-graph.<br>
   * <b>Be warned an don't produce dead-locks!</b>
   * 
   * <pre>
   * 
   * +-------------+            
   * |MessageServer|            
   * +-------------+      +-----+
   *  |^                +-| GUI |
   *  ||                | +-----+
   *  ||                | | GUI |
   *  ||                | +-----+
   *  v|                |    ^   
   * +--------+ &lt;----+    |   
   * |ManagerX| -------------+   
   * +--------+
   * 
   * </pre>
   */
  private final List<ObjectListener<T>> listener = 
    Collections.synchronizedList(new LinkedList<ObjectListener<T>>());

  public AbstractManager()
  {
  }
  
  @Override
  public void addListener(ObjectListener<T> l)
  {
    if(!listener.contains(l)) {
      listener.add(l);
    }
    if(listener.size() == 1) {
      getServer().subscribe(this);
    }
  }

  @Override
  public void removeListener(ObjectListener<T> l)
  {
    listener.remove(l);
    if(listener.isEmpty()) {
      getServer().unsubscribe(this);
    }
  }

  @Override
  public void handleResponse(final byte[] result, Command originalCommand)
  {
    SwingUtilities.invokeLater(new Runnable() {
        @Override
        public void run() {
            try
            {
              final T object = convertByteArrayToType(result);
              synchronized(listener) {
                for(ObjectListener<T> l: listener) {
                  l.newObjectReceived(object);
                }
              }
            }
            catch(IllegalArgumentException ex)
            {
              notifyErrorOccured("Conversion of the received byte array failed.\n" +
                "Reason:\n" + ex.getLocalizedMessage());
            }
        }
    });
  }

  @Override
  public void handleError(final int code)
  {
    SwingUtilities.invokeLater(new Runnable() { 
        @Override 
        public void run() {
            notifyErrorOccured("Robot detected an error: error code " + code);
        }
    });
  }
  
  private void notifyErrorOccured(final String message)
  {
    synchronized(listener) {
      for(final ObjectListener<T> l : listener) {
        l.errorOccured(message);
      }
    }
  }
  
  /**
   * Convert a byte array to the type provided by this manager.
   * @throws java.lang.IllegalArgumentException Throw an exception if you wasn't able
   *                                            to convert this byte array. The
   *                                            error handlers will be called.
   */
  public abstract T convertByteArrayToType(byte[] result) throws IllegalArgumentException;

  /**
   * 
   * @return the message server
   */
  public abstract MessageServer getServer();

}//end class AbstractManager
