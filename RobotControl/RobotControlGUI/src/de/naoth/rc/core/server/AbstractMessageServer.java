/*
 */

package de.naoth.rc.server;

import java.net.InetSocketAddress;
import javax.swing.event.EventListenerList;

/**
 *
 * @author Heinrich Mellmann
 */
public class AbstractMessageServer {
    
    protected EventListenerList listenerList = new EventListenerList();
    
    public void fireConnected(InetSocketAddress address) {
        for (ConnectionStatusListener listener:
         listenerList.getListeners(ConnectionStatusListener.class)) {
            listener.connected(new ConnectionStatusEvent(this, address));
        }
    }
    
    public void fireDisconnected(String message) {
        for (ConnectionStatusListener listener:
         listenerList.getListeners(ConnectionStatusListener.class)) {
            listener.disconnected(new ConnectionStatusEvent(this, message));
        }
    }
        
    public void addConnectionStatusListener(ConnectionStatusListener listener) {
        this.listenerList.add(ConnectionStatusListener.class, listener);
    }
    
    public void removeConnectionStatusListener(ConnectionStatusListener listener) {
        this.listenerList.remove(ConnectionStatusListener.class, listener);
    }
}
