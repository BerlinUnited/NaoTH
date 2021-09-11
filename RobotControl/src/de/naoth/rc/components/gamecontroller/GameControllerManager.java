package de.naoth.rc.components.gamecontroller;

import java.util.EventObject;
import net.xeoh.plugins.base.Plugin;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public interface GameControllerManager extends Plugin
{
    /**
     * Adds the listener to the listener list.
     * 
     * @param l the listener
     */
    void addGameListener(GameControllerEventListener l);
    
    /**
     * Removes the listener from the listener list.
     * 
     * @param l the listener
     */
    void removeGameListener(GameControllerEventListener l);
    
    /**
     * Notify all registred listeners of the new event.
     * 
     * @param e the new event
     */
    void fireEvent(EventObject e);
    
    /**
     * Connects to the GameController and returns if it was successfull or not.
     * 
     * @return true, if successfull connected, false otherwise
     */
    boolean connect();
    
    /**
     * Disconnects from the GameController.
     */
    void disconnect();
    
    /**
     * Determines, if all required conditions are fulfilled.
     * 
     * @return true, if everything is correct, false otherwise
     */
    boolean isReady();
    
    /**
     * Returns if already connected to a Gamecontroller.
     * 
     * @return true|false
     */
    boolean isConnected();
}
