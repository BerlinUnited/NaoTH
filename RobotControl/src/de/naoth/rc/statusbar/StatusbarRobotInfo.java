package de.naoth.rc.statusbar;

import com.google.protobuf.InvalidProtocolBufferException;
import de.naoth.rc.RobotControlImpl;
import de.naoth.rc.core.manager.ObjectListener;
import de.naoth.rc.core.manager.SwingCommandExecutor;
import de.naoth.rc.core.messages.FrameworkRepresentations;
import de.naoth.rc.core.server.Command;
import de.naoth.rc.core.server.ConnectionStatusEvent;
import de.naoth.rc.core.server.ConnectionStatusListener;
import static de.naoth.rc.statusbar.StatusbarPluginImpl.rc;
import java.awt.event.MouseEvent;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import javax.swing.JOptionPane;
import javax.swing.SwingUtilities;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
@PluginImplementation
public class StatusbarRobotInfo extends StatusbarPluginImpl implements ConnectionStatusListener
{
    @InjectPlugin
    public static SwingCommandExecutor commandExecutor;
    
    /* executed (debug) commands */
    private final Command cmd_ri = new Command("Cognition:representation:get").addArg("RobotInfo");
    private final Command cmd_pi = new Command("Cognition:representation:print").addArg("PlayerInfo");
    
    /** the icon/label placed in the RC statusbar */
    private static final Pattern KEY_VALUE_PATTERN = Pattern.compile("^(?<key>.+)=(?<value>.*)$", Pattern.MULTILINE);
    
    /* vars for the tooltip */
    private String bodyId = "?";
    private String headId = "?";
    private String player = "?";
    private String team   = "?";
    private String scheme = "?";
    
    @Override
    protected void init() {
        // disable by default
        setEnabled(false);
        updateTooltip();
        
        // we're using a mouse listener to detect a "tooltip show"-event
        // and updating the tooltip text before the tooltip is triggered
        addMouseListener(new java.awt.event.MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent e) {
                if(rc.getMessageServer().isConnected()) {
                    // disconnect from robot
                    rc.getMessageServer().disconnect();
                } else if(SwingUtilities.isRightMouseButton(e)) {
                    // connect to robot with last used host/port
                    String host = rc.getConfig().getProperty("hostname");
                    String port = rc.getConfig().getProperty("port");
                    if(host == null || port == null) {
                        JOptionPane.showMessageDialog((RobotControlImpl)rc, "There is no last used connection info available.", "Disconnect", JOptionPane.ERROR_MESSAGE);
                    } else {
                        rc.getMessageServer().connect(host, Integer.parseInt(port));
                    }
                } else {
                    // connect to robot with connection dialog
                    rc.checkConnected();
                }
            }
        });
        
        rc.getMessageServer().addConnectionStatusListener(this);
        
        setIcon(new javax.swing.ImageIcon(getClass().getResource("/de/naoth/rc/res/network-idle.png")));
        setText("Not connected");
        setTooltipHeight(1, 0);
    }

    @Override
    protected void exit() {
        rc.getMessageServer().removeConnectionStatusListener(this);
    }
    
    @Override
    public void connected(ConnectionStatusEvent event) {
        setEnabled(true);
        setText("Connected to " + event.getAddress());
        // execute commands once
        scheduleCommandsOnce();
        updateTooltip();
    }

    @Override
    public void disconnected(ConnectionStatusEvent event) {
        setEnabled(false);
        setText("Not connected");
        updateTooltip();
    }
    
    private void updateTooltip() {
        if(isEnabled()) {
            // INFO: the number of rows/lines should be equivalent to the tooltip location calculation!
            setTooltipHeight(5, 6);
            setToolTipText(
                  "<html>"
                + "<table border=\"0\">"
                + "<tr><td align=\"right\"><b>BodyID:</b></td><td>"+headId+"</td></tr>"
                + "<tr><td align=\"right\"><b>HeadID:</b></td><td>"+bodyId+"</td></tr>"
                + "<tr><td align=\"right\"><b>Player number:</b></td><td>"+player+"</td></tr>"
                + "<tr><td align=\"right\"><b>Team number:</b></td><td>"+team+"</td></tr>"
                + "<tr><td align=\"right\"><b>Scheme:</b></td><td>"+scheme+"</td></tr>"
                + "</table>"
                + "</html>"
            );
        } else {
            // "disable" tooltip
            setTooltipHeight(1, 0);
            setToolTipText("Indicates if the RobotControl is connected to a Robot");
        }
    }

    private void scheduleCommandsOnce() {
        /* command for RobotInfo */
        commandExecutor.executeCommand(new ObjectListener<byte[]>() {
            @Override
            public void newObjectReceived(byte[] object) {
                try {
                    FrameworkRepresentations.RobotInfo info = FrameworkRepresentations.RobotInfo.parseFrom(object);
                    headId = info.getHeadNickName();
                    bodyId = info.getBodyNickName();
                    updateTooltip();
                } catch (InvalidProtocolBufferException ex) {/* ignore exception */}
            }

            @Override
            public void errorOccured(String cause) {/* ignore error */}
        }, cmd_ri);
        
        /* command for PlayerInfo */
        commandExecutor.executeCommand(new ObjectListener<byte[]>() {
            @Override
            public void newObjectReceived(byte[] object) {
                // extract representation from string
                Map<String, String> repr = new HashMap<>();
                Matcher m = KEY_VALUE_PATTERN.matcher(new String(object));
                while (m.find()) {
                    repr.put(m.group("key").trim(), m.group("value").trim());
                }
                // set vars
                player = repr.getOrDefault("playerNumber", "?");
                team = repr.getOrDefault("teamNumber", "?");
                scheme = repr.getOrDefault("active scheme", "?");
                updateTooltip();
            }

            @Override
            public void errorOccured(String cause) {/* ignore error */}
        }, cmd_pi);
    }

    @Override
    public int getWeight() {
        return 100;
    }
}
