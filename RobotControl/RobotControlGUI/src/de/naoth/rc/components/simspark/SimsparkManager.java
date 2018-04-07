package de.naoth.rc.components.simspark;

import de.naoth.rc.dataformats.SimsparkState;
import net.xeoh.plugins.base.Plugin;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public interface SimsparkManager extends Plugin
{
    /**
     * Removes a simspark listener from the active listener list.
     * 
     * @param l the listener, which should be removed
     */
    void addSimsparkListener(SimsparkListener l);
    
    /**
     * Adds a simspark listener to the active listner list.
     * The listener gets called, when a new simspark state is received.
     * 
     * @param l the listener, which should be added
     */
    void removeSimsparkListener(SimsparkListener l);
    
    /**
     * When a new simspark state was received, this method should be called to notify all active listener.
     * 
     * @param s the new simspark state
     */
    void receivedSimsparkState(SimsparkState s);
}
