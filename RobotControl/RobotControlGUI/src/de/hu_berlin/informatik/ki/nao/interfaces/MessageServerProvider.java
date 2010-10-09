/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.hu_berlin.informatik.ki.nao.interfaces;

import de.hu_berlin.informatik.ki.nao.server.MessageServer;
import net.xeoh.plugins.base.Plugin;

/**
 *
 * @author thomas
 */
public interface  MessageServerProvider extends Plugin
{
  public MessageServer getMessageServer();
}
