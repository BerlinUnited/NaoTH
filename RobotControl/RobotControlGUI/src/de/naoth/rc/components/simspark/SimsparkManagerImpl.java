package de.naoth.rc.components.simspark;

import de.naoth.rc.dataformats.SimsparkState;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;
import net.xeoh.plugins.base.annotations.PluginImplementation;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
@PluginImplementation
public class SimsparkManagerImpl implements SimsparkManager
{
    /** The registered simspark listener. */
    private final List<SimsparkListener> simsparkListerners = Collections.synchronizedList(new ArrayList<>());

    /**
     * Removes a simspark listener from the active listener list.
     * 
     * @param l the listener, which should be removed
     */
    @Override
    public void removeSimsparkListener(SimsparkListener l) {
        simsparkListerners.remove(l);
    }

    /**
     * Adds a simspark listener to the active listner list.
     * The listener gets called, when a new simspark state is received.
     * 
     * @param l the listener, which should be added
     */
    @Override
    public void addSimsparkListener(SimsparkListener l) {
        simsparkListerners.add(l);
    }

    /**
     * A new simspark state is received; all active listeners are called.
     * 
     * @param s the new simspark state
     */
    @Override
    public void receivedSimsparkState(SimsparkState s) {
        simsparkListerners.forEach((l) -> {
            try {
                l.newSimsparkData(s);
            } catch (Exception e) {
                Logger.getLogger(SimsparkManagerImpl.class.getName()).log(Level.SEVERE, null, e);
            }
        });
    }
}
