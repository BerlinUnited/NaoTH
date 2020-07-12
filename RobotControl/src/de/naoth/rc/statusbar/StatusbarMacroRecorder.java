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
import java.util.LinkedList;
import java.util.List;
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
public class StatusbarMacroRecorder extends StatusbarPluginImpl implements ObjectListener<Command>
{
    @InjectPlugin
    public static SwingCommandExecutor commandExecutor;
    
    private boolean isRecording = false;
    
    private final String tooltipRecording = "Is recording ...";
    private final String tooltipNotRecording = "Not recording";
    
    private List<Command> log = new LinkedList<>();
    
    @Override
    protected void init() {
        // disable by default
        setEnabled(false);
        setToolTipText(tooltipNotRecording);
        setIcon(new javax.swing.ImageIcon(getClass().getResource("/de/naoth/rc/res/media-record.png")));
        setTooltipHeight(1, 0);
        
        // we're using a mouse listener to detect a "tooltip show"-event
        // and updating the tooltip text before the tooltip is triggered
        addMouseListener(new java.awt.event.MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent e) {
                if (isRecording) {
                    stopRecording();
                } else {
                    startRecording();
                }
            }
        });
    }

    @Override
    protected void exit() {
        stopRecording();
    }
    
    @Override
    public int getWeight() {
        return 100;
    }
    
    private void startRecording() {
        isRecording = true;
        setEnabled(true);
        setToolTipText(tooltipRecording);
        rc.getMessageServer().addListener(this);
    }
    
    
    private void stopRecording() {
        isRecording = false;
        setEnabled(false);
        setToolTipText(tooltipNotRecording);
        rc.getMessageServer().removeListener(this);
        
        System.out.println("Saved commands: " + log.size());
        
        log.clear();
    }

    @Override
    public void newObjectReceived(Command cmd) {
        //System.out.println(cmd);
        System.out.println(cmd.getName());
        
        if (cmd.getName().endsWith(":set") || cmd.getName().endsWith(":set_agent")) {
            log.add(cmd);
        }
    }

    @Override
    public void errorOccured(String cause) { /* should never happen! */ }
}
