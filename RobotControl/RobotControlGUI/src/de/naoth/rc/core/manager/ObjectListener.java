/*
 * 
 */

package de.naoth.rc.core.manager;

/**
 * A generic listener for all kinds of debug objects.
 * @author thomas
 * @param <T> type of the object to listen for
 */
public interface ObjectListener<T>
{
  public void newObjectReceived(T object);
  public void errorOccured(String cause);
}
