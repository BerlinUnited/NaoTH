package de.naoth.rc.components.gamecontroller;

import de.naoth.rc.RobotControl;
import de.naoth.rc.components.gamecontroller.event.GameControllerConnectionEvent;
import de.naoth.rc.components.gamecontroller.event.GameControllerDataEvent;
import java.io.File;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetSocketAddress;
import java.net.SocketException;
import java.net.SocketTimeoutException;
import java.net.URL;
import java.net.URLClassLoader;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Collections;
import java.util.EventObject;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
@PluginImplementation
public class GameControllerManagerImpl implements GameControllerManager
{
    /** The (parent) RC plugin. */
    @InjectPlugin
    public static RobotControl rc;
    
    /** The timeout in ms, when there's nothing received from the GameController. */
    private static final int GAMECONTROLLER_TIMEOUT = 4000;
    
    /** The port of the GameController. */
    private static final int GAMECONTROLLER_GAMEDATA_PORT = 3838;
    
    /** The class loader of the GameController (external jar file). */
    private ClassLoader gamecontrol;
    
    /** The GameControlData class of GameController. */
    private Class gamecontroldata;
    
    /** The size of a GameControlData object. */
    private int gamecontroldatasize;
    
    /** List of registered GameControllerEvent listeners. */
    private final List<GameControllerEventListener> listeners = Collections.synchronizedList(new ArrayList<>());
    
    /** Listener thread to the GameController. */
    private GameControllerReceiver receiver;

    /**
     * Adds the listener to the listener list.
     * 
     * @param l the listener
     */
    @Override
    public void addGameListener(GameControllerEventListener l) {
        listeners.add(l);
    }

    /**
     * Removes the listener from the listener list.
     * 
     * @param l the listener
     */
    @Override
    public void removeGameListener(GameControllerEventListener l) {
        listeners.remove(l);
    }

    /**
     * Notify all registred listeners of the new event.
     * 
     * @param e the new event
     */
    @Override
    public void fireEvent(EventObject e) {
        if(e instanceof GameControllerDataEvent) {
            // got new game data, notify all listeners
            listeners.forEach((l) -> {
                l.newGameData((GameControllerDataEvent) e);
            });
        } else if(e instanceof GameControllerConnectionEvent) {
            // got timeout event, notify all listeners
            listeners.forEach((l) -> {
                l.connectionChanged((GameControllerConnectionEvent) e);
            });
        }
    }
    
    /**
     * Determines, if all required conditions are fulfilled.
     * In particular, if the GameController jar file is set and if the GameControlData class can be found
     * and loaded.
     * 
     * @return true, if everything is correct, false otherwise
     */
    @Override
    public boolean isReady() {
        return loadGameController();
    }
    
    /**
     * Tries to load the GameController from the jar file defined in the configuration.
     * Moreover it tries to load the GameControlData class and determine its object size.
     * 
     * @return true, if loading was successfull, false otherwise
     */
    private boolean loadGameController() {
        if(gamecontrol == null && rc.getConfig().containsKey("GameController")) {
            File gcFile = new File(rc.getConfig().getProperty("GameController"));
            if(gcFile.isFile()) {
                try {
                    // subclassed classloader
                    gamecontrol = new URLClassLoader (new URL[] {gcFile.toURI().toURL()}, this.getClass().getClassLoader());
                    gamecontroldata = Class.forName("data.GameControlData", true, gamecontrol);
                    gamecontroldatasize = (int) gamecontroldata.getField("SIZE").get(null);
                } catch (Exception ex) {
                    Logger.getLogger(GameControllerManagerImpl.class.getName()).log(Level.SEVERE, null, ex);
                    gamecontrol = null;
                }
            }
        }
        return gamecontrol != null;
    }
    
    /**
     * Connects to a GameController, if requirements are fulfilled and not already connected.
     * A receiver thread for the connection is created.
     * 
     * @return true, if connected, false otherwise.
     */
    @Override
    public boolean connect() {
        if(loadGameController()) {
            if(isConnected()) {
                return true;
            } else {
                try {
                    receiver = new GameControllerReceiver();
                    receiver.start();
                    return true;
                } catch (SocketException ex) {
                    Logger.getLogger(GameControllerManagerImpl.class.getName()).log(Level.SEVERE, null, ex);
                }
            }
        }
        return false;
    }
    
    /**
     * Disconnects from the GameController.
     */
    @Override
    public void disconnect() {
        if(receiver != null && receiver.isAlive()) {
            receiver.interrupt();
            try {
                receiver.join();
            } catch (InterruptedException ex) {
                Logger.getLogger(GameControllerManagerImpl.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
    }

    /**
     * Returns if already connected to a Gamecontroller.
     * 
     * @return true|false
     */
    @Override
    public boolean isConnected() {
        return receiver != null && receiver.isAlive() && !receiver.isInterrupted();
    }
    
    /**
     * Parses the buffer and returning a GameControllerData object if successfull. Otherwise null is returned.
     * 
     * @param buffer the data, which should be parsed
     * @return GameControllerData|null
     */
    private GameControllerData createGameControlData(ByteBuffer buffer) {
        try {
            // create new GameControlData object
            final Object data = gamecontroldata.newInstance();
            // call GameControlData.fromByteArray()
            if((boolean) gamecontroldata.getMethod("fromByteArray", ByteBuffer.class).invoke(data, buffer)) {
                GameControllerData parsedData = new GameControllerData();
                parsedData.playersPerTeam = gamecontroldata.getField("playersPerTeam").getByte(data);
                parsedData.competitionType = gamecontroldata.getField("competitionType").getByte(data);
                parsedData.gamePhase = gamecontroldata.getField("gamePhase").getByte(data);
                parsedData.gameState = gamecontroldata.getField("gameState").getByte(data);
                parsedData.setPlay = gamecontroldata.getField("setPlay").getByte(data);
                parsedData.firstHalf = gamecontroldata.getField("firstHalf").getByte(data);
                parsedData.kickingTeam = gamecontroldata.getField("kickingTeam").getByte(data);
                parsedData.secsRemaining = gamecontroldata.getField("secsRemaining").getShort(data);
                parsedData.secondaryTime = gamecontroldata.getField("secondaryTime").getShort(data);
                return parsedData;
            }
        } catch (Exception ex) {
            Logger.getLogger(GameControllerManagerImpl.class.getName()).log(Level.SEVERE, null, ex);
        }
        return null;
    } // END createGameControlData()
    
    /**
     * A receiver thread which handles the connection & incoming data.
     */
    class GameControllerReceiver extends Thread
    {
        /** The udp socket to the GameController. */
        private final DatagramSocket datagramSocket;
        
        /**
         * Constructor.
         * Establishes the connection to the GameController via a udp socket.
         * If successfull, the 'CONNECTED' event is fired.
         * 
         * @throws SocketException 
         */
        public GameControllerReceiver() throws SocketException {
            datagramSocket = new DatagramSocket(null);
            datagramSocket.setReuseAddress(true);
            datagramSocket.setSoTimeout(GAMECONTROLLER_TIMEOUT);
            datagramSocket.bind(new InetSocketAddress(GAMECONTROLLER_GAMEDATA_PORT));
            
            fireEvent(new GameControllerConnectionEvent(GameControllerManagerImpl.this, GameControllerConnectionEvent.State.CONNECTED));
        }
        
        /**
         * Receiving data from the GameController.
         */
        @Override
        public void run() {
            // thread is stopped via interrupt.
            while (!isInterrupted()) {
                try {
                    final ByteBuffer buffer = ByteBuffer.wrap(new byte[gamecontroldatasize]);
                    final DatagramPacket packet = new DatagramPacket(buffer.array(), buffer.array().length);
                    datagramSocket.receive(packet);

                    buffer.rewind();
                    GameControllerData data = createGameControlData(buffer);
                    // fire only if we got valid data
                    if(data != null) {
                        fireEvent(new GameControllerDataEvent(this, data));
                    }
                    
                } catch (SocketTimeoutException e) {
                    fireEvent(new GameControllerConnectionEvent(this, GameControllerConnectionEvent.State.TIMEOUT));
                } catch (IOException ex) {
                    Logger.getLogger(GameControllerManagerImpl.class.getName()).log(Level.SEVERE, null, ex);
                }
            }

            datagramSocket.close();
            fireEvent(new GameControllerConnectionEvent(GameControllerManagerImpl.this, GameControllerConnectionEvent.State.DISCONNECTED));
        } // END run()
    } // END GameControllerReceiver
}
