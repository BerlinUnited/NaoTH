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
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.util.HashMap;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 *
 * @author Heinrich Mellmann
 */
public class BDRControl extends AbstractDialog implements TeamCommListener {

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
    
    private final HashMap<String, RobotPanel> robotsMap = new HashMap<>();
    
    public BDRControl() 
    {
        initComponents();

        // dummy robot for tests
        //robotsMap.put("10.0.4.77", new RemoteRobotPanel(Plugin.parent.getMessageServer(), "10.0.4.77", new SPLMessage()));
        //updateRoboPanel();
        
        Plugin.teamcommManager.addListener(this);
        Plugin.teamCommUDPReceiver.connect(this, port);
        
        Plugin.rc.getMessageServer().addConnectionStatusListener(new ConnectionStatusListener() {
            @Override
            public void connected(ConnectionStatusEvent event) {
                //setButtons(true);
                updateStatus();
            }

            @Override
            public void disconnected(ConnectionStatusEvent event) {
                setButtons(false);
                robotsMap.forEach((t, u) -> {
                    u.setEnableConnectButton(true);
                });
            }
        });
        
        setButtons(false);
        
        System.out.println(RobotControlBdrMonitorImpl.healthPanel);
        if(RobotControlBdrMonitorImpl.healthPanel != null) {
            RobotHealth health = (RobotHealth) RobotControlBdrMonitorImpl.healthPanel;
            health.jToolBar1.setVisible(false);
            statusPanel.add(RobotControlBdrMonitorImpl.healthPanel, java.awt.BorderLayout.CENTER);
//            getContentPane().add(dialog.getPanel(), java.awt.BorderLayout.CENTER);
        }
        
        // set the 'correct' divider position
        this.addComponentListener(new ComponentAdapter() {
            @Override
            public void componentResized(ComponentEvent e) {
//                System.out.println(e.getComponent().getWidth() + "/" + e.getComponent().getHeight());
//                int robotPanelHeight = e.getComponent().getHeight() / 2;
                int robotPanelWidth = (robotPanel.getHeight() - 20) / 3;
                Dimension robotPanelSize = new Dimension((robotPanelWidth + 5) * 2 + 5, robotPanel.getHeight());
                robotPanel.setPreferredSize(robotPanelSize);
                robotPanel.setMinimumSize(robotPanelSize);
                robotPanel.repaint();
                
                controlPanel.setPreferredSize(new Dimension(0, controlPanel.getHeight()));
            }
        });
//        System.out.println(Plugin.getManager());
        
    }
    
    @Override
    public void dispose() {
        Plugin.teamcommManager.removeListener(this);
        Plugin.teamCommUDPReceiver.disconnect(this, port);
    }
    
    private void setButtons(boolean flag) {
        bt_autonomois.setEnabled(flag);
        bt_stop.setEnabled(flag);
        bt_wartung.setEnabled(flag);
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
        statusPanel = new javax.swing.JPanel();
        controlPanel = new javax.swing.JPanel();
        bt_stop = new javax.swing.JToggleButton();
        bt_autonomois = new javax.swing.JToggleButton();
        bt_wartung = new javax.swing.JToggleButton();

        setLayout(new javax.swing.BoxLayout(this, javax.swing.BoxLayout.LINE_AXIS));

        robotPanel.setBackground(new java.awt.Color(255, 255, 255));
        robotPanel.setLayout(new java.awt.GridLayout(2, 2, 5, 5));

        robotPanel.setLayout(new de.naoth.rc.components.WrapLayout(java.awt.FlowLayout.LEFT));

        add(robotPanel);

        statusPanel.setPreferredSize(new java.awt.Dimension(32767, 32767));
        statusPanel.setLayout(new java.awt.BorderLayout());
        add(statusPanel);

        controlPanel.setMinimumSize(new java.awt.Dimension(200, 72));
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

        add(controlPanel);
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
    
    private void updateStatus() {
        Command cmd = new Command("Cognition:representation:get").addArg("BDRControlCommand");
        Plugin.commandExecutor.executeCommand(new StatusUpdater(), cmd);
        // activate health status panel
        ((RobotHealth) RobotControlBdrMonitorImpl.healthPanel).btReceiveDrawings.doClick();
        // disable button of all 'not connected' robots
        String connectedIp = Plugin.rc.getMessageServer().getAddress().getHostName();
        robotsMap.forEach((t, u) -> {
            if(!u.getAddress().equals(connectedIp)) {
                u.setEnableConnectButton(false);
            }
        });
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
        int displayHeight = robotPanel.getHeight() - 20;
        System.out.println(displayHeight/3);
        
        for (RobotPanel robot : robotsMap.values()) {    
            robot.setChestColor(Color.BLUE);
            robot.setPreferredSize(new Dimension(displayHeight/3, displayHeight/2));
            robotPanel.add(robot);
        }
        createDummies();
        this.robotPanel.repaint();
    }
    
    private void createDummies() {
        int displayHeight = robotPanel.getHeight() - 20;
        if(displayHeight < 0) { return; }
        for(int i = robotPanel.getComponentCount(); i<4; i++) {
            RobotPanel d = new RobotPanel("0.0.0.0", new SPLMessage());
            d.setPreferredSize(new Dimension(displayHeight/3, displayHeight/2));
            d.setEnabled(false);
            robotPanel.add(d);
        }
    }

    @Override
    public void newTeamCommMessages(List<TeamCommMessage> messages) {
        if (!messages.isEmpty()) {
            messages.forEach((m) -> {
                if (!robotsMap.containsKey(m.address)) {
                    robotsMap.put(m.address, new RobotPanel(Plugin.rc.getMessageServer(), m.address, m.message));
                    updateRoboPanel();
                } else {
                    robotsMap.get(m.address).setStatus(m.timestamp, m.message);
                }
            });
        }
    }


    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JToggleButton bt_autonomois;
    private javax.swing.JToggleButton bt_stop;
    private javax.swing.JToggleButton bt_wartung;
    private javax.swing.JPanel controlPanel;
    private javax.swing.JPanel robotPanel;
    private javax.swing.JPanel statusPanel;
    // End of variables declaration//GEN-END:variables
}
