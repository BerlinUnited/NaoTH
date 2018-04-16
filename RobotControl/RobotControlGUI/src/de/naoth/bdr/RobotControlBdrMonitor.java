package de.naoth.bdr;

import de.naoth.rc.server.MessageServer;
import java.util.Properties;
import net.xeoh.plugins.base.Plugin;

/**
 *
 * @author thomas
 */
public interface RobotControlBdrMonitor extends Plugin
{
  public void setVisible(boolean v);
  public boolean checkConnected();
  public MessageServer getMessageServer();
  public Properties getConfig();
}
