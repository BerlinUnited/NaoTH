/*
 * FieldViewer.java
 *
 * Created on 1. Mai 2008, 00:02
 */
package de.naoth.rc.dialogs.bdr;

import de.naoth.rc.core.dialog.AbstractDialog;
import de.naoth.rc.core.dialog.DialogPlugin;
import de.naoth.rc.RobotControl;
import de.naoth.rc.components.teamcomm.TeamCommListener;
import de.naoth.rc.components.teamcomm.TeamCommManager;
import de.naoth.rc.components.teamcomm.TeamCommMessage;
import de.naoth.rc.components.teamcomm.TeamCommUDPReceiver;
import de.naoth.rc.core.dialog.RCDialog;
import de.naoth.rc.dataformats.SPLMessage;
import de.naoth.rc.drawings.DrawingCollection;
import de.naoth.rc.drawings.FieldDrawingBDR;
import de.naoth.rc.messages.BDRMessages;
import de.naoth.rc.messages.TeamMessageOuterClass;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ComponentEvent;
import java.awt.event.ComponentListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.util.HashMap;
import java.util.List;
import java.util.TreeMap;
import java.util.concurrent.TimeUnit;
import javax.swing.Timer;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 *
 * @author  Heinrich Mellmann
 */
public class BDRMonitor extends AbstractDialog implements ActionListener, TeamCommListener, ComponentListener
{
    @RCDialog(category = RCDialog.Category.BDR, name = "Monitor")
    @PluginImplementation
    public static class Plugin extends DialogPlugin<BDRMonitor> {

        @InjectPlugin
        public static RobotControl parent;
        @InjectPlugin
        public static TeamCommManager teamcommManager;
        @InjectPlugin
        public static TeamCommUDPReceiver teamCommUDPReceiver;
    }//end Plugin
    
    private final int port = 10004;
    private final int maxMessageTime = 1000;
    private final int maxTimeRobotIsDead = 2000; //ms
    
    private final int robotPanelGap = 5;
    int robotPanelHeight = 300;
    int robotPanelWidth = 200;
    /**
     * The robot-TeamCommProvider for our own team.
     */
    //private final RobotTeamCommListener teamcommListener = new RobotTeamCommListener(true);
    
    private final TreeMap<String, RobotPanel> robots = new TreeMap<>();
    private final HashMap<String, RobotMessage> robotsMsg = new HashMap<>();
    
    private final Timer drawingTimer;
    
    public BDRMonitor() {
        initComponents();

        this.fieldCanvas.setBackgroundDrawing(new FieldDrawingBDR());
        this.fieldCanvas.setToolTipText("");
        this.fieldCanvas.setFitToViewport(true);
        this.fieldCanvas.setAntializing(true);

        // intialize the field
        clearField();
        this.fieldCanvas.repaint();

        this.fieldCanvas.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent e) {
                if (e.getClickCount() == 2 && !e.isConsumed()) {
                    e.consume();
                    fieldCanvas.fitToViewport();
//                    fieldCanvas.setOffsetY(580);
//                    fieldCanvas.repaint();
                }
            }
        });
        
        addComponentListener(this);
        
        // schedules canvas drawing at a fixed rate, should prevent "flickering"
        this.drawingTimer = new Timer(300, this);
        this.drawingTimer.start();

        //connect();
        Plugin.teamCommUDPReceiver.connect(this, port);
        Plugin.teamcommManager.addListener(this);
        
        // set the font for the whole scoreboard
        setLabelFont(new java.awt.Font("Bitstream Vera Sans Mono", 1, 30));
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
        
        Dimension drawingPanelSize = new Dimension(width - robotPanelTotalWidth, height);
        drawingPanel.setPreferredSize(drawingPanelSize);
        
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
    
    /*
    private void connect() {
        try {
            teamcommListener.connect(port);
            // listen to TeamCommMessages
            Plugin.teamcommManager.addListener(this);
        } catch (IOException | InterruptedException ex) {
            Logger.getLogger(BDRMonitor.class.getName()).log(Level.SEVERE, null, ex);
            // TODO: hint dialog?!
        }
    }

    private void disconnect() {
        try {
            teamcommListener.disconnect();
            // listen to TeamCommMessages
            Plugin.teamcommManager.removeListener(this);
        } catch (IOException | InterruptedException ex) {
            Logger.getLogger(BDRMonitor.class.getName()).log(Level.SEVERE, null, ex);
            // TODO: hint dialog?!
        }
    }
    */
    
    @Override
    public void dispose() {
        // remove all the registered listeners
        //disconnect();
        Plugin.teamCommUDPReceiver.disconnect(this, port);
        Plugin.teamcommManager.removeListener(this);
        if (drawingTimer.isRunning()) {
            drawingTimer.stop();
        }
    }

    private void clearField() {
        fieldCanvas.getDrawingList().clear();
    }

    @Override
    public void actionPerformed(ActionEvent e) {
        // remove robots & update panel, if the last message is older than 'maxTimeRobotIsDead'
        if(robots.entrySet().removeIf((t) -> { return t.getValue().getLastTimestamp() + maxTimeRobotIsDead < System.currentTimeMillis(); })) {
            updateRoboPanel();
        }
        
        // clear old field drawings
        clearField();

        // add buffered drawings to field drawing list
        DrawingCollection drawings = new DrawingCollection();
        robots.forEach((ip, m) -> {
            TeamMessageOuterClass.BUUserTeamMessage umsg = m.getMessage().user;

            // 
            boolean bdrRobotIsLocalized = 
                umsg.hasBdrPlayerState() && 
                umsg.getBdrPlayerState().hasLocalizedOnField() && 
                umsg.getBdrPlayerState().getLocalizedOnField();
            
            // color
            Color c = Color.lightGray; 
            if(bdrRobotIsLocalized) {
                c = m.getChestColor();
            }
            
            // only draw robot on field if he's localized
            if( bdrRobotIsLocalized || umsg.getRobotState() == TeamMessageOuterClass.RobotState.playing ) {
                m.getMessage().draw(drawings, c, false);
            }
        });
        fieldCanvas.getDrawingList().add(drawings);

        // re-draw field
        fieldCanvas.repaint();
        
        // collect the robot messages and show them
        String msg = "<html>";
        for (RobotMessage u : robotsMsg.values()) {
            if(!u.get().isEmpty() && u.time() + maxMessageTime > System.currentTimeMillis()) {
                msg += "Robot #" + u.number + ": " + u.get()+ "<br>";
            }
        }
        msg += "</html>";
        if(msg.length() > 13) {
            lblMessage.setText(msg);
            lblMessage.setVisible(true);
        } else {
            lblMessage.setVisible(false);
        }
    }

    /**
     * Waits for new TeamCommMessages and writes them to the "message-buffer".
     */
    @Override
    public void newTeamCommMessages(List<TeamCommMessage> messages) {
        if (!messages.isEmpty()) {
            messages.forEach((m) -> {
                // skip 'invalid' players
                if(m.message.playerNum > 0) {
//                System.out.println(m.address + ": " + m.message.user.getIsCharging());
                    if (!robots.containsKey(m.address)) {
                        robots.put(m.address, new RobotPanel(m.address, m.message));
                        updateRoboPanel();
                    } else {
                        robots.get(m.address).setStatus(System.currentTimeMillis(), m.message);
                    }
                    
                    if(m.message.user.hasMessage()) {
                        if(!robotsMsg.containsKey(m.address)) {
                            robotsMsg.put(m.address, new RobotMessage(m.message.playerNum));
                        }
                        robotsMsg.get(m.address).set(m.message.user.getMessage());
                    }
                } else {
                    // show timestamp & goals from 'coach'
                    if(m.message.user.hasBdrPlayerState()) {
                        if(m.message.user.getBdrPlayerState().hasTimePlaying()) {
                            long millis = (long) m.message.user.getBdrPlayerState().getTimePlaying();
                            lblTime.setText(String.format("%02d:%02d", 
                                //TimeUnit.MILLISECONDS.toHours(millis),
                                TimeUnit.MILLISECONDS.toMinutes(millis) - TimeUnit.HOURS.toMinutes(TimeUnit.MILLISECONDS.toHours(millis)),
                                TimeUnit.MILLISECONDS.toSeconds(millis) - TimeUnit.MINUTES.toSeconds(TimeUnit.MILLISECONDS.toMinutes(millis)))
                            );
                        }
                        if(m.message.user.getBdrPlayerState().hasGoalsLeft() && m.message.user.getBdrPlayerState().hasGoalsRight()) {
                            lblScore.setText(m.message.user.getBdrPlayerState().getGoalsLeft() + ":" + m.message.user.getBdrPlayerState().getGoalsRight());
                        }
                    }
                }
            });
        }
    }
    
    private void rotate(boolean left) {
        this.fieldCanvas.setRotation(this.fieldCanvas.getRotation() + Math.PI*0.5 * (left?1:-1));
        // TODO: should this be inside the DynamicCanvasPanel?
        if(this.fieldCanvas.isFitToViewport()) {
            this.fieldCanvas.fitToViewport();
        }
        this.fieldCanvas.repaint();
    }
    
    private synchronized void updateRoboPanel() {
        this.robotPanel.removeAll();
        
        robots.forEach((t, u) -> {
            u.setPreferredSize(new Dimension(robotPanelWidth, robotPanelHeight));
            u.setMinimumSize(new Dimension(robotPanelWidth, robotPanelHeight));
            u.setHideConnectButton(true);
            robotPanel.add(u);
        });
        createDummies();
        this.robotPanel.revalidate();
        this.robotPanel.repaint();
    }
    
    private void createDummies() {
        if(robotPanelWidth <= 0 || robotPanelHeight <= 0) { return; }
        for(int i = robotPanel.getComponentCount(); i<4; i++) {
            RobotPanel d = new RobotPanel("0.0.0.0", new SPLMessage());
            d.setPreferredSize(new Dimension(robotPanelWidth, robotPanelHeight));
            d.setMinimumSize(new Dimension(robotPanelWidth, robotPanelHeight));
            d.setEnabled(false);
            d.setVisible(true);
            robotPanel.add(d);
        }
    }
    
    private void setLabelFont(Font f) {
        lblTime.setFont(f);
        lblDiv.setFont(f);
        lblScore.setFont(f);
    }

  /** This method is called from within the constructor to
   * initialize the form.
   * WARNING: Do NOT modify this code. The content of this method is
   * always regenerated by the Form Editor.
   */
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        jPopupMenu = new javax.swing.JPopupMenu();
        miClear = new javax.swing.JMenuItem();
        miRotateLeft = new javax.swing.JMenuItem();
        miRotateRight = new javax.swing.JMenuItem();
        coordsPopup = new javax.swing.JDialog();
        robotPanel = new javax.swing.JPanel();
        drawingPanel = new javax.swing.JPanel();
        fieldCanvas = new de.naoth.rc.components.DynamicCanvasPanel();
        jLayeredPane1 = new javax.swing.JLayeredPane();
        jPanel2 = new javax.swing.JPanel();
        lblPanel = new javax.swing.JPanel();
        lblTime = new javax.swing.JLabel();
        lblDiv = new javax.swing.JLabel();
        lblScore = new javax.swing.JLabel();
        jLayeredPane2 = new javax.swing.JLayeredPane();
        jPanel1 = new javax.swing.JPanel();
        lblMessage = new javax.swing.JLabel();

        miClear.setText("Clear");
        miClear.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                miClearActionPerformed(evt);
            }
        });
        jPopupMenu.add(miClear);

        miRotateLeft.setText("Rotate left");
        miRotateLeft.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                miRotateLeftActionPerformed(evt);
            }
        });
        jPopupMenu.add(miRotateLeft);

        miRotateRight.setText("Rotate right");
        miRotateRight.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                miRotateRightActionPerformed(evt);
            }
        });
        jPopupMenu.add(miRotateRight);

        javax.swing.GroupLayout coordsPopupLayout = new javax.swing.GroupLayout(coordsPopup.getContentPane());
        coordsPopup.getContentPane().setLayout(coordsPopupLayout);
        coordsPopupLayout.setHorizontalGroup(
            coordsPopupLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 114, Short.MAX_VALUE)
        );
        coordsPopupLayout.setVerticalGroup(
            coordsPopupLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 38, Short.MAX_VALUE)
        );

        setLayout(new javax.swing.BoxLayout(this, javax.swing.BoxLayout.LINE_AXIS));

        robotPanel.setBackground(new java.awt.Color(255, 255, 255));
        robotPanel.setLayout(new java.awt.GridLayout(2, 2, robotPanelGap, robotPanelGap));
        add(robotPanel);

        drawingPanel.setBackground(new java.awt.Color(247, 247, 247));
        drawingPanel.setLayout(new java.awt.BorderLayout());

        fieldCanvas.setBackground(new java.awt.Color(247, 247, 247));
        fieldCanvas.setComponentPopupMenu(jPopupMenu);
        fieldCanvas.setOffsetX(350.0);
        fieldCanvas.setOffsetY(200.0);
        fieldCanvas.setScale(0.07);
        fieldCanvas.setLayout(new javax.swing.OverlayLayout(fieldCanvas));

        jLayeredPane1.setLayout(new java.awt.BorderLayout());

        jPanel2.setOpaque(false);

        lblPanel.setBorder(javax.swing.BorderFactory.createEmptyBorder(10, 10, 10, 10));
        lblPanel.setOpaque(false);
        lblPanel.setLayout(new javax.swing.BoxLayout(lblPanel, javax.swing.BoxLayout.LINE_AXIS));

        lblTime.setText("00:00");
        lblPanel.add(lblTime);

        lblDiv.setText(" / ");
        lblPanel.add(lblDiv);

        lblScore.setFont(new java.awt.Font("Bitstream Vera Sans Mono", 0, 11)); // NOI18N
        lblScore.setText("0:0");
        lblPanel.add(lblScore);

        jPanel2.add(lblPanel);

        jLayeredPane1.add(jPanel2, java.awt.BorderLayout.NORTH);

        fieldCanvas.add(jLayeredPane1);

        jLayeredPane2.setLayout(new java.awt.BorderLayout());

        jPanel1.setBorder(javax.swing.BorderFactory.createEmptyBorder(0, 0, 50, 0));
        jPanel1.setOpaque(false);

        lblMessage.setFont(new java.awt.Font("Dialog", 0, 18)); // NOI18N
        lblMessage.setHorizontalAlignment(javax.swing.SwingConstants.CENTER);
        lblMessage.setText("Message from a robot ...");
        lblMessage.setBorder(javax.swing.BorderFactory.createEmptyBorder(25, 25, 25, 25));
        lblMessage.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        lblMessage.setOpaque(true);
        jPanel1.add(lblMessage);

        jLayeredPane2.add(jPanel1, java.awt.BorderLayout.PAGE_END);

        fieldCanvas.add(jLayeredPane2);

        drawingPanel.add(fieldCanvas, java.awt.BorderLayout.CENTER);

        add(drawingPanel);
    }// </editor-fold>//GEN-END:initComponents

private void miClearActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_miClearActionPerformed
    clearField();
}//GEN-LAST:event_miClearActionPerformed

    private void miRotateLeftActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_miRotateLeftActionPerformed
        rotate(true);
    }//GEN-LAST:event_miRotateLeftActionPerformed

    private void miRotateRightActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_miRotateRightActionPerformed
        rotate(false);
    }//GEN-LAST:event_miRotateRightActionPerformed

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JDialog coordsPopup;
    private javax.swing.JPanel drawingPanel;
    private de.naoth.rc.components.DynamicCanvasPanel fieldCanvas;
    private javax.swing.JLayeredPane jLayeredPane1;
    private javax.swing.JLayeredPane jLayeredPane2;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JPanel jPanel2;
    private javax.swing.JPopupMenu jPopupMenu;
    private javax.swing.JLabel lblDiv;
    private javax.swing.JLabel lblMessage;
    private javax.swing.JPanel lblPanel;
    private javax.swing.JLabel lblScore;
    private javax.swing.JLabel lblTime;
    private javax.swing.JMenuItem miClear;
    private javax.swing.JMenuItem miRotateLeft;
    private javax.swing.JMenuItem miRotateRight;
    private javax.swing.JPanel robotPanel;
    // End of variables declaration//GEN-END:variables

    class RobotMessage {
        public final int number;
        private String msg = "";
        private long time = System.currentTimeMillis();

        public RobotMessage(int n) {
            number = n;
        }
        
        public void set(String m) {
            this.msg = m;
            time = System.currentTimeMillis();
        }
        
        public String get() { return msg; }
        public long time() { return time; }
    }
}
