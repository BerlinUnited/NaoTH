package de.naoth.rc.components.simspark;

import de.naoth.rc.dataformats.SimsparkState;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.beans.property.BooleanProperty;
import javafx.beans.property.SimpleBooleanProperty;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.events.Shutdown;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
@PluginImplementation
public class SimsparkManagerImpl implements SimsparkManager
{
    /** The registered simspark listener. */
    private final List<SimsparkListener> simsparkListeners = Collections.synchronizedList(new ArrayList<>());
    /** The connected SimSpark monitor instance. */
    private SimsparkMonitor simsparkMonitor;
    /** Boolean property indicating the connection state of the SimSpark monitor instance. */
    private final BooleanProperty isConnected = new SimpleBooleanProperty();

    /**
     * {@inheritDoc}
     */
    @Override
    public void connect(String host, int port)
    {
        try
        {
            if (simsparkMonitor != null)
            {
                simsparkMonitor.disconnect();
            }

            simsparkMonitor = new SimsparkMonitor();
            simsparkMonitor.isConnected.addListener((v) ->
            {
                isConnected.set(((BooleanProperty) v).get());
            });
            simsparkMonitor.connect(host.trim(), port);
        }
        catch (IOException | InterruptedException ex)
        {
            Logger.getLogger(SimsparkManager.class.getName()).log(Level.WARNING, "Unable to connect to SimSpark", ex);
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void disconnect()
    {
        try
        {
            if (simsparkMonitor != null)
            {
                simsparkMonitor.disconnect();
                isConnected.set(false);
            }
        }
        catch (IOException | InterruptedException ex)
        {
            Logger.getLogger(SimsparkManager.class.getName()).log(Level.SEVERE, null, ex);
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public BooleanProperty isConnected()
    {
        return isConnected;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public String getHost()
    {
        return simsparkMonitor != null ? simsparkMonitor.getHost() : null;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public int getPort()
    {
        return simsparkMonitor != null ? simsparkMonitor.getPort() : null;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean sendCommand(String command)
    {
        if (simsparkMonitor != null)
        {
            try
            {
                return simsparkMonitor.sendMessage(command);
            }
            catch (IOException ex)
            {
                Logger.getLogger(SimsparkManagerImpl.class.getName()).log(Level.SEVERE, null, ex);
            }
        }

        return false;
    }

    /**
     * Gracefully disconnect from SimSpark, when the application exits.
     */
    @Shutdown
    public void shutdown()
    {
        disconnect();
    }

    /**
     * Removes a simspark listener from the active listener list.
     *
     * @param l the listener, which should be removed
     */
    @Override
    public void removeSimsparkListener(SimsparkListener l)
    {
        simsparkListeners.remove(l);
    }

    /**
     * Adds a simspark listener to the active listner list. The listener gets
     * called, when a new simspark state is received.
     *
     * @param l the listener, which should be added
     */
    @Override
    public void addSimsparkListener(SimsparkListener l)
    {
        if (l != null)
        {
            simsparkListeners.add(l);
        }
    }

    /**
     * A new simspark state is received; all active listeners are called.
     *
     * @param s the new simspark state
     */
    @Override
    public void receivedSimsparkState(SimsparkState s)
    {
        simsparkListeners.forEach((l) ->
        {
            try
            {
                l.newSimsparkData(s);
            }
            catch (Exception e)
            {
                Logger.getLogger(SimsparkManagerImpl.class.getName()).log(Level.SEVERE, null, e);
            }
        });
    }
}
