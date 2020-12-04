package de.naoth.rc.plugins;

import de.naoth.rc.RobotControl;
import de.naoth.rc.RobotControlImpl;
import de.naoth.rc.core.manager.ObjectListener;
import de.naoth.rc.core.manager.SwingCommandExecutor;
import de.naoth.rc.core.server.Command;
import de.naoth.rc.core.server.ConnectionStatusEvent;
import de.naoth.rc.core.server.ConnectionStatusListener;
import de.naoth.rc.core.server.ResponseListener;
import java.awt.Component;
import java.awt.event.ActionEvent;
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
import javax.swing.SwingConstants;
import net.xeoh.plugins.base.Plugin;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.events.PluginLoaded;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
@PluginImplementation
public class CommandRecorder implements Plugin, ConnectionStatusListener, ObjectListener<Command>
{
    @InjectPlugin
    public static RobotControl robotControl;
    
    @InjectPlugin
    public static SwingCommandExecutor commandExecutor;
    
    private static final String REC_PREFIX = "cmd_recording_";
    private static final String REC_SUFFIX = ".bin";
    private static final String MENU_TOOLTIP = "Not connected to robot";
    private static final String MENU_REC_START = "Start Recording";
    private static final String MENU_REC_STOP = "Stop Recording";
    
    private boolean isRecording = false;
    private final List<Command> log = new LinkedList<>();
    
    private final JMenu menuMacros = new javax.swing.JMenu("Macros");
    private final JMenuItem menuStartRecording = new JMenuItem(MENU_REC_START);

    /**
     * Gets called, when the RC plugin was loaded.Adds the "Macros" menu item to the RC menubar and registers itself as connection listener.
     *
     * @param rc the RC instance (JFrame)
     */
    @PluginLoaded
    public void loaded(RobotControl rc) {
        robotControl.getMessageServer().addConnectionStatusListener(this);
        
        menuMacros.setEnabled(false);
        menuMacros.setToolTipText(MENU_TOOLTIP);
        menuMacros.setHorizontalTextPosition(SwingConstants.LEADING);
        menuMacros.add(menuStartRecording);
        menuMacros.addSeparator();

        menuStartRecording.addActionListener((ActionEvent e) -> { toggleRecording(); });
        
        addRecordingsToMenu();
        insertMacrosMenuToMenuBar();
    }
    
    /**
     * Inserts the "Macros" menu item to the end of the left menubar part.
     */
    private void insertMacrosMenuToMenuBar() {
        JMenuBar menu = ((RobotControlImpl)robotControl).getJMenuBar();
        // find the index of the 'Filler', in order to insert the menu before it
        int idx = menu.getComponentCount();
        for (Component component : menu.getComponents()) {
            if (component instanceof Box.Filler) {
                idx = menu.getComponentIndex(component);
            }
        }
        menu.add(menuMacros, idx);
    }
    
    /**
     * Searches for recoding files and adds them to the macro menu.
     */
    private void addRecordingsToMenu() {
        Arrays.asList((new File(robotControl.getConfigPath())).listFiles((dir, name) -> {
            return name.startsWith(REC_PREFIX) && name.endsWith(REC_SUFFIX);
        })).forEach((f) -> {
            String name = f.getName().substring(REC_PREFIX.length(), f.getName().length() - REC_SUFFIX.length());
            addMenuItem(name);
        });
    }

    /**
     * Adds a recording to the macros menu.
     * Also sets the tooltip and handles the click events.
     * 
     * @param name  the recording name
     */
    private void addMenuItem(String name) {
        JMenuItem item = new JMenuItem(name);
        item.setToolTipText("<html>Replays the recorded commands.<br>Use <i>Ctrl+Click</i> to delete this recording.</html>");
        item.addActionListener((e) -> {
            JMenuItem source = (JMenuItem) e.getSource();
            if((e.getModifiers() & ActionEvent.CTRL_MASK) == ActionEvent.CTRL_MASK) {
                // delete requested
                if(JOptionPane.showConfirmDialog(
                        ((RobotControlImpl)robotControl), 
                        "Do you want to remove the recording '"+name+"'?", 
                        "Remove recording", 
                        JOptionPane.YES_NO_OPTION) == JOptionPane.YES_OPTION) {
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
                    JOptionPane.showMessageDialog(
                            ((RobotControlImpl)robotControl), 
                            "The '"+source.getText()+"' recording file doesn't exists!?", 
                            "Missing recording file", 
                            JOptionPane.ERROR_MESSAGE);
                }
            }
        });
        menuMacros.add(item);
    }
    
    /**
     * Starts or stops the recording, based on the current state (helper method).
     */
    private void toggleRecording() {
        if (isRecording) {
            stopRecording();
        } else {
            startRecording();
        }
    }
    
    /**
     * Starts the recording.
     * Registers to the message server and updates the ui.
     */
    private void startRecording() {
        isRecording = true;
        robotControl.getMessageServer().addListener(this);
        
        menuMacros.setIcon(new javax.swing.ImageIcon(getClass().getResource("/de/naoth/rc/res/media-record.png")));
        menuStartRecording.setText(MENU_REC_STOP);
    }
    
    /**
     * Stops the recording.
     * Unregisters from the message server, updates the ui and saves the recording.
     */
    private void stopRecording() {
        isRecording = false;
        robotControl.getMessageServer().removeListener(this);
        
        menuMacros.setIcon(null);
        menuStartRecording.setText(MENU_REC_START);
        
        saveRecording();
    }
    
    /**
     * Returns the File for a given recording name (helper method).
     * 
     * @param name  the name of the recording
     * @return      the file object pointing to the file where the recording is/should be saved
     */
    private File recordingFileName(String name) {
        return new File(robotControl.getConfigPath() + "/" + REC_PREFIX + name + REC_SUFFIX);
    }
    
    /**
     * Actually saves the recorded commands.
     * Therefore checks whether commands were recorded and ask for a name for this
     * recording. The recorded commands are written as serialized java objects to the file.
     */
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
        String inputName = JOptionPane.showInputDialog(
                ((RobotControlImpl)robotControl), 
                "<html>Set a name for the recorded commands<br>"
                + "<small>Only alphanumerical characters are allowed and "
                + "whitespaces are replaced with '_'.</small></html>",
                "Name of recording",
                JOptionPane.QUESTION_MESSAGE);
        // ignore canceled inputs
        if(inputName == null) { return; }
        // replace whitespaces with "_" and remove all other "invalid" characters
        String name = inputName.trim()
                               .replaceAll("[\\s+]", "_")
                               .replaceAll("[^A-Za-z0-9_-]", "");
        // ignore empty inputs
        if(name.isEmpty()) { return; }
        // create record file
        File file = recordingFileName(name);
        // if recording name already exists - ask to overwrite
        if(file.isFile() && JOptionPane.showConfirmDialog(
                ((RobotControlImpl)robotControl), 
                "This dialog layout name already exists. Overwrite?",
                "Overwrite",
                JOptionPane.YES_NO_OPTION) != JOptionPane.YES_OPTION) {
            // don't overwrite!
            return;
        }
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
            Logger.getLogger(CommandRecorder.class.getName()).log(Level.SEVERE, null, ex);
        } catch (IOException ex) {
            Logger.getLogger(CommandRecorder.class.getName()).log(Level.SEVERE, null, ex);
        }
        
        log.clear();
    }
    
    /**
     * Loads the recorded commands from the given file.
     * 
     * @param f
     * @throws IOException 
     */
    private void loadRecording(File f) throws IOException {
        // if not connect, do not replay commands
        if (!robotControl.getMessageServer().isConnected()) {
            return;
        }
        
        FileInputStream stream = new FileInputStream(f);
        ObjectInputStream oi = new ObjectInputStream(stream);

        try {
            List<Command> commands = (List<Command>) oi.readObject();
            for (Command command : commands) {
                robotControl.getMessageServer().executeCommand(new ResponseListener() {
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
            Logger.getLogger(CommandRecorder.class.getName()).log(Level.SEVERE, null, ex);
        }

        oi.close();
        stream.close();
    }
    
    /**
     * Enables the macros menu.
     * 
     * @param event 
     */
    @Override
    public void connected(ConnectionStatusEvent event) {
        menuMacros.setEnabled(true);
        menuMacros.setToolTipText(null);
    }

    /**
     * Disables the macros menu and sets the tooltip.
     * @param event 
     */
    @Override
    public void disconnected(ConnectionStatusEvent event) {
        menuMacros.setEnabled(false);
        menuMacros.setToolTipText(MENU_TOOLTIP);
    }

    /**
     * Records all 'set' commands.
     * 
     * @param cmd 
     */
    @Override
    public void newObjectReceived(Command cmd) {
        // only store the 'set' commands
        if (cmd.getName().endsWith(":set") || cmd.getName().endsWith(":set_agent")) {
            log.add(cmd);
            Logger.getLogger(CommandRecorder.class.getName()).log(Level.INFO, "Command logged: " + cmd.getName());
        }
    }

    /**
     * Ignore any errors.
     * 
     * @param cause 
     */
    @Override
    public void errorOccured(String cause) { /* should never happen! */ }
}
