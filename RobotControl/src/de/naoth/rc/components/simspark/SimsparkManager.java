package de.naoth.rc.components.simspark;

import de.naoth.rc.components.simspark.scene.SimsparkScene;
import javafx.beans.property.BooleanProperty;
import net.xeoh.plugins.base.Plugin;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public interface SimsparkManager extends Plugin
{
    /**
     * Connects to a SimSpark instance on the given host & port.
     * @param host the host, where the SimSpark instance is running
     * @param port the port, where the SimSpark instance has the monitor port
     */
    public void connect(String host, int port);
    
    /**
     * Disconnects from the SimSpark instance.
     */
    public void disconnect();
    
    /**
     * Returns a boolean property indicating the connection state to SimSpark.
     * @return the connection state
     */
    public BooleanProperty isConnected();
    
    /**
     * Returns the host of the connected SimSpark instance.
     * @return the SimSpark host
     */
    public String getHost();
    
    /**
     * Returns the port of the connected SimSpark instance.
     * @return the SimSpark monitor port
     */
    public int getPort();
    
    /**
     * Sends a command to the SimSpark instance.
     * @param command the command to send
     * @return true, if sending was successful, false otherwise
     */
    public boolean sendCommand(String command);
    
    /**
     * Removes a simspark listener from the active listener list.
     * 
     * @param l the listener, which should be removed
     */
    void addSimsparkStateListener(SimsparkStateListener l);
    
    /**
     * Adds a simspark listener to the active listner list.
     * The listener gets called, when a new simspark state is received.
     * 
     * @param l the listener, which should be added
     */
    void removeSimsparkStateListener(SimsparkStateListener l);
    
    /**
     * When a new simspark state was received, this method should be called to notify all active listener.
     * 
     * @param s the new simspark state
     */
    void receivedSimsparkState(SimsparkState s);

    /**
     * Removes a simspark listener from the active scene listener list.
     *
     * @param l the listener, which should be removed
     */
    void addSimsparkSceneListener(SimsparkSceneListener l);

    /**
     * Adds a simspark listener to the active scene listner list. The listener
     * gets called, when a new simspark scene is received.
     *
     * @param l the listener, which should be added
     */
    void removeSimsparkSceneListener(SimsparkSceneListener l);

    /**
     * When the simspark scene was updated, all active listeners are notified.
     *
     * @param s the updated simspark scene
     */
    void updateSimsparkScene(SimsparkScene s);
}
