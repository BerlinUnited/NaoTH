package de.naoth.rc.components.teamcommviewer;

import de.naoth.rc.components.teamcomm.TeamCommManager;
import de.naoth.rc.components.teamcomm.TeamCommMessage;
import de.naoth.rc.core.messages.TeamMessageOuterClass;
import de.naoth.rc.dataformats.SPLMessage;
import de.naoth.rc.dialogs.TeamCommViewer;
import java.io.IOException;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.SocketAddress;
import java.net.SocketException;
import java.net.StandardSocketOptions;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.channels.AsynchronousCloseException;
import java.nio.channels.DatagramChannel;
import java.util.Collections;
import java.util.logging.Level;
import java.util.logging.Logger;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class RobotTeamCommListener implements Runnable {
        
        @PluginImplementation
        public static class Plugin implements net.xeoh.plugins.base.Plugin {
            @InjectPlugin
            public static TeamCommManager teamcommManager;
        }//end Plugin
        
        private DatagramChannel channel;
        
        private Thread trigger;

    private final ByteBuffer readBuffer;
    private final static ByteBuffer splHeader = ByteBuffer.wrap("SPL ".getBytes());
    private final static ByteBuffer dbgHeader = ByteBuffer.wrap("DBG ".getBytes());

        private final boolean isOpponent;

        public RobotTeamCommListener(boolean isOpponent) {
            this.readBuffer = ByteBuffer.allocateDirect(SPLMessage.size());
            this.readBuffer.order(ByteOrder.LITTLE_ENDIAN);
            this.isOpponent = isOpponent;
        }

        public boolean isConnected() {
            return this.channel != null && this.trigger != null;
        }

        public void connect(int port) throws IOException, InterruptedException {
            disconnect();

            this.channel = DatagramChannel.open();
            this.channel.setOption(StandardSocketOptions.SO_REUSEADDR, true);
            this.channel.configureBlocking(true);
            this.channel.bind(new InetSocketAddress(InetAddress.getByName("0.0.0.0"), port));

            this.trigger = new Thread(this);
            this.trigger.start();
        }

        public void disconnect() throws IOException, InterruptedException {
            if (this.channel != null) {
                this.channel.close();
                this.channel = null;
            }
            if (this.trigger != null) {
                this.trigger.join();
                this.trigger = null;
            }
        }

        @Override
        public void run() {
            try {
                while (true) {
                    this.readBuffer.clear();
                    SocketAddress address = this.channel.receive(this.readBuffer);
                    this.readBuffer.flip();

                    try {
                        long timestamp = System.currentTimeMillis();

                        ByteBuffer headBuffer = readBuffer.slice().limit(4);
                        if (headBuffer.equals(dbgHeader))
                        {
                            // we must copy the data from the buffer, otherwise errors are thrown!
                            byte[] data = new byte[readBuffer.limit() - 4];
                            readBuffer.position(4);
                            readBuffer.get(data);

                            // TOOD: use the actual debug message!
                            // create a dummy teammessage, so we have something to show
                            TeamMessageOuterClass.TeamMessageDebug user = TeamMessageOuterClass.TeamMessageDebug.parseFrom(data);
                            SPLMessage spl_msg = new SPLMessage();
                            spl_msg.playerNum = (byte) user.getPlayerNumber();
                            spl_msg.teamNum = (byte) user.getTeamNumber();
                            spl_msg.fallen = (byte) (user.getFallen() ? 1 : 0);
                            spl_msg.pose_x = (float) user.getPose().getTranslation().getX();
                            spl_msg.pose_y = (float) user.getPose().getTranslation().getY();
                            spl_msg.pose_a = (float) user.getPose().getRotation();
                            spl_msg.ballAge = user.getBallAge();
                            spl_msg.ball_x = (float) user.getBallPosition().getX();
                            spl_msg.ball_y = (float) user.getBallPosition().getY();
                            spl_msg.ballVel_x = (float) user.getBallVelocity().getX();
                            spl_msg.ballVel_y = (float) user.getBallVelocity().getY();

                            TeamMessageOuterClass.BUUserTeamMessage.Builder b = TeamMessageOuterClass.BUUserTeamMessage.newBuilder()
                                    .setBodyID(user.getBodyID())
                                    .setTimeToBall(user.getTimeToBall())
                                    //.setWasStriker()
                                    //.setIsPenalized()
                                    //.addOpponents()
                                    .setTeamNumber(user.getTeamNumber())
                                    .setBatteryCharge(user.getBatteryCharge())
                                    .setTemperature(user.getTemperature())
                                    .setTimestamp(user.getTimestamp())
                                    //.setWantsToBeStriker()
                                    .setCpuTemperature(user.getCpuTemperature())
                                    .setWhistleDetected(user.getWhistleDetected())
                                    .setWhistleCount(user.getWhistleCount())
                                    .setTeamBall(user.getTeamBall())
                                    //.addNtpRequest()
                                    .setBallVelocity(user.getBallVelocity())
                                    .setRobotState(user.getRobotState())
                                    .setRobotRole(user.getRobotRole())
                                    .setReadyToWalk(user.getReadyToWalk())
                                    .setKey("naoth");
                            spl_msg.user = b.build();
                            TeamCommMessage tc_msg = new TeamCommMessage(timestamp, ((InetSocketAddress) address).getHostString(), spl_msg, this.isOpponent);

                            if (address instanceof InetSocketAddress && Plugin.teamcommManager != null)
                            {
                                Plugin.teamcommManager.receivedMessages(Collections.singletonList(tc_msg));
                            }
                        }
                        else if (headBuffer.equals(splHeader))
                        {
                            SPLMessage spl_msg = SPLMessage.parseFrom(this.readBuffer);
                            TeamCommMessage tc_msg = new TeamCommMessage(timestamp, ((InetSocketAddress) address).getHostString(), spl_msg, this.isOpponent);

                            if (address instanceof InetSocketAddress && Plugin.teamcommManager != null)
                            {
                                Plugin.teamcommManager.receivedMessages(Collections.singletonList(tc_msg));
                            }
                        }
                    } catch (Exception ex) {
                        Logger.getLogger(TeamCommViewer.class.getName()).log(Level.INFO, null, ex);
                    }

                }
            } catch (AsynchronousCloseException ex) {
                /* socket was closed, that's fine */
            } catch (SocketException ex) {
                Logger.getLogger(TeamCommViewer.class.getName()).log(Level.SEVERE, null, ex);
            } catch (IOException ex) {
                Logger.getLogger(TeamCommViewer.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
    }//end class RobotTeamCommListener
