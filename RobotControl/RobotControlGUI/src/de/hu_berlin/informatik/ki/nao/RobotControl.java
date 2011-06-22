/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.hu_berlin.informatik.ki.nao;

import de.hu_berlin.informatik.ki.nao.server.MessageServer;
import java.util.Properties;
import net.xeoh.plugins.base.Plugin;

/**
 *
 * @author thomas
 */
public interface RobotControl extends Plugin
{
  public void setVisible(boolean v);
  public boolean checkConnected();
  public MessageServer getMessageServer();
  public Properties getConfig();
}
