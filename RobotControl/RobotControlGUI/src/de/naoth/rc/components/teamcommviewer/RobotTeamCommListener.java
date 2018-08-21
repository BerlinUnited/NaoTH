package de.naoth.rc.components.teamcommviewer;

import de.naoth.rc.components.teamcomm.TeamCommManager;
import de.naoth.rc.components.teamcomm.TeamCommMessage;
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
                        SPLMessage spl_msg = SPLMessage.parseFrom(this.readBuffer);
                        TeamCommMessage tc_msg = new TeamCommMessage(timestamp, ((InetSocketAddress) address).getHostString(), spl_msg, this.isOpponent);

                        if (address instanceof InetSocketAddress && Plugin.teamcommManager != null) {
                            Plugin.teamcommManager.receivedMessages(Collections.singletonList(tc_msg));
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
