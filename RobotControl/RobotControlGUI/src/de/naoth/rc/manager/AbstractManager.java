/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.naoth.rc.manager;

import de.naoth.rc.server.Command;
import de.naoth.rc.server.MessageServer;
import java.util.LinkedList;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

/**
 * This is the basic class for every manager. It does a lot of work in order
 * to avoid deadlocks. It should be much easier to extend you manager from 
 * this class instead of implementing you own <code>CommandSender</code>. <br><br>
 * 
 * The type <code>T</code> is the type of the (data) object that is managed.
 * 
 * @author thomas
 */
public abstract class AbstractManager<T> implements Manager<T>
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
   * +--------+ &lt;-------+    |   
   * |ManagerX| -------------+   
   * +--------+
   * 
   * </pre>
   */
  private final Lock LISTENER_LOCK = new ReentrantLock();
  
  private LinkedList<ObjectListener<T>> listener = 
    new LinkedList<ObjectListener<T>>();

  public AbstractManager()
  {
  }
  
  @Override
  public void addListener(ObjectListener<T> l)
  {
    LISTENER_LOCK.lock();
    if(!listener.contains(l))
    {
      listener.add(l);
    }
    int size = listener.size();
    LISTENER_LOCK.unlock();
    if(size == 1)
    {
      getServer().addCommandSender(this);
    }
  }

  @Override
  public void removeListener(ObjectListener<T> l)
  {
    LISTENER_LOCK.lock();
    listener.remove(l);
    int size = listener.size();
    LISTENER_LOCK.unlock();
    if(size == 0)
    {
      getServer().removeCommandSender(this);
    }    
  }

  @Override
  public void handleResponse(byte[] result, Command originalCommand)
  {    
    // copy listeners
    LISTENER_LOCK.lock();
    LinkedList<ObjectListener<T>> copyListener = new LinkedList<ObjectListener<T>>();
    copyListener.addAll(listener);
    LISTENER_LOCK.unlock();
    
    try
    {
      T object = convertByteArrayToType(result);
      
      for(ObjectListener<T> l : copyListener)
      {
        l.newObjectReceived(object);
      }
    }
    catch(IllegalArgumentException ex)
    {
      privateErrorHandler("Conversion of the received byte array failed.\n" +
        "Reason:\n" + ex.getLocalizedMessage(), copyListener);
    }
    
  }

  @Override
  public void handleError(int code)
  {
    // copy listeners
    LISTENER_LOCK.lock();
    LinkedList<ObjectListener<T>> copyListener = new LinkedList<ObjectListener<T>>();
    copyListener.addAll(listener);
    LISTENER_LOCK.unlock();
    
    privateErrorHandler("Robot detected an error: error code " + code, copyListener);
  }
  
  private void privateErrorHandler(String message, LinkedList<ObjectListener<T>> list)
  {
    for(ObjectListener<T> l : list)
    {
      l.errorOccured(message);
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
