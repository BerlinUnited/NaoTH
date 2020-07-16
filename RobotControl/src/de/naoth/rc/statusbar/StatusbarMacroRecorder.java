package de.naoth.rc.statusbar;

import de.naoth.rc.RobotControl;
import de.naoth.rc.RobotControlImpl;
import de.naoth.rc.core.manager.ObjectListener;
import de.naoth.rc.core.manager.SwingCommandExecutor;
import de.naoth.rc.core.server.Command;
import de.naoth.rc.core.server.ConnectionStatusEvent;
import de.naoth.rc.core.server.ConnectionStatusListener;
import de.naoth.rc.core.server.ResponseListener;
import static de.naoth.rc.statusbar.StatusbarPluginImpl.rc;
import java.awt.Component;
import java.awt.event.ActionEvent;
import java.awt.event.MouseEvent;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.Arrays;
import java.util.LinkedList;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.Box;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
@PluginImplementation
public class StatusbarMacroRecorder extends StatusbarPluginImpl implements ObjectListener<Command>, ConnectionStatusListener
{
    @InjectPlugin
    public static SwingCommandExecutor commandExecutor;
    @InjectPlugin
    public static RobotControl robotControl;
    
    private final String tooltipRecording = "Is recording ...";
    private final String tooltipNotRecording = "Not recording";
    
    private final String recordingPrefix = "recording_";
    private final String recordingSuffix = ".bin";
    
    private boolean isRecording = false;
    
    private List<Command> log = new LinkedList<>();
    
    private JMenu menuMacros = new javax.swing.JMenu("Macros");
    private JMenuItem menuStartRecording = new JMenuItem("Start Recording");
    
    @Override
    protected void init() {
        // disable by default
        setEnabled(false);
        setToolTipText(tooltipNotRecording);
        setIcon(new javax.swing.ImageIcon(getClass().getResource("/de/naoth/rc/res/media-record.png")));
        setTooltipHeight(1, 0);
        
        rc.getMessageServer().addConnectionStatusListener(this);
        
        // we're using a mouse listener to detect a "tooltip show"-event
        // and updating the tooltip text before the tooltip is triggered
        addMouseListener(new java.awt.event.MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent e) {
                handleRecording();
            }
        });

        menuStartRecording.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                handleRecording();
            }
        });
        menuMacros.add(menuStartRecording);
        menuMacros.addSeparator();
        menuMacros.setHorizontalTextPosition(LEADING);
        menuMacros.setEnabled(false);
        menuMacros.setToolTipText("Not connected to robot");

        Arrays.asList((new File(robotControl.getConfigPath())).listFiles((dir, name) -> {
            return name.startsWith(recordingPrefix) && name.endsWith(recordingSuffix);
        })).forEach((f) -> {
            String name = f.getName().substring(recordingPrefix.length(), f.getName().length() - recordingSuffix.length());
            addMenuItem(name);
        });

        JMenuBar menu = ((RobotControlImpl)robotControl).getJMenuBar();
        
        int idx = menu.getComponentCount();
        for (Component component : menu.getComponents()) {
            if (component instanceof Box.Filler) {
                idx = menu.getComponentIndex(component);
            }
        }
        menu.add(menuMacros, idx);
    }

    @Override
    protected void exit() {
        stopRecording();
    }
    
    @Override
    public int getWeight() {
        return 101;
    }
    
    private void addMenuItem(String name) {
        JMenuItem item = new JMenuItem(name);
        item.setToolTipText("<html>Replays the recorded commands.<br>Use <i>Ctrl+Click</i> to delete this recording.</html>");
        item.addActionListener((e) -> {
            JMenuItem source = (JMenuItem) e.getSource();
            if((e.getModifiers() & ActionEvent.CTRL_MASK) == ActionEvent.CTRL_MASK) {
                // delete requested
                if(JOptionPane.showConfirmDialog(((RobotControlImpl)robotControl), "Do you want to remove the recording '"+name+"'?", "Remove recording", JOptionPane.YES_NO_OPTION) == JOptionPane.YES_OPTION) {
                    if(recordingFileName(name).delete()) { menuMacros.remove(source); }
                }
            } else {
                // restore dialog configuration
                File f = recordingFileName(source.getText());
                if(f.isFile()) {
                    try {
                        loadRecording(f);
                    } catch (IOException ex) {
                        Logger.getLogger(RobotControlImpl.class.getName()).log(Level.SEVERE, null, ex);
                    }
                } else {
                    JOptionPane.showMessageDialog(((RobotControlImpl)robotControl), "The '"+source.getText()+"' recording file doesn't exists!?", "Missing recording file", JOptionPane.ERROR_MESSAGE);
                }
            }
        });
        menuMacros.add(item);
    }
    
    private void handleRecording() {
        if (isRecording) {
            stopRecording();
        } else {
            startRecording();
        }
    }
    
    private void startRecording() {
        isRecording = true;
        setEnabled(true);
        setToolTipText(tooltipRecording);
        rc.getMessageServer().addListener(this);
        
        menuMacros.setIcon(new javax.swing.ImageIcon(getClass().getResource("/de/naoth/rc/res/media-record.png")));
        menuStartRecording.setText("Stop Recording");
    }
    
    private void stopRecording() {
        isRecording = false;
        setEnabled(false);
        setToolTipText(tooltipNotRecording);
        rc.getMessageServer().removeListener(this);
        
        menuMacros.setIcon(null);
        menuStartRecording.setText("Start Recording");
        
        saveRecording();
    }
    
    private File recordingFileName(String name) {
        return new File(robotControl.getConfigPath() + "/" + recordingPrefix + name + recordingSuffix);
    }
    
    private void saveRecording() {
        // check if something was recorded
        if (log.isEmpty()) {
            JOptionPane.showMessageDialog(
                    ((RobotControlImpl)robotControl), 
                    "No commands were recorded.", 
                    "Nothing recorded", 
                    JOptionPane.WARNING_MESSAGE);
            return;
        }
        
        // Ask for a name for this dialog configuration
        String inputName = JOptionPane.showInputDialog("Set a name for the recorded commands");
        // ignore canceled inputs
        if(inputName == null) { return; }
        // replace "invalid" characters
        String name = inputName.trim().replaceAll("[^A-Za-z0-9_-]", "");
        // ignore empty inputs
        if(name.isEmpty()) { return; }
        // create record file
        File file = recordingFileName(name);
        // TODO: should the file be overwritten?
        // write the logged commands to the record file
        try {
            FileOutputStream stream = new FileOutputStream(file);
            ObjectOutputStream o = new ObjectOutputStream(stream);
            o.writeObject(log);
            o.close();
            stream.close();
            // add the new recording to the macro menu
            addMenuItem(name);
        } catch (FileNotFoundException ex) {
            Logger.getLogger(StatusbarMacroRecorder.class.getName()).log(Level.SEVERE, null, ex);
        } catch (IOException ex) {
            Logger.getLogger(StatusbarMacroRecorder.class.getName()).log(Level.SEVERE, null, ex);
        }
        
        log.clear();
    }
    
    private void loadRecording(File f) throws IOException {
        // if not connect, do not replay commands
        if (!rc.getMessageServer().isConnected()) {
            return;
        }
        
        FileInputStream stream = new FileInputStream(f);
        ObjectInputStream oi = new ObjectInputStream(stream);

        try {
            List<Command> commands = (List<Command>) oi.readObject();
            for (Command command : commands) {
                rc.getMessageServer().executeCommand(new ResponseListener() {
                    @Override
                    public void handleResponse(byte[] result, Command command) {
                        System.out.println("Successfully executed command: " + command.getName());
                    }

                    @Override
                    public void handleError(int code) {
                        System.out.println("Error during command execution, error code: " + code);
                    }
                }, command);
            }
        } catch (ClassNotFoundException ex) {
            Logger.getLogger(StatusbarMacroRecorder.class.getName()).log(Level.SEVERE, null, ex);
        }

        oi.close();
        stream.close();
    }
    
    @Override
    public void connected(ConnectionStatusEvent event) {
        menuMacros.setEnabled(true);
        menuMacros.setToolTipText(null);
    }

    @Override
    public void disconnected(ConnectionStatusEvent event) {
        menuMacros.setEnabled(false);
        menuMacros.setToolTipText("Not connected to robot");
    }

    @Override
    public void newObjectReceived(Command cmd) {
        // only store the 'set' commands
        if (cmd.getName().endsWith(":set") || cmd.getName().endsWith(":set_agent")) {
            log.add(cmd);
        }
    }

    @Override
    public void errorOccured(String cause) { /* should never happen! */ }
}
