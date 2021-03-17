/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.naoth.rc;

import de.naoth.rc.core.server.MessageServer;
import java.awt.Component;
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
  public String getConfigPath();
  
  public int getFontSize();
  public String getTheme();
  public void addToStatusBar(Component c);
}
