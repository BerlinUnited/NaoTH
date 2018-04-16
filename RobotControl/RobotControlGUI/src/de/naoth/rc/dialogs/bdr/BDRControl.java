package de.naoth.rc.dialogs.bdr;

import com.google.protobuf.InvalidProtocolBufferException;
import de.naoth.rc.RobotControl;
import de.naoth.rc.components.RemoteRobotPanel;
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
import de.naoth.rc.dialogs.TeamCommViewer;
import de.naoth.rc.messages.BDRMessages.BDRControlCommand;
import de.naoth.rc.messages.BDRMessages.BDRBehaviorMode;
import de.naoth.rc.server.Command;
import de.naoth.rc.server.ConnectionStatusEvent;
import de.naoth.rc.server.ConnectionStatusListener;
import java.awt.Color;
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
        public static RobotControl parent;
        @InjectPlugin
        public static SwingCommandExecutor commandExecutor;
        @InjectPlugin
        public static TeamCommManager teamcommManager;
        @InjectPlugin
        public static TeamCommUDPReceiver teamCommUDPReceiver;
    }//end Plugin

    private final int port = 10004;
    
    private final HashMap<String, RemoteRobotPanel> robotsMap = new HashMap<>();
    
    public BDRControl() 
    {
        initComponents();

        // dummy robot for tests
        //robotsMap.put("10.0.4.77", new RemoteRobotPanel(Plugin.parent.getMessageServer(), "10.0.4.77", new SPLMessage()));
        //updateRoboPanel();
        
        Plugin.teamcommManager.addListener(this);
        Plugin.teamCommUDPReceiver.connect(this, port);
        
        Plugin.parent.getMessageServer().addConnectionStatusListener(new ConnectionStatusListener() {
            @Override
            public void connected(ConnectionStatusEvent event) {
                //setButtons(true);
                updateStatus();
            }

            @Override
            public void disconnected(ConnectionStatusEvent event) {
                setButtons(false);
            }
        });
        
        setButtons(false);
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

        jScrollPane2 = new javax.swing.JScrollPane();
        robotPanel = new javax.swing.JPanel();
        controlPanel = new javax.swing.JPanel();
        bt_stop = new javax.swing.JToggleButton();
        bt_autonomois = new javax.swing.JToggleButton();
        bt_wartung = new javax.swing.JToggleButton();

        robotPanel.setBackground(new java.awt.Color(255, 255, 255));
        robotPanel.setLayout(new java.awt.GridLayout(2, 2, 5, 5));

        robotPanel.setLayout(new de.naoth.rc.components.WrapLayout(java.awt.FlowLayout.LEFT));

        jScrollPane2.setViewportView(robotPanel);

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

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addComponent(jScrollPane2, javax.swing.GroupLayout.DEFAULT_SIZE, 426, Short.MAX_VALUE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(controlPanel, javax.swing.GroupLayout.PREFERRED_SIZE, 226, javax.swing.GroupLayout.PREFERRED_SIZE))
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jScrollPane2)
            .addComponent(controlPanel, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, 329, Short.MAX_VALUE)
        );
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

        for (RemoteRobotPanel robot : robotsMap.values()) {
            robotPanel.add(robot);
        }
        this.robotPanel.repaint();
    }

    @Override
    public void newTeamCommMessages(List<TeamCommMessage> messages) {
        if (!messages.isEmpty()) {
            messages.forEach((m) -> {
                if (!robotsMap.containsKey(m.address)) {
                    RemoteRobotPanel p = new RemoteRobotPanel(Plugin.parent.getMessageServer(), m.address, m.message);
                    p.setChestColor(Color.BLUE);
                    robotsMap.put(m.address, p);
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
    private javax.swing.JScrollPane jScrollPane2;
    private javax.swing.JPanel robotPanel;
    // End of variables declaration//GEN-END:variables
}