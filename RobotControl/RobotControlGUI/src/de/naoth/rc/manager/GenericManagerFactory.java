/*
 * 
 */

package de.naoth.rc.manager;

import de.naoth.rc.server.Command;
import net.xeoh.plugins.base.Plugin;

/**
 *
 * @author Heinrich Mellmann
 */
public interface GenericManagerFactory extends Plugin
{
  abstract GenericManager getManager(Command command);
}
