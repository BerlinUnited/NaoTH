/*
 * 
 */

package de.naoth.rc.server;

import java.net.InetSocketAddress;
import java.util.EventObject;

/**
 *
 * @author Heinrich Mellmann
 */
public class ConnectionStatusEvent extends EventObject {
    private String message;
    private InetSocketAddress address;
    
    public ConnectionStatusEvent(Object source) {
        this(source, null, null);
    }
    
    public ConnectionStatusEvent(Object source, String message) {
        this(source, message, null);
    }
    
    public ConnectionStatusEvent(Object source, InetSocketAddress address) {
        this(source, null, address);
    }
    
    public ConnectionStatusEvent(Object source, String message, InetSocketAddress address) {
        super(source);
        this.message = message;
        this.address = address;
    }
    
    public String getMessage() {
        return this.message;
    }
    
    public InetSocketAddress getAddress() {
        return this.address;
    }
}
