package de.naoth.rc.components.teamcomm;

import java.util.Collections;
import java.util.List;
import java.util.ArrayList;
import net.xeoh.plugins.base.annotations.PluginImplementation;

/**
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
@PluginImplementation
public class TeamCommManagerImpl implements TeamCommManager
{
    /**
     * A list containing all registered listeners.
     */
    private final List<TeamCommListener> listeners = Collections.synchronizedList(new ArrayList<TeamCommListener>());

    public TeamCommManagerImpl() {
    }
    
    @Override
    public void addListener(TeamCommListener l) {
        listeners.add(l);
    }
    
    @Override
    public void removeListener(TeamCommListener l) {
        listeners.remove(l);
    }

    @Override
    public void receivedMessages(List<TeamCommMessage> messages) {
        if(!messages.isEmpty()) {
            listeners.stream().forEach((listener) -> {
                listener.newTeamCommMessages(Collections.unmodifiableList(messages));
            });
        }
    }
}
