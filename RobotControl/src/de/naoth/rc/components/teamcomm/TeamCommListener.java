package de.naoth.rc.components.teamcomm;

import java.util.List;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public interface TeamCommListener {
    /**
     * Callback method for new TeamCommMessages.
     * @param messages list of received messages
     */
    public void newTeamCommMessages(List<TeamCommMessage> messages);
}
