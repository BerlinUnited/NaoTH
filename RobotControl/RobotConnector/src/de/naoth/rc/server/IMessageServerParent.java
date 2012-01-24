/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.naoth.rc.server;

import java.util.Properties;

/**
 *
 * @author thomas
 */
public interface IMessageServerParent
{
  MessageServer getMessageServer();
  void showConnected(boolean isConnected);
  Properties getConfig();
}
