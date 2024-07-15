package de.naoth.rc.components.simspark;

import de.naoth.rc.components.simspark.commands.AgentInitCommand;
import de.naoth.rc.components.simspark.commands.AgentSceneCommand;
import de.naoth.rc.components.simspark.commands.AgentSyncCommand;
import de.naoth.rc.components.simspark.commands.SimsparkCommand;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * Basic Simspark agent.
 * Just an experiment and to show how to create an agent and communicate with
 * Simspark.
 * 
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class SimsparkAgent extends Simspark
{
    private SimsparkCommand scene = new AgentSceneCommand();
    private SimsparkCommand init = new AgentInitCommand();
    private SimsparkCommand sync = new AgentSyncCommand();
    /**
     * Main method of the simspark agent thread.
     */
    @Override
    public void run() {
        if (socket == null) {
            return;
        }
        try {
            // init simspark communication
            sendMessage(scene);
            System.out.println(receiveMessage());
            sendMessage(init);
            System.out.println(receiveMessage());
            
            System.out.println("listening");
            
            while (isConnected.get()) {
                try {
                    // Simspark in "sync" mode ...
                    sendMessage(sync);
                    String msg = receiveMessage();
                    if (msg != null) {
                        // TODO: where should the received agent messages go?!
                        System.out.println(msg);
                    }
                } catch (IOException ex) {
                    // check connection and disconnect if connection lost!
                    checkConnection();
                }
            }
        } catch (IOException ex) {
            Logger.getLogger(SimsparkAgent.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
} // end class SimsparkAgent
