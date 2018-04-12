package de.naoth.rc.components.teamcomm;

import net.xeoh.plugins.base.Plugin;

/**
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public interface TeamCommUDPReceiver extends Plugin {
    public void connect(TeamCommListener listener, int port);
    public void disconnect(TeamCommListener listener, int port);
}
