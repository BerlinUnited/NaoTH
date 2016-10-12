package de.naoth.rc.components.simspark;

/**
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public interface SimsparkMonitorListener {
    /**
     * Callback method for new TeamCommMessages.
     * @param messages list of received messages
     */
    public void newTeamCommMessages();
}
