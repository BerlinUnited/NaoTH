/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.naoth.rc.manager;

import de.naoth.rc.server.CommandSender;
import net.xeoh.plugins.base.Plugin;

/**
 *
 * @author thomas
 */
public interface Manager<T> extends Plugin, CommandSender
{
  public void addListener(ObjectListener<T> l);
  public void removeListener(ObjectListener<T> l);

}
