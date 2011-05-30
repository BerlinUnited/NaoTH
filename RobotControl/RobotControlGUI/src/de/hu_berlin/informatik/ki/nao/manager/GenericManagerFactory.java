/*
 * 
 */

package de.hu_berlin.informatik.ki.nao.manager;

import de.hu_berlin.informatik.ki.nao.server.Command;
import net.xeoh.plugins.base.Plugin;

/**
 *
 * @author Heinrich Mellmann
 */
public interface GenericManagerFactory extends Plugin
{
  abstract GenericManager getManager(Command command);
}//end GenericManagerFactory
