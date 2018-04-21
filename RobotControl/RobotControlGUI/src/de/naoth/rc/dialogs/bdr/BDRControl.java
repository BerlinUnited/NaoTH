package de.naoth.rc.dialogs.bdr;

import com.google.protobuf.InvalidProtocolBufferException;
import de.naoth.bdr.RobotControlBdrMonitor;
import de.naoth.bdr.RobotControlBdrMonitorImpl;
import de.naoth.rc.RobotControl;
import de.naoth.rc.components.teamcomm.TeamCommListener;
import de.naoth.rc.components.teamcomm.TeamCommManager;
import de.naoth.rc.components.teamcomm.TeamCommMessage;
import de.naoth.rc.components.teamcomm.TeamCommUDPReceiver;
import de.naoth.rc.core.dialog.AbstractDialog;
import de.naoth.rc.core.dialog.DialogPlugin;
import de.naoth.rc.core.dialog.RCDialog;
import de.naoth.rc.core.manager.ObjectListener;
import de.naoth.rc.core.manager.SwingCommandExecutor;
import de.naoth.rc.dataformats.SPLMessage;
import de.naoth.rc.dialogs.RobotHealth;
import de.naoth.rc.dialogs.TeamCommViewer;
import de.naoth.rc.messages.BDRMessages.BDRControlCommand;
import de.naoth.rc.messages.BDRMessages.BDRBehaviorMode;
import de.naoth.rc.server.Command;
import de.naoth.rc.server.ConnectionStatusEvent;
import de.naoth.rc.server.ConnectionStatusListener;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ComponentEvent;
import java.awt.event.ComponentListener;
import java.util.List;
import java.util.TreeMap;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.Timer;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.events.PluginLoaded;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 *
 * @author Heinrich Mellmann
 */
public class BDRControl extends AbstractDialog implements TeamCommListener, ComponentListener, ConnectionStatusListener, ActionListener
{
    @RCDialog(category = RCDialog.Category.BDR, name = "Control")
    @PluginImplementation
    public static class Plugin extends DialogPlugin<BDRControl> {

        @InjectPlugin
        public static RobotControl rc;
        @InjectPlugin
        public static RobotControlBdrMonitor parent;
        @InjectPlugin
        public static SwingCommandExecutor commandExecutor;
        @InjectPlugin
        public static TeamCommManager teamcommManager;
        @InjectPlugin
        public static TeamCommUDPReceiver teamCommUDPReceiver;
    }//end Plugin

    private final int port = 10004;
    private final int maxTimeRobotIsDead = 2000; //ms
    
    private final TreeMap<String, RobotPanel> robotsMap = new TreeMap<>();
    
    private final int robotPanelGap = 5;
    int robotPanelHeight = 300;
    int robotPanelWidth = 200;
    
    Timer updater;
    
    public BDRControl() 
    {
        // init ui
        initComponents();
        setButtons(false);

        // set listeners
        addComponentListener(this);
        Plugin.teamcommManager.addListener(this);
        Plugin.teamCommUDPReceiver.connect(this, port);
        Plugin.rc.getMessageServer().addConnectionStatusListener(this);
        
        
        // schedules canvas drawing at a fixed rate, should prevent "flickering"
        this.updater = new Timer(300, this);
        this.updater.start();
        
        // add listener, if the health panel changes
        RobotControlBdrMonitorImpl.healthPanel.addListener((o) -> {
            setHealthPanel();
        });
        // if health panel loaded, set it to ui
        if(RobotControlBdrMonitorImpl.healthPanel.get() != null) {
            setHealthPanel();
        }
    }
    
    private void setHealthPanel() {
        // add health panel
        if(RobotControlBdrMonitorImpl.healthPanel.get() != null) {
            RobotHealth health = (RobotHealth) RobotControlBdrMonitorImpl.healthPanel.get();
            health.jToolBar1.setVisible(false);
            statusPanel.add(RobotControlBdrMonitorImpl.healthPanel.get(), java.awt.BorderLayout.CENTER);
        }
    }
    
    @Override
    public void dispose() {
        Plugin.teamcommManager.removeListener(this);
        Plugin.teamCommUDPReceiver.disconnect(this, port);
    }

    @Override
    public void componentResized(ComponentEvent e) {
        //
        int width = getSize().width;
        int height = getSize().height;
        // reduce top/bottom and subtract the gap between 2 panels
        robotPanelHeight = (height - 2 * robotPanelGap) / 2 - robotPanelGap;
        robotPanelWidth = robotPanelHeight * 2 / 3;
        // determine the total width of the robot panel
        int robotPanelTotalWidth = robotPanelGap * 3 + 2 * robotPanelWidth;
        
        Dimension robotPanelSize = new Dimension(robotPanelTotalWidth, height);
        robotPanel.setPreferredSize(robotPanelSize);
        
        Dimension controlWrapperSize = new Dimension(width - robotPanelTotalWidth, height);
        controlWrapper.setPreferredSize(controlWrapperSize);
        
        updateRoboPanel();

        revalidate();
        repaint();
    }

    @Override
    public void componentMoved(ComponentEvent e) { /* ignore */ }

    @Override
    public void componentShown(ComponentEvent e) { /* ignore */ }

    @Override
    public void componentHidden(ComponentEvent e) { /* ignore */ }
    
    @Override
    public void connected(ConnectionStatusEvent event) {
        updateStatus();
    }

    @Override
    public void actionPerformed(ActionEvent e) {
        // remove robots & update panel, if the last message is older than 'maxTimeRobotIsDead'
        if(robotsMap.entrySet().removeIf((t) -> { return t.getValue().getLastTimestamp() + maxTimeRobotIsDead < System.currentTimeMillis(); })) {
            updateRoboPanel();
        }
    }

    @Override
    public void disconnected(ConnectionStatusEvent event) {
        setButtons(false);
        robotsMap.forEach((t, u) -> {
            u.setEnableConnectButton(true);
        });
        layerNotConnected.setVisible(true);
    }

    private void setButtons(boolean flag) {
        bt_autonomois.setEnabled(flag);
        bt_stop.setEnabled(flag);
        bt_wartung.setEnabled(flag);
    }

    private void updateStatus() {
        Command cmd = new Command("Cognition:representation:get").addArg("BDRControlCommand");
        Plugin.commandExecutor.executeCommand(new StatusUpdater(), cmd);
        // activate health status panel
        ((RobotHealth) RobotControlBdrMonitorImpl.healthPanel.get()).btReceiveDrawings.doClick();
        // disable button of all 'not connected' robots
        String connectedIp = Plugin.rc.getMessageServer().getAddress().getHostName();
        robotsMap.forEach((t, u) -> {
            if(!u.getAddress().equals(connectedIp)) {
                u.setEnableConnectButton(false);
            }
        });
        layerNotConnected.setVisible(false);
    }
    
    /**
     * This method is called from within the constructor to initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is always
     * regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        robotPanel = new javax.swing.JPanel();
        controlWrapper = new javax.swing.JPanel();
        layerNotConnected = new javax.swing.JLayeredPane();
        jPanel1 = new javax.swing.JPanel();
        jLabel1 = new javax.swing.JLabel();
        layerControls = new javax.swing.JLayeredPane();
        statusPanel = new javax.swing.JPanel();
        controlPanel = new javax.swing.JPanel();
        bt_stop = new javax.swing.JToggleButton();
        bt_autonomois = new javax.swing.JToggleButton();
        bt_wartung = new javax.swing.JToggleButton();

        setLayout(new javax.swing.BoxLayout(this, javax.swing.BoxLayout.LINE_AXIS));

        robotPanel.setBackground(new java.awt.Color(255, 255, 255));
        robotPanel.setMinimumSize(new java.awt.Dimension(50, 5));
        robotPanel.setName(""); // NOI18N
        robotPanel.setPreferredSize(new java.awt.Dimension(50, 50));
        robotPanel.setLayout(new java.awt.GridLayout(2, 2, robotPanelGap, robotPanelGap));

        robotPanel.setLayout(new de.naoth.rc.components.WrapLayout(java.awt.FlowLayout.LEFT));

        add(robotPanel);

        controlWrapper.setAlignmentX(0.5F);
        controlWrapper.setLayout(new javax.swing.OverlayLayout(controlWrapper));

        layerNotConnected.setBackground(new java.awt.Color(255, 0, 0));
        layerNotConnected.setForeground(new java.awt.Color(51, 255, 51));
        layerNotConnected.setLayout(new java.awt.BorderLayout());

        jPanel1.setBackground(new Color(0,0,0,125));
        jPanel1.setLayout(new java.awt.BorderLayout());

        jLabel1.setFont(new java.awt.Font("Dialog", 1, 48)); // NOI18N
        jLabel1.setForeground(new java.awt.Color(255, 255, 255));
        jLabel1.setHorizontalAlignment(javax.swing.SwingConstants.CENTER);
        jLabel1.setText("Not Connected ...");
        jLabel1.setAlignmentX(0.5F);
        jLabel1.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        jPanel1.add(jLabel1, java.awt.BorderLayout.CENTER);

        layerNotConnected.add(jPanel1, java.awt.BorderLayout.CENTER);

        controlWrapper.add(layerNotConnected);

        layerControls.setLayout(new javax.swing.BoxLayout(layerControls, javax.swing.BoxLayout.LINE_AXIS));

        statusPanel.setLayout(new java.awt.BorderLayout());
        layerControls.add(statusPanel);

        controlPanel.setMinimumSize(new java.awt.Dimension(200, 72));
        controlPanel.setPreferredSize(new java.awt.Dimension(200, 72));
        controlPanel.setLayout(new java.awt.GridLayout(3, 0));

        bt_stop.setText("STOP");
        bt_stop.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                bt_stopActionPerformed(evt);
            }
        });
        controlPanel.add(bt_stop);

        bt_autonomois.setText("AUTONOMES SPIEL");
        bt_autonomois.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                bt_autonomoisActionPerformed(evt);
            }
        });
        controlPanel.add(bt_autonomois);

        bt_wartung.setText("WARTUNG");
        bt_wartung.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                bt_wartungActionPerformed(evt);
            }
        });
        controlPanel.add(bt_wartung);

        layerControls.add(controlPanel);

        controlWrapper.add(layerControls);

        add(controlWrapper);
    }// </editor-fold>//GEN-END:initComponents

    private void bt_stopActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_bt_stopActionPerformed
        BDRControlCommand.Builder cmd = BDRControlCommand.newBuilder();
        cmd.setBehaviorMode(BDRBehaviorMode.DO_NOTHING);
        sendBDRCommand(cmd.build());
    }//GEN-LAST:event_bt_stopActionPerformed

    private void bt_autonomoisActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_bt_autonomoisActionPerformed
        BDRControlCommand.Builder cmd = BDRControlCommand.newBuilder();
        cmd.setBehaviorMode(BDRBehaviorMode.AUTONOMOUS_PLAY);
        sendBDRCommand(cmd.build());
    }//GEN-LAST:event_bt_autonomoisActionPerformed

    private void bt_wartungActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_bt_wartungActionPerformed
        BDRControlCommand.Builder cmd = BDRControlCommand.newBuilder();
        cmd.setBehaviorMode(BDRBehaviorMode.WARTUNG);
        sendBDRCommand(cmd.build());
    }//GEN-LAST:event_bt_wartungActionPerformed

    class StatusUpdater implements ObjectListener<byte[]>
    {
        @Override
        public void newObjectReceived(byte[] object) {
            try {
                BDRControlCommand command = BDRControlCommand.parseFrom(object);
                
                if(command.hasBehaviorMode()) {
                    setButtons(true);
                    
                    bt_autonomois.setSelected(command.getBehaviorMode() == BDRBehaviorMode.AUTONOMOUS_PLAY);
                    bt_stop.setSelected(command.getBehaviorMode() == BDRBehaviorMode.DO_NOTHING);
                    bt_wartung.setSelected(command.getBehaviorMode() == BDRBehaviorMode.WARTUNG);
                }
                
            } catch (InvalidProtocolBufferException ex) {
                Logger.getLogger(TeamCommViewer.class.getName()).log(Level.SEVERE, null, ex);
            }
        }

        @Override
        public void errorOccured(String cause) {
            System.err.println(cause);
        }
    }
    
    class RemoteCommandResultHandler implements ObjectListener<byte[]> {

        @Override
        public void newObjectReceived(byte[] object) {
            if (!new String(object).isEmpty()) {
                System.out.println(new String(object));
            }
        }

        @Override
        public void errorOccured(String cause) {
            System.out.println(cause);
        }
    }
    
    private void sendBDRCommand(BDRControlCommand command) {
        
        Command cmd = new Command("Cognition:representation:set").addArg("BDRControlCommand", command.toByteArray());
        Plugin.commandExecutor.executeCommand(new ObjectListener<byte[]>() {
            @Override
            public void newObjectReceived(byte[] object) {
                System.out.println(new String(object));
                updateStatus();
            }

            @Override
            public void errorOccured(String cause) {
                System.err.println(cause);
            }
        }, cmd);
        
        bt_autonomois.setEnabled(false);
        bt_stop.setEnabled(false);
        bt_wartung.setEnabled(false);
    }

    private void updateRoboPanel() {
        this.robotPanel.removeAll();
        
        for (RobotPanel robot : robotsMap.values()) {
            robot.setChestColor(robot.getMessage().getTeamColor());
            robot.setPreferredSize(new Dimension(robotPanelWidth, robotPanelHeight));
            robot.setMinimumSize(new Dimension(robotPanelWidth, robotPanelHeight));
            robotPanel.add(robot);
        }
        createDummies();
        this.robotPanel.repaint();
    }
    
    private void createDummies() {
        if(robotPanelWidth <= 0 || robotPanelHeight <= 0) { return; }
        for(int i = robotPanel.getComponentCount(); i<4; i++) {
            RobotPanel d = new RobotPanel("0.0.0.0", new SPLMessage());
            d.setPreferredSize(new Dimension(robotPanelWidth, robotPanelHeight));
            d.setMinimumSize(new Dimension(robotPanelWidth, robotPanelHeight));
            d.setEnabled(false);
            robotPanel.add(d);
        }
    }

    @Override
    public void newTeamCommMessages(List<TeamCommMessage> messages) {
        if (!messages.isEmpty()) {
            messages.forEach((m) -> {
                if(m.message.playerNum > 0) {
                    if (!robotsMap.containsKey(m.address)) {
                        robotsMap.put(m.address, new RobotPanel(Plugin.rc.getMessageServer(), m.address, m.message));
                        updateRoboPanel();
                    } else {
                        robotsMap.get(m.address).setStatus(System.currentTimeMillis(), m.message);
                    }
                } else {
                    /* TODO: do something with 'coach' messages */
                }
            });
        }
    }


    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JToggleButton bt_autonomois;
    private javax.swing.JToggleButton bt_stop;
    private javax.swing.JToggleButton bt_wartung;
    private javax.swing.JPanel controlPanel;
    private javax.swing.JPanel controlWrapper;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JLayeredPane layerControls;
    private javax.swing.JLayeredPane layerNotConnected;
    private javax.swing.JPanel robotPanel;
    private javax.swing.JPanel statusPanel;
    // End of variables declaration//GEN-END:variables
}
