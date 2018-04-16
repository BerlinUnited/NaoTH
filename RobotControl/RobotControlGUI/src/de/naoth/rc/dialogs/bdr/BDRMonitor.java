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
import de.naoth.rc.drawings.DrawingCollection;
import de.naoth.rc.drawings.FieldDrawingBDR;
import java.awt.Color;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.util.List;
import java.util.TreeMap;
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
                jSplitPane1.setDividerLocation(e.getComponent().getWidth()*2/3);
                
            }
        });

        // schedules canvas drawing at a fixed rate, should prevent "flickering"
        this.drawingTimer = new Timer(300, this);
        this.drawingTimer.start();

        //connect();
        Plugin.teamCommUDPReceiver.connect(this, port);
        Plugin.teamcommManager.addListener(this);
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
                    robots.put(m.address, p);
                    // the status panel should be ordered - remove all and readded it in order
                    statusPanel.removeAll();
                    robots.forEach((t, u) -> {
                        statusPanel.add(u);
                    });
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
        jSplitPane1 = new javax.swing.JSplitPane();
        fieldCanvas = new de.naoth.rc.components.DynamicCanvasPanel();
        statusPanel = new javax.swing.JPanel();

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

        jSplitPane1.setDividerLocation(337);

        fieldCanvas.setBackground(new java.awt.Color(247, 247, 247));
        fieldCanvas.setComponentPopupMenu(jPopupMenu);
        fieldCanvas.setOffsetX(350.0);
        fieldCanvas.setOffsetY(200.0);
        fieldCanvas.setScale(0.07);

        javax.swing.GroupLayout fieldCanvasLayout = new javax.swing.GroupLayout(fieldCanvas);
        fieldCanvas.setLayout(fieldCanvasLayout);
        fieldCanvasLayout.setHorizontalGroup(
            fieldCanvasLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 0, Short.MAX_VALUE)
        );
        fieldCanvasLayout.setVerticalGroup(
            fieldCanvasLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 386, Short.MAX_VALUE)
        );

        jSplitPane1.setLeftComponent(fieldCanvas);

        statusPanel.setLayout(new java.awt.GridLayout(2, 0));
        jSplitPane1.setRightComponent(statusPanel);

        drawingPanel.add(jSplitPane1, java.awt.BorderLayout.CENTER);

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
    private javax.swing.JSplitPane jSplitPane1;
    private javax.swing.JMenuItem miClear;
    private javax.swing.JMenuItem miRotateLeft;
    private javax.swing.JMenuItem miRotateRight;
    private javax.swing.JPanel statusPanel;
    // End of variables declaration//GEN-END:variables
}
