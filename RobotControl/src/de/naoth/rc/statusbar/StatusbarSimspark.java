package de.naoth.rc.statusbar;

import de.naoth.rc.RobotControlImpl;
import de.naoth.rc.components.simspark.SimsparkListener;
import de.naoth.rc.components.simspark.SimsparkManager;
import de.naoth.rc.components.simspark.SimsparkMonitor;
import de.naoth.rc.dataformats.SimsparkState;
import java.awt.event.KeyEvent;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.beans.property.BooleanProperty;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.KeyStroke;
import javax.swing.SwingUtilities;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 * Statusbar plugin to connect to a simspark instance and sending monitor commands to simspark.
 * 
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
@PluginImplementation
public class StatusbarSimspark extends StatusbarPluginImpl implements SimsparkListener
{
    @InjectPlugin
    public SimsparkManager simsparkManger;

    /* some vars */
    private ConnectionDialog connectionDialog;
    private CommandDialog commandDialog;
    private SimsparkMonitor simspark_comm;
    
    private final String defaultTooltip = "Not connected to simspark.";
    
    @Override
    protected void init() {
        // create connection dialog
        this.connectionDialog = new ConnectionDialog((RobotControlImpl)rc);
        this.connectionDialog.setLocationRelativeTo((RobotControlImpl)rc);
        // create command dialog
        commandDialog = new CommandDialog((RobotControlImpl)rc);
        commandDialog.setLocationRelativeTo((RobotControlImpl)rc);
        
        // set icon
        setIcon(new javax.swing.ImageIcon(getClass().getResource("/de/naoth/rc/res/simspark20.png")));

        // disable by default
        setEnabled(false);

        // default tooltip
        setTooltipHeight(1, 0);
        setToolTipText(defaultTooltip);

        addMouseListener(new java.awt.event.MouseAdapter() {
            @Override
            public void mouseClicked(java.awt.event.MouseEvent evt) {
                if(!isEnabled()) {
                    // call connection dialog
                    connectionDialog.setVisible(true);
                } else {
                    if(SwingUtilities.isRightMouseButton(evt)) {
                        commandDialog.setVisible(true);
                    } else {
                        disconnect();
                    }
                }
            }
        });
    }

    @Override
    protected void exit() {
        disconnect();
    }

    private void connect(String host, String port) {
        try {
            if(simspark_comm != null) {
                simspark_comm.disconnect();
            }
            simspark_comm = new SimsparkMonitor();
            // register connection listener
            simspark_comm.isConnected.addListener((o) -> {
                if(((BooleanProperty)o).get()) {
                    this.onConnect();
                } else {
                    disconnect();
                    onDisconnect();
                }
            });
            simspark_comm.connect(host.trim(), Integer.parseInt(port.trim()));
            connectionDialog.setVisible(false);
        } catch (IOException | InterruptedException ex) {
            JOptionPane.showMessageDialog(null, "Couldn't connect!", "Couldn't connect", JOptionPane.WARNING_MESSAGE);
        }
    }
    
    private void disconnect() {
        try {
            if(simspark_comm != null) {
                simspark_comm.disconnect();
            }
            simspark_comm = null;
        } catch (IOException | InterruptedException ex) {
            Logger.getLogger(StatusbarSimspark.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
    
    private void onConnect() {
        if(simsparkManger != null) {
            simsparkManger.addSimsparkListener(this);
        }

        // update UI
        setEnabled(true);
        setToolTipText("Connected to simspark ("+simspark_comm.getHost()+":"+simspark_comm.getPort()+")");
    }
    
    private void onDisconnect() {
        if(simsparkManger != null) {
            simsparkManger.removeSimsparkListener(this);
        }
        setEnabled(false);
        setToolTipText(defaultTooltip);
        setTooltipHeight(1, 0);
    }

    @Override
    public void newSimsparkData(SimsparkState state) {
        String ttt = state.data.entrySet().stream().map(
            (entry) -> {
                if(entry.getKey().equals("messages")) {
                    return "";
                }
                return "<tr><td align=\"right\"><b>"+entry.getKey()+":</b></td><td>" + entry.getValue() + "</td></tr>";
            } ).reduce("", String::concat);
        setTooltipHeight(state.data.size(), 6);
        setToolTipText("<html>" + "<table border=\"0\">" + ttt + "</table>" + "</html>");
    }
    
    private void sendCommand(String cmd) {
        if(simspark_comm == null || !simspark_comm.isAlive()) {
            JOptionPane.showMessageDialog(this, "Not connected to simspark!", "Not connected", JOptionPane.ERROR_MESSAGE);
        } else {
            try {
                System.out.println(cmd);
                commandDialog.lastSendCommand.setText(" Last command: "+cmd);
                simspark_comm.sendMessage(cmd);
            } catch (IOException ex) {
                if(!simspark_comm.checkConnection()) {
                    JOptionPane.showMessageDialog(null, "Connection to server lost!\nStill running?", "Lost connection", JOptionPane.WARNING_MESSAGE);
                }
            }
        }
    }

    /**
     * A simple connection dialog for simspark.
     */
    class ConnectionDialog extends javax.swing.JDialog
    {
        private javax.swing.JButton btnCancel;
        private javax.swing.JButton btnConnect;
        private javax.swing.JTextField txtHost;
        private javax.swing.JLabel jLabel1;
        private javax.swing.JTextField txtPort;
        
        public ConnectionDialog(java.awt.Frame parent) {
            super(parent, true);
            initComponents();
            
            getRootPane().setDefaultButton(this.btnConnect);
            getRootPane().registerKeyboardAction((e) -> {
                setVisible(false);
            }, KeyStroke.getKeyStroke(KeyEvent.VK_ESCAPE, 0), JComponent.WHEN_IN_FOCUSED_WINDOW);
        }

        private void initComponents() {

            txtHost = new javax.swing.JTextField("127.0.0.1");
            jLabel1 = new javax.swing.JLabel();
            txtPort = new javax.swing.JTextField("3200");
            btnConnect = new javax.swing.JButton();
            btnCancel = new javax.swing.JButton();

            setTitle("Connect");
            setLocationByPlatform(true);

            txtHost.setHorizontalAlignment(javax.swing.JTextField.RIGHT);
            jLabel1.setText(":");
            txtPort.setHorizontalAlignment(javax.swing.JTextField.LEFT);

            btnConnect.setMnemonic('c');
            btnConnect.setText("Connect");
            btnConnect.setToolTipText("Connect with this IP and port");
            btnConnect.addActionListener((java.awt.event.ActionEvent evt) -> {
                connect(txtHost.getText(), txtPort.getText());
            });

            btnCancel.setMnemonic('e');
            btnCancel.setText("Cancel");
            btnCancel.setToolTipText("Do not connect and close this dialog");
            btnCancel.addActionListener((java.awt.event.ActionEvent evt) -> {
                this.setVisible(false);
            });

            javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
            getContentPane().setLayout(layout);
            layout.setHorizontalGroup(
                layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(layout.createSequentialGroup()
                        .addContainerGap()
                        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addGroup(layout.createSequentialGroup()
                                .addComponent(txtHost, javax.swing.GroupLayout.PREFERRED_SIZE, 189, javax.swing.GroupLayout.PREFERRED_SIZE)
                                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                                .addComponent(jLabel1)
                                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                                .addComponent(txtPort, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                            .addGroup(layout.createSequentialGroup()
                                .addComponent(btnConnect)
                                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                                .addComponent(btnCancel)))
                        .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
            );
            layout.setVerticalGroup(
                layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(layout.createSequentialGroup()
                        .addContainerGap()
                        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                            .addComponent(jLabel1)
                            .addComponent(txtPort, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                            .addComponent(txtHost, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                            .addComponent(btnConnect)
                            .addComponent(btnCancel))
                        .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
            );
            pack();
        }
    } // end ConnectionDialog

    /**
     * A dialog for sending monitor commands to the connected simspark instance.
     */
    class CommandDialog extends javax.swing.JDialog
    {
        JPanel jPanel6 = new javax.swing.JPanel();
        JComboBox<Object> cmd = new javax.swing.JComboBox<>();
        JButton btnSendCommand = new javax.swing.JButton();
        JComboBox<Command> cmdPredefined = new javax.swing.JComboBox<>();
        JLabel lastSendCommand = new JLabel();
            
        public CommandDialog(java.awt.Frame parent) {
            super(parent, true);
            initComponents();
            
            getRootPane().registerKeyboardAction((e) -> {
                setVisible(false);
            }, KeyStroke.getKeyStroke(KeyEvent.VK_ESCAPE, 0), JComponent.WHEN_IN_FOCUSED_WINDOW);
        }
        
        public void initComponents() {
            // set dialog properties
            setTitle("Command Dialog");
            setSize(new java.awt.Dimension(600, 85));
            setModal(false);
            getContentPane().setLayout(new java.awt.GridLayout(3, 1));

            // init dialog components
            btnSendCommand.setText("Send Command");
            btnSendCommand.setFocusable(false);
            btnSendCommand.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
            btnSendCommand.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
            btnSendCommand.addActionListener((java.awt.event.ActionEvent evt) -> {
                if (((String) cmd.getSelectedItem()).isEmpty()) {
                    JOptionPane.showMessageDialog(this, "Please enter a (valid) command.", "Empty command", JOptionPane.WARNING_MESSAGE);
                } else {
                    sendCommand(((String) cmd.getEditor().getItem()).trim());
                }
            });

            cmd.setEditable(true);
            cmd.setModel(new javax.swing.DefaultComboBoxModel<>(new String[]{"(ball (pos 0 0 0))", "(ball (pos 0 0 0)(vel 0 0 0))", "(dropBall)", "(playMode )", "(agent (unum <num>) (team <team>) (pos <x> <y> <z>))", "(agent (unum <num>) (team <team>) (move <x> <y> <z> <rot>))", "(agent (unum <num>) (team <team>) (battery <batterylevel>))", "(agent (unum <num>) (team <team>) (temperature <temperature>))", "(agent (unum <num>) (team <team>) (pos <x> <y> <z>)(move <x> <y> <z> <rot>)(battery <batterylevel>)(temperature <temperature>))", "(kickOff <team>)", "(select (unum <num>) (team <team>))", "(kill)", "(kill (unum <num>) (team <team>))", "(repos)", "(repos (unum <num>) (team <team>))", "(time <time>)", "(score (left <score>) (right <score>))", "(reqfullstate)"}));
            cmd.addActionListener((java.awt.event.ActionEvent evt) -> {
                if (evt.getActionCommand().equals("comboBoxEdited")) {
                    // "click the send button"
                    btnSendCommand.doClick();
                }
            });

            javax.swing.GroupLayout jPanel6Layout = new javax.swing.GroupLayout(jPanel6);
            jPanel6.setLayout(jPanel6Layout);
            jPanel6Layout.setHorizontalGroup(
                jPanel6Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(jPanel6Layout.createSequentialGroup()
                        .addComponent(cmd, 0, 319, Short.MAX_VALUE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(btnSendCommand))
            );
            jPanel6Layout.setVerticalGroup(
                jPanel6Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(btnSendCommand, javax.swing.GroupLayout.Alignment.TRAILING)
                    .addComponent(cmd, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
            );

            getContentPane().add(jPanel6);

            cmdPredefined.setModel(new javax.swing.DefaultComboBoxModel<>(new Command[]{
                new Command("--- Predefined commands ---","---"),
                new Command("DropBall",               "(dropBall)"),
                new Command("KickOff",                "(kickOff None)"),
                new Command("KickOff Left",           "(kickOff Left)"),
                new Command("KickOff Right",          "(kickOff Right)"),
                new Command("Ball Center Circle",     "(ball (pos 0 0 0))"),
                new Command("Ball Right Lower Corner","(ball (pos 4.5 -3 0))"),
                new Command("Ball Right Upper Corner","(ball (pos 4.5 3 0))"),
                new Command("Ball Left Lower Corner", "(ball (pos -4.5 -3 0))"),
                new Command("Ball Left Upper Corner", "(ball (pos -4.5 3 0))"),
                new Command("---",                    "---"),
                new Command("Close Simspark",         "(killsim)"),
            }));
            cmdPredefined.addActionListener((java.awt.event.ActionEvent evt) -> {
                Command item = (Command) cmdPredefined.getSelectedItem();
                if(!item.cmd.equals("---")) {
                    sendCommand(item.cmd);
                }
            });
            getContentPane().add(cmdPredefined);
            lastSendCommand.setText(" Last command: ");
            getContentPane().add(lastSendCommand);

            pack();
        }
        
        /* Helper class for predefined commands */
        class Command {
            public String name;
            public String cmd;
            public Command(String name, String cmd) {
                this.name = name;
                this.cmd = cmd;
            }

            @Override
            public String toString() {
                return name;
            }
        }
    } // END CommandDialog
}
