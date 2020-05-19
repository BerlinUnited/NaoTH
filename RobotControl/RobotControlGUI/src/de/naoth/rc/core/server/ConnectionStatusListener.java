/*
 */

package de.naoth.rc.server;

import java.util.EventListener;

/**
 *
 * @author Heinrich Mellmann
 */
public interface ConnectionStatusListener extends EventListener {
    abstract public void connected(ConnectionStatusEvent event);
    abstract public void disconnected(ConnectionStatusEvent event);
}
