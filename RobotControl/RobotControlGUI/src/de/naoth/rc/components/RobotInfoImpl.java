package de.naoth.rc.components;

import com.google.protobuf.InvalidProtocolBufferException;
import de.naoth.rc.RobotControl;
import de.naoth.rc.core.manager.ObjectListener;
import de.naoth.rc.core.manager.SwingCommandExecutor;
import de.naoth.rc.messages.FrameworkRepresentations;
import de.naoth.rc.server.Command;
import de.naoth.rc.server.ConnectionStatusEvent;
import de.naoth.rc.server.ConnectionStatusListener;
import java.awt.FontMetrics;
import java.awt.Point;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.util.HashMap;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import javax.swing.JLabel;
import javax.swing.UIManager;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.events.PluginLoaded;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
@PluginImplementation
public class RobotInfoImpl implements RobotInfo, ConnectionStatusListener
{
    /* required plugins */
    @InjectPlugin
    public static RobotControl rc;
    @InjectPlugin
    public static SwingCommandExecutor commandExecutor;
    
    /* executed (debug) commands */
    private final Command cmd_ri = new Command("Cognition:representation:get").addArg("RobotInfo");
    private final Command cmd_pi = new Command("Cognition:representation:print").addArg("PlayerInfo");
    
    /** the icon/label placed in the RC statusbar */
    private final JLabel l;
    private static final Pattern KEY_VALUE_PATTERN = Pattern.compile("^(?<key>.+)=(?<value>.*)$", Pattern.MULTILINE);
    
    /* vars for the tooltip */
    private String bodyId = "?";
    private String headId = "?";
    private String player = "?";
    private String team   = "?";
    private String scheme = "?";
    
    public RobotInfoImpl() {
        // create icon/label with custom tooltip placement
        l = new ConnectionIcon();
        // disable by default
        l.setEnabled(false);
        updateTooltip();
        
        // we're using a mouse listener to detect a "tooltip show"-event
        // and updating the tooltip text before the tooltip is triggered
        l.addMouseListener(new MouseListener() {
            @Override
            public void mouseClicked(MouseEvent e) {}

            @Override
            public void mousePressed(MouseEvent e) {}

            @Override
            public void mouseReleased(MouseEvent e) {}

            @Override
            public void mouseExited(MouseEvent e) {}

            @Override
            public void mouseEntered(MouseEvent e) {
                updateTooltip();
            }
        });
    }
    
    /**
     * Gets called, when the RC plugin was loaded.
     * Adds the icon/label to the RC statusbar and registers itself as connection listener.
     * 
     * @param robotControl 
     */
    @PluginLoaded
    public void loaded(RobotControl robotControl) {
        rc.addToStatusBar(l);
        rc.getMessageServer().addConnectionStatusListener(this);
    }

    @Override
    public void connected(ConnectionStatusEvent event) {
        l.setEnabled(true);
        // execute commands once
        scheduleCommandsOnce();
    }

    @Override
    public void disconnected(ConnectionStatusEvent event) {
        l.setEnabled(false);
    }
    
    private void updateTooltip() {
        if(l.isEnabled()) {
            // INFO: the number of rows/lines should be equivalent to the tooltip location calculation!
            l.setToolTipText(
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
            l.setToolTipText(null);
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
            }

            @Override
            public void errorOccured(String cause) {/* ignore error */}
        }, cmd_pi);
    }
    
    class ConnectionIcon extends JLabel
    {
        private final int tooltipHeight;

        public ConnectionIcon() {
            setIcon(new javax.swing.ImageIcon(getClass().getResource("/de/naoth/rc/res/network-idle.png")));
            
            // calculate tooltip height: line-height * line-num + spacing-height * spacing-num + statusbar-height
            FontMetrics metrics = this.getFontMetrics(UIManager.getLookAndFeelDefaults().getFont("ToolTip.font"));
            tooltipHeight = metrics.getHeight() * 5 + 5*6 + 25;
        }
        
        @Override
        public Point getToolTipLocation(MouseEvent event) {
            return new Point(0, -tooltipHeight);
        }
    }
}
