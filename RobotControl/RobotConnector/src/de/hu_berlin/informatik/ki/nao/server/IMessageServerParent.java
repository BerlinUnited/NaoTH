/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.hu_berlin.informatik.ki.nao.server;

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
