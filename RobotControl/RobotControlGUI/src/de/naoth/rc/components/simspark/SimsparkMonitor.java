package de.naoth.rc.components.simspark;

import de.naoth.rc.components.teamcomm.TeamCommManager;
import de.naoth.rc.components.teamcomm.TeamCommMessage;
import de.naoth.rc.dataformats.SPLMessage;
import de.naoth.rc.dataformats.Sexp;
import de.naoth.rc.dataformats.SimsparkState;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.Base64;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.logging.Level;
import java.util.logging.Logger;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 * A simple Simspark monitor.
 * Receives Simspark monitor messages and parses them.
 * Additional, the parsed state is provided to other modules via the SimsparkManger and if simspark 
 * sends teamcomm messages, these are parsed too and  broadcasted to other listening modules/dialog
 * (eg. TeamCommViewer).
 * 
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class SimsparkMonitor extends Simspark {
    
    @PluginImplementation
    public static class Plugin implements net.xeoh.plugins.base.Plugin {
        @InjectPlugin
        public static TeamCommManager teamcommManager;
        @InjectPlugin
        public static SimsparkManager simsparkManger;
    }//end Plugin

    /** Representation of the simspark game state. */
    public SimsparkState state = new SimsparkState();
    
    /**
     * Main method of the simspark monitor thread.
     */
    @Override
    public void run() {
        if (socket == null) {
            return;
        }
        ExecutorService s = Executors.newSingleThreadExecutor();
        
        while (isConnected.get()) {
            try {
                final String msg;
                msg = receiveMessage();

                if (msg != null) {
                    // parse message in another thread
                    s.submit(new SimsparkMonitorMessageParser(msg));
                }
            } catch (IOException ex) {
                // NOTE: is there a way to notiy the user?!
                checkConnection();
            }
        }
    }
    
    /**
     * Parser for the simspark monitor messages.
     */
    private class SimsparkMonitorMessageParser implements Runnable {
        private final Sexp parser = new Sexp(); 

        public SimsparkMonitorMessageParser(String msg) {
            this.parser.setExpression(msg);
        }

        @Override
        public void run() {
            state.hasBeenUpdated = false;
            parseMessages(parser.parseSexp());
            if(state.hasBeenUpdated) {
                Plugin.simsparkManger.receivedSimsparkState(state);
            }
        }

        public void parseMessages(List<Object> messages) {
            // skip empty messages
            if (!messages.isEmpty()) {
                    // get the first "object"
                    Object o = messages.get(0);
                    // first "object" could be a string ...
                    if (o instanceof String) {
                        updateSimsparkState((String) o, messages.subList(1, messages.size()));
                        // if there are TeamCommMessages - parse and "broadcast" them
                        if(((String) o).equals("messages")) {
                            broadcastTeamCommMessages(messages.subList(1, messages.size()));
                        }
                    } else {
                        // ... or object could be a list of objects/strings
                        for (Object object : messages) {
                            // iterate over list and parse the objects/strings
                            parseMessages((List<Object>) object);
                        }
                    }
                }
        } // end parseMessages()
        
        private void updateSimsparkState(String attribute, List<Object> value) {
            if(value.isEmpty()) {
                // if there's no value, attribute is handled as boolean and set to true
                state.set(attribute, true);
            } else if(value.size() == 1) {
                // if there's only one value in the list, set simsparkstate value to this one
                state.set(attribute, value.get(0));
            } else {
                // otherwise it's a list of values ...
                state.set(attribute, value);
            }
        }
        
        private void broadcastTeamCommMessages(List<Object> messages) {
            List<TeamCommMessage> c = new ArrayList<>();
            ByteBuffer readBuffer = ByteBuffer.allocateDirect(SPLMessage.size());
            readBuffer.order(ByteOrder.LITTLE_ENDIAN);
            // iterate over available messages
            for (Object object : messages) {
                List<Object> msg_list = (List<Object>) object;
                // retrieve additional message infos
                int side = 0; String ip = "127.0.0.1";
                for (Object info : msg_list.subList(1, msg_list.size())) {
                    switch(((List<String>)info).get(0)) {
                        // 0 - None, 1 - left, 2 - right
                        case "side": side = Integer.parseInt(((List<String>)info).get(1)); break;
                        case "ip": ip = ((List<String>)info).get(1); break;
                    }
                }
                String message = (String) msg_list.get(0);
                try {
                    byte[] b = Base64.getDecoder().decode(message);
                    readBuffer.clear();
                    readBuffer.put(b);
                    readBuffer.flip();
                    SPLMessage spl = SPLMessage.parseFrom(readBuffer);
                    c.add(new TeamCommMessage(
                        System.currentTimeMillis(),
                        // see SimSparkController.cpp, ~line: 280, "calculate debug communicaiton port"
                        String.format("%s:%d", ip, ((side==1?5400:5500)+spl.playerNum)),
                        spl,
                        ((int)spl.teamNum) != 4) // TOOD: can we set anywhere our team number?!?
                    );
                } catch (Exception ex) {
                    Logger.getLogger(SimsparkMonitor.class.getName()).log(Level.SEVERE, null, ex);
                }
            }
            Plugin.teamcommManager.receivedMessages(c);
        }
    } // end class SimsparkMonitorMessageParser
    
} // end class SimsparkMonitor
