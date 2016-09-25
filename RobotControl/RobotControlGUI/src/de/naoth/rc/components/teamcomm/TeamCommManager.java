package de.naoth.rc.components.teamcomm;

import java.util.List;
import net.xeoh.plugins.base.Plugin;

/**
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public interface TeamCommManager extends Plugin {
    /**
     * Sets the provider, where the TeamCommMessages came from.
     * @param provider  the TeamCommMessage-provider
     */
    public void setProvider(TeamCommProvider provider);
    
    /**
     * Removes the TeamCommMessage provider.
     */
    public void removeProvider();
    
    /**
     * Adds a listener to the TeamCommMessages.
     * The listener gets "notified", if a new TeamCommMessage arrives.
     * @param l 
     */
    public void addListener(TeamCommListener l);
    
    /**
     * Removes the listner.
     * @param l 
     */
    public void removeListener(TeamCommListener l);
    
    /**
     * Notifies the proxy, that new TeamCommMessages are available.
     * @param messages newly available TeamCommMessages
     */
    public void receivedMessages(List<TeamCommMessage> messages);
}
