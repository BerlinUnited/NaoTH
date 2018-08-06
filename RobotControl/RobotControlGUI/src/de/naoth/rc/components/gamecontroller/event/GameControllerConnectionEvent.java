package de.naoth.rc.components.gamecontroller.event;

import java.util.EventObject;

/**
 * Event signalling the connection state to the GameController.
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class GameControllerConnectionEvent extends EventObject
{
    public enum State {
        CONNECTED,
        DISCONNECTED,
        TIMEOUT
    }
    
    public final State state;
    
    public GameControllerConnectionEvent(Object source, State state) {
        super(source);
        this.state = state;
    }
}
