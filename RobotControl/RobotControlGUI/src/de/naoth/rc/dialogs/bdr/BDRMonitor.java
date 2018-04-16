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
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.util.List;
import java.util.TreeMap;
import javax.swing.ImageIcon;
import javax.swing.JPanel;
import javax.swing.Timer;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 *
 * @author  Heinrich Mellmann
 */
public class BDRMonitor extends AbstractDialog implements ActionListener, TeamCommListener
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
    
    /**
     * The robot-TeamCommProvider for our own team.
     */
    //private final RobotTeamCommListener teamcommListener = new RobotTeamCommListener(true);
    
    private final TreeMap<String, RobotPanel> robots = new TreeMap<>();
    
    private final Timer drawingTimer;
    
    public BDRMonitor() {
        initComponents();

        this.fieldCanvas.setBackgroundDrawing(new FieldDrawingBDR());
        this.fieldCanvas.setToolTipText("");
        this.fieldCanvas.setFitToViewport(true);

        // intialize the field
        clearField();
        //this.fieldCanvas.setAntializing(true);
        this.fieldCanvas.repaint();

        this.fieldCanvas.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent e) {
                if (e.getClickCount() == 2 && !e.isConsumed()) {
                    e.consume();
                    fieldCanvas.fitToViewport();
                }
            }
        });
        
        // set the 'correct' divider position
        this.addComponentListener(new ComponentAdapter() {
            @Override
            public void componentResized(ComponentEvent e) {
                splitpane.setDividerLocation(e.getComponent().getWidth()*1/3);
                
                createDummies();
                centerScoreboard();
            }
        });
        
        // schedules canvas drawing at a fixed rate, should prevent "flickering"
        this.drawingTimer = new Timer(300, this);
        this.drawingTimer.start();

        //connect();
        Plugin.teamCommUDPReceiver.connect(this, port);
        Plugin.teamcommManager.addListener(this);
        
        // set the font for the whole scoreboard
        setLabelFont(new java.awt.Font("Bitstream Vera Sans Mono", 1, 30));
    }
    
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
        // clear old field drawings
        clearField();

        // add buffered drawings to field drawing list
        DrawingCollection drawings = new DrawingCollection();
        robots.forEach((ip, m) -> {
            m.getMessage().draw(drawings, m.getChestColor(), false);
        });
        fieldCanvas.getDrawingList().add(drawings);

        // re-draw field
        fieldCanvas.repaint();
    }

    /**
     * Waits for new TeamCommMessages and writes them to the "message-buffer".
     */
    @Override
    public void newTeamCommMessages(List<TeamCommMessage> messages) {
        if (!messages.isEmpty()) {
            messages.forEach((m) -> {
                if (!robots.containsKey(m.address)) {
                    RobotPanel p = new RobotPanel(m.address, m.message);
                    p.setChestColor(Color.BLUE);
                    int displayHeight = statusPanel.getHeight() - 20;
                    p.setPreferredSize(new Dimension(displayHeight/3, displayHeight/2));
                    robots.put(m.address, p);
                    // the status panel should be ordered - remove all and readded it in order
                    statusPanel.removeAll();
                    robots.forEach((t, u) -> {
                        JPanel panel = new JPanel();
                        panel.add(u);
                        
                        statusPanel.add(panel);
                    });
                    createDummies();
                    statusPanel.repaint();
                } else {
                    robots.get(m.address).setStatus(m.timestamp, m.message);
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
    
    private void createDummies() {
        int displayHeight = statusPanel.getHeight() - 20;
        if(displayHeight < 0) { return; }
        for(int i = statusPanel.getComponentCount(); i<4; i++) {
            RobotPanel d = new RobotPanel("0.0.0.0", new SPLMessage());
            d.setPreferredSize(new Dimension(displayHeight/3, displayHeight/2));
            d.setEnabled(false);
            JPanel panel = new JPanel();
            panel.add(d);
            statusPanel.add(panel);
        }
    }
    
    private void setLabelFont(Font f) {
        lblTime.setFont(f);
        lblDiv.setFont(f);
        lblScore.setFont(f);
    }
    
    private void centerScoreboard() {
        // only if the field is "shown"
        if(fieldCanvas.getWidth() > 0) {
            System.out.println(fieldCanvas.getWidth() / 2);
            lblPanel.setLocation(0, 0);
            lblPanel.repaint();
        }
        System.out.println(">> " + fieldCanvas.getWidth());
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
        drawingPanel = new javax.swing.JPanel();
        splitpane = new javax.swing.JSplitPane();
        statusPanel = new javax.swing.JPanel();
        fieldCanvas = new de.naoth.rc.components.DynamicCanvasPanel();
        lblPanel = new javax.swing.JPanel();
        lblTime = new javax.swing.JLabel();
        lblDiv = new javax.swing.JLabel();
        lblScore = new javax.swing.JLabel();

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

        setLayout(new java.awt.BorderLayout());

        drawingPanel.setBackground(new java.awt.Color(247, 247, 247));
        drawingPanel.setLayout(new java.awt.BorderLayout());

        splitpane.setDividerLocation(337);
        splitpane.setDividerSize(1);

        statusPanel.setLayout(new java.awt.GridLayout(2, 2, 10, 10));
        splitpane.setLeftComponent(statusPanel);

        fieldCanvas.setBackground(new java.awt.Color(247, 247, 247));
        fieldCanvas.setComponentPopupMenu(jPopupMenu);
        fieldCanvas.setOffsetX(350.0);
        fieldCanvas.setOffsetY(200.0);
        fieldCanvas.setScale(0.07);

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

        javax.swing.GroupLayout fieldCanvasLayout = new javax.swing.GroupLayout(fieldCanvas);
        fieldCanvas.setLayout(fieldCanvasLayout);
        fieldCanvasLayout.setHorizontalGroup(
            fieldCanvasLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, fieldCanvasLayout.createSequentialGroup()
                .addContainerGap(400, Short.MAX_VALUE)
                .addComponent(lblPanel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap())
        );
        fieldCanvasLayout.setVerticalGroup(
            fieldCanvasLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(fieldCanvasLayout.createSequentialGroup()
                .addContainerGap()
                .addComponent(lblPanel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap(328, Short.MAX_VALUE))
        );

        splitpane.setRightComponent(fieldCanvas);

        drawingPanel.add(splitpane, java.awt.BorderLayout.CENTER);

        add(drawingPanel, java.awt.BorderLayout.CENTER);
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
    private javax.swing.JPopupMenu jPopupMenu;
    private javax.swing.JLabel lblDiv;
    private javax.swing.JPanel lblPanel;
    private javax.swing.JLabel lblScore;
    private javax.swing.JLabel lblTime;
    private javax.swing.JMenuItem miClear;
    private javax.swing.JMenuItem miRotateLeft;
    private javax.swing.JMenuItem miRotateRight;
    private javax.swing.JSplitPane splitpane;
    private javax.swing.JPanel statusPanel;
    // End of variables declaration//GEN-END:variables
}
