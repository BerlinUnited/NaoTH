package de.naoth.rc.components.teamcomm;

import de.naoth.rc.components.teamcommviewer.RobotTeamCommListener;
import java.io.IOException;
import java.util.Collections;
import java.util.List;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.logging.Level;
import java.util.logging.Logger;
import net.xeoh.plugins.base.annotations.PluginImplementation;

/**
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
@PluginImplementation
public class TeamCommUDPReceiverImpl implements TeamCommUDPReceiver
{
    /**
     * A list containing all registered listeners.
     */
    
    private final HashMap<Integer, Receiver> receivers = new HashMap<Integer, Receiver>();
    
    
    @Override
    public void connect(TeamCommListener listener, int port) {
        Receiver r = receivers.get(port);
        
        if (r == null) {
            r = new Receiver(port);
            receivers.put(port, r);
        }
        r.add(listener);
    }

    @Override
    public void disconnect(TeamCommListener listener, int port) {
        Receiver r = receivers.get(port);
        if (r != null) {
            r.remove(listener);
        }
    }
    
    class Receiver {
        private final List<TeamCommListener> listeners = Collections.synchronizedList(new ArrayList<TeamCommListener>());
        private final RobotTeamCommListener teamcommListener = new RobotTeamCommListener(false);
        
        public final int port;
        public Receiver(int port) {
            this.port = port;
        }
        
        public void add(TeamCommListener listener) {
            try {
                if(!listeners.contains(listener)) {
                    listeners.add(listener);
                }
                if(!teamcommListener.isConnected()) {
                    teamcommListener.connect(port);
                }
            } catch (IOException | InterruptedException ex) {
                Logger.getLogger(TeamCommUDPReceiverImpl.class.getName()).log(Level.SEVERE, null, ex);
            }
        }

        public void remove(TeamCommListener listener) {
            try {
                listeners.remove(listener);
                if (listeners.isEmpty()) {
                    teamcommListener.disconnect();
                }
            } catch (IOException | InterruptedException ex) {
                Logger.getLogger(TeamCommUDPReceiverImpl.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
    }
}
