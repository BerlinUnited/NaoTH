package de.naoth.rc.components.simspark;

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
            sendMessage("(scene rsg/agent/nao/nao.rsg 0)(syn)");
            System.out.println(receiveMessage());
            sendMessage("(init (unum 0)(teamname NaoTH))(syn)");
            System.out.println(receiveMessage());
            
            System.out.println("listening");
            
            while (isConnected.get()) {
                try {
                    // Simspark in "sync" mode ...
                    sendMessage("(syn)");
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
