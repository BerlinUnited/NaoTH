package de.naoth.rc.components.gamecontroller;

import de.naoth.rc.components.gamecontroller.event.GameControllerConnectionEvent;
import de.naoth.rc.components.gamecontroller.event.GameControllerDataEvent;
import java.util.EventListener;

/**
 * Interface for GameController listeners.
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public interface GameControllerEventListener extends EventListener
{
    /**
     * Method gets called, when the connection state changed (CONNECTED, DISCONNECTED, TIMEOUT).
     * 
     * @param e the event signalling the connection state
     */
    void connectionChanged(GameControllerConnectionEvent e);
    
    /**
     * Method gets called, when new data from the GameController was received.
     * 
     * @param e event holding the GameController data
     */
    void newGameData(GameControllerDataEvent e);

}
