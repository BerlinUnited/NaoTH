/*
 * RobotStatus.java
 *
 * Created on 09.11.2010, 23:10:57
 */

package de.naoth.rc.dialogs.panels;

import de.naoth.rc.dataformats.SPLMessage;
import de.naoth.rc.dialogs.TeamCommViewer;
import de.naoth.rc.server.MessageServer;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author Heinrich Mellmann
 */
public class RobotStatus extends javax.swing.JPanel {

    private final MessageServer messageServer;
    private String ipAddress = "";

    /** Creates new form RobotStatus */
    public RobotStatus(MessageServer messageServer, String ipAddress) {
        initComponents();

        this.messageServer = messageServer;
        this.ipAddress = ipAddress;
        
        this.jlAddress.setText(this.ipAddress);
    }

    public void setStatus(int playerNumber, long timestamp, SPLMessage msg)
    {
      this.jlPlayerNumber.setText("PN: " + msg.playerNum);
      
      long currentTime = System.currentTimeMillis();
      double timeDelta = (double)(currentTime-timestamp);
      this.jlTimestamp.setText("" + timeDelta);
      
      this.jlTeamNumber.setText("TN: " + msg.team);
      
      if(msg.team==1) {
        this.jlTeamColor.setText("TEAM RED");
      } else if(msg.team==0) {
        this.jlTeamColor.setText("TEAM BLUE");
      } else {
        this.jlTeamColor.setText("TEAM " + msg.team);
      }

      this.jlFallenTime.setText("FT: " + msg.fallen);
      this.jlBallAge.setText("BA: " + msg.ballAge);
      
      this.connectButton.setEnabled(!this.messageServer.isConnected());
    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        jLabel5 = new javax.swing.JLabel();
        jPanel1 = new javax.swing.JPanel();
        jlPlayerNumber = new javax.swing.JLabel();
        jlTimestamp = new javax.swing.JLabel();
        jlTeamNumber = new javax.swing.JLabel();
        jlTeamColor = new javax.swing.JLabel();
        jlFallenTime = new javax.swing.JLabel();
        jlBallAge = new javax.swing.JLabel();
        jlAddress = new javax.swing.JLabel();
        connectButton = new javax.swing.JButton();

        jLabel5.setText("jLabel5");

        javax.swing.GroupLayout jPanel1Layout = new javax.swing.GroupLayout(jPanel1);
        jPanel1.setLayout(jPanel1Layout);
        jPanel1Layout.setHorizontalGroup(
            jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 100, Short.MAX_VALUE)
        );
        jPanel1Layout.setVerticalGroup(
            jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 100, Short.MAX_VALUE)
        );

        setBorder(new javax.swing.border.LineBorder(new java.awt.Color(0, 0, 0), 1, true));
        setMaximumSize(new java.awt.Dimension(150, 200));
        setMinimumSize(new java.awt.Dimension(150, 100));
        setPreferredSize(new java.awt.Dimension(150, 100));
        setLayout(new java.awt.GridLayout(4, 2));

        jlPlayerNumber.setText("PN");
        add(jlPlayerNumber);

        jlTimestamp.setText("DEAD");
        add(jlTimestamp);

        jlTeamNumber.setText("TN");
        add(jlTeamNumber);

        jlTeamColor.setText("NONE");
        add(jlTeamColor);

        jlFallenTime.setText("FT");
        add(jlFallenTime);

        jlBallAge.setText("BA");
        add(jlBallAge);

        jlAddress.setText("-");
        add(jlAddress);

        connectButton.setText("Connect");
        connectButton.setMaximumSize(new java.awt.Dimension(50, 23));
        connectButton.setMinimumSize(new java.awt.Dimension(50, 23));
        connectButton.setPreferredSize(new java.awt.Dimension(50, 23));
        connectButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                connectButtonActionPerformed(evt);
            }
        });
        add(connectButton);
    }// </editor-fold>//GEN-END:initComponents

    private void connectButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_connectButtonActionPerformed
      if(!this.messageServer.isConnected())
      {
        try
        {
          // TODO: fix port 5401
          this.messageServer.connect(this.ipAddress, 5401);
        }catch(IOException ex)
        {
          Logger.getLogger(TeamCommViewer.class.getName()).log(Level.SEVERE, "Coult not connect.", ex);
        }
      }
    }//GEN-LAST:event_connectButtonActionPerformed


    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JButton connectButton;
    private javax.swing.JLabel jLabel5;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JLabel jlAddress;
    private javax.swing.JLabel jlBallAge;
    private javax.swing.JLabel jlFallenTime;
    private javax.swing.JLabel jlPlayerNumber;
    private javax.swing.JLabel jlTeamColor;
    private javax.swing.JLabel jlTeamNumber;
    private javax.swing.JLabel jlTimestamp;
    // End of variables declaration//GEN-END:variables

}
