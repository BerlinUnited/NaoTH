package de.naoth.rc.components.simspark;

import de.naoth.rc.dataformats.SimsparkState;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public interface SimsparkListener
{
    /**
     * Gets called, when a new simspark state was received.
     * 
     * @param state the new simspark state
     */
    void newSimsparkData(SimsparkState state);
}
