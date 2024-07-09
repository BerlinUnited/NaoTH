package de.naoth.rc.components.simspark;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public interface SimsparkStateListener
{
    /**
     * Gets called, when a new simspark state was received.
     * 
     * @param state the new simspark state
     */
    void newSimsparkData(SimsparkState state);
}
