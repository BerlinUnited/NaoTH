/*
 * 
 */

package de.naoth.rc.manager;

/**
 *
 * @author thomas
 * @param <T> type of the message for this manager to handle
 */
public interface Manager<T>
{
  abstract public void addListener(ObjectListener<T> l);
  abstract public void removeListener(ObjectListener<T> l);
}
