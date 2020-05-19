/*
 *
 */

package de.naoth.rc.core.manager;

import de.naoth.rc.core.server.Command;
import net.xeoh.plugins.base.Plugin;

/**
 *
 * @author Heinrich Mellmann
 */
public interface SwingCommandExecutor extends Plugin{
    abstract public void executeCommand(ObjectListener<byte[]> listener, Command command);
}
