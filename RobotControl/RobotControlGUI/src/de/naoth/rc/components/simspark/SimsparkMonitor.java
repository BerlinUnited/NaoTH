
package de.naoth.rc.components.simspark;

import de.naoth.rc.components.teamcomm.TeamCommManager;
import de.naoth.rc.components.teamcomm.TeamCommMessage;
import de.naoth.rc.dataformats.SPLMessage;
import de.naoth.rc.dataformats.Sexp;
import de.naoth.rc.dataformats.SimsparkState;
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
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class SimsparkMonitor extends Simspark {
    
    @PluginImplementation
    public static class Plugin implements net.xeoh.plugins.base.Plugin {
        @InjectPlugin
        public static TeamCommManager teamcommManager;
    }//end Plugin

    public SimsparkState state = new SimsparkState();
    
    public void run() {
        if (socket == null) {
            return;
        }
        ExecutorService s = Executors.newSingleThreadExecutor();
        
        while (isRunning) {
            try {
                sleep(1);
                final String msg = getServerMessage();
                if (msg != null) {
                    s.submit(new SimsparkMonitorMessageParser(msg));
                }
            } catch (InterruptedException ex) {
                Logger.getLogger(SimsparkMonitor.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
    }
    
    
    private class SimsparkMonitorMessageParser implements Runnable {
        private final Sexp parser = new Sexp(); 

        public SimsparkMonitorMessageParser(String msg) {
            this.parser.setExpression(msg);
        }

        public void run() {
            parseMessages(parser.parseSexp());
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
            ByteBuffer readBuffer = ByteBuffer.allocateDirect(SPLMessage.SPL_STANDARD_MESSAGE_SIZE);
            readBuffer.order(ByteOrder.LITTLE_ENDIAN);
            for (Object object : messages) {
                String message = (String) object;
                try {
                    byte[] b = Base64.getDecoder().decode(message);
                    readBuffer.clear();
                    readBuffer.put(b);
                    readBuffer.flip();
                    SPLMessage spl = new SPLMessage(readBuffer);
                    c.add(new TeamCommMessage(
                        System.currentTimeMillis(),
                        spl.teamNum + "." + spl.playerNum,
                        spl,
                        spl.teamNum == 4)
                    );
                } catch (Exception ex) {
                    Logger.getLogger(SimsparkMonitor.class.getName()).log(Level.SEVERE, null, ex);
                }
            }
            Plugin.teamcommManager.receivedMessages(c);
        }
    } // end class SimsparkMonitorMessageParser
    
} // end class SimsparkMonitor
