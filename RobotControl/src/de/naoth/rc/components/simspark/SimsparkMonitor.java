package de.naoth.rc.components.simspark;

import de.naoth.rc.components.simspark.scene.SimsparkScene;
import de.naoth.rc.components.simspark.scene.SimsparkSceneAgent;
import de.naoth.rc.components.simspark.scene.SimsparkSceneBall;
import de.naoth.rc.components.simspark.scene.SimsparkSceneObject;
import de.naoth.rc.components.teamcomm.TeamCommManager;
import de.naoth.rc.components.teamcomm.TeamCommMessage;
import de.naoth.rc.dataformats.SPLMessage;
import de.naoth.rc.dataformats.Sexp;
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
 * Additional, the parsed state is provided to other modules via the
 * SimsparkManger and if simspark * sends teamcomm messages, these are parsed too
 * and broadcasted to other listening modules/dialog (eg. TeamCommViewer). Also
 * the simspark scene graph is parsed and published to the scene listeners.
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

    /**
     * Representation of the simspark game state.
     */
    public SimsparkState state = new SimsparkState();

    /**
     * Indicates, if the state was updated.
     */
    private boolean stateHasBeenUpdated = false;

    /**
     * Representation of the simspark scene.
     */
    public SimsparkScene scene = new SimsparkScene();

    /**
     * Indicates, if the scene was updated.
     */
    private boolean sceneHasBeenUpdated = false;

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
        public void run()
        {
            stateHasBeenUpdated = false;
            sceneHasBeenUpdated = false;

            parseMessages(parser.parseSexp());

            if (stateHasBeenUpdated)
            {
                Plugin.simsparkManger.receivedSimsparkState(state);
            }

            if (sceneHasBeenUpdated)
            {
                Plugin.simsparkManger.updateSimsparkScene(scene);
            }
        }

        public void parseMessages(List<Object> messages)
        {
            // skip empty messages
            if (messages.isEmpty())
            {
                return;
            }

            // the message must have 3 parts
            if (messages.size() != 3)
            {
                return;
            }

            // the first part contains the environment (update)
            ((List<Object>) messages.get(0)).stream().forEach((env) ->
            {
                List<Object> p = (List<Object>) env;
                updateSimsparkState((String) p.get(0), p.subList(1, p.size()));
            });

            // the second part contains the update type and consist of 3 parts
            List<Object> type = (List<Object>) messages.get(1);
            if (type.size() != 3)
            {
                return;
            }

            String name = (String) type.get(0);
            if (name.equals("RSG"))
            {
                // Ruby Scene Graph: indicates that the scene graph is a full description of the env.
                parseRubySceneGraph((List<Object>) messages.get(2));
            }
            else if (name.equals("RDS"))
            {
                // Ruby Diff Scene, and indicates that the scene graph is a partial description of the environment
                parseRubyDiffScene((List<Object>) messages.get(2));
            }
            else
            {
                Logger.getLogger(SimsparkMonitor.class.getName()).log(Level.WARNING, "Invalid scene graph update!");
            }
        } // end parseMessages()

        private void parseRubySceneGraph(List<Object> nodes)
        {
            scene.clear();
            for (int i = 0; i < nodes.size(); i++)
            {
                List<Object> nd = (List<Object>) nodes.get(i);

                // check if node is ball
                Object value = listAccessor(nd, 3, 3, 1);
                if (value instanceof String && ((String) value).contains("soccerball"))
                {
                    SimsparkSceneBall ball = new SimsparkSceneBall(
                            i,
                            Float.parseFloat((String) listAccessor(nd, 2, 13)),
                            Float.parseFloat((String) listAccessor(nd, 2, 14)),
                            Float.parseFloat((String) listAccessor(nd, 2, 15)));
                    scene.add(ball);
                    sceneHasBeenUpdated = true;

                    continue;
                }

                // check if node is an agent
                value = listAccessor(nd, 3, 3, 3, 3, 1);
                if (value instanceof String && ((String) value).contains("naobody"))
                {
                    List<Object> pos = (List<Object>) listAccessor(nd, 3, 2);

                    String team = ((String) listAccessor(nd, 3, 3, 3, 5, 2)).substring(3);
                    int number = Integer.parseInt(((String) listAccessor(nd, 3, 3, 3, 5, 1)).substring(6));

                    SimsparkSceneAgent robot = new SimsparkSceneAgent(
                            i,
                            team,
                            number,
                            Float.parseFloat((String) pos.get(13)),
                            Float.parseFloat((String) pos.get(14)),
                            Float.parseFloat((String) pos.get(15)),
                            (float) calculateRotation(Float.parseFloat((String) pos.get(5)), Float.parseFloat((String) pos.get(6))));
                    scene.add(robot);
                    sceneHasBeenUpdated = true;

                    continue;
                }
            }
        }

        private void parseRubyDiffScene(List<Object> nodes)
        {
            for (SimsparkSceneObject object : scene.data)
            {
                if (object instanceof SimsparkSceneBall)
                {
                    List<Object> ballNode = (List<Object>) listAccessor(nodes, object.sceneIndex, 1);
                    if (ballNode.size() >= 16)
                    {
                        ((SimsparkSceneBall) object).update(
                                Float.parseFloat((String) ballNode.get(13)),
                                Float.parseFloat((String) ballNode.get(14)),
                                Float.parseFloat((String) ballNode.get(15))
                        );
                        sceneHasBeenUpdated = true;
                    }
                }
                else if (object instanceof SimsparkSceneAgent)
                {
                    List<Object> agentNode = (List<Object>) listAccessor(nodes, object.sceneIndex, 1, 1);
                    if (agentNode.size() >= 16)
                    {
                        SimsparkSceneAgent robot = (SimsparkSceneAgent) object;
                        robot.update(
                                Float.parseFloat((String) agentNode.get(13)),
                                Float.parseFloat((String) agentNode.get(14)),
                                Float.parseFloat((String) agentNode.get(15)),
                                (float) calculateRotation(Float.parseFloat((String) agentNode.get(5)), Float.parseFloat((String) agentNode.get(6)))
                        );
                        sceneHasBeenUpdated = true;
                    }
                }
            }
        }

        private double calculateRotation(float r1, float r2)
        {
            return Math.acos(r1 / Math.sqrt(r1 * r1 + r2 * r2)) * (r2 < 0 ? -1 : 1);
        }

        private Object listAccessor(List<Object> l, int... i)
        {
            Object current = l;
            for (int idx : i)
            {
                if (current instanceof List && ((List<Object>) current).size() > idx)
                {
                    current = ((List<Object>) current).get(idx);
                }
                else
                {
                    return null;
                }
            }

            return current;
        }
        
        private void updateSimsparkState(String attribute, List<Object> value)
        {
            if (attribute.equals("messages"))
            {
                // if it's a list of message, broadcast them
                //broadcastTeamCommMessages(value);
            }
            else if (value.isEmpty())
            {
                // if there's no value, attribute is handled as boolean and set to true
                state.set(attribute, true);
            }
            else if (value.size() == 1)
            {
                // if there's only one value in the list, set simsparkstate value to this one
                state.set(attribute, value.get(0));
            }
            else
            {
                // otherwise it's a list of values ...
                state.set(attribute, value);
            }
            stateHasBeenUpdated = true;
        }

        private void broadcastTeamCommMessages(List<Object> messages)
        {
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
                try
                {
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
