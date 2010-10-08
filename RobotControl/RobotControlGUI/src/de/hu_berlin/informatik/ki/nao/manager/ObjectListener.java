/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.hu_berlin.informatik.ki.nao.manager;

/**
 * A generic listener for all kinds of debug objects.
 * @author thomas
 */
public interface ObjectListener<T>
{
  public void newObjectReceived(T object);
  
  public void errorOccured(String cause);
}
