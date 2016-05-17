/*
 * RobotStatus.java
 *
 * Created on 09.11.2010, 23:10:57
 */

package de.naoth.rc.components;

import de.naoth.rc.dataformats.SPLMessage;
import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.image.BufferedImage;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;
import javax.imageio.ImageIO;

/**
 *
 * @author Heinrich Mellmann
 */
public class RemoteRobotPanel extends javax.swing.JPanel {

    private class RingBuffer extends ArrayList<Long> {
        private final int size;
        public RingBuffer(int size) {
            this.size = size;
        }
        
        @Override
        public boolean add(Long v) {
            boolean r = super.add(v);
            
            if(size() > size) {
                remove(0);
            }
            
            return r;
        }        
    }
    
    private final Color darkOrange = new Color(255, 130, 0);
    public final static long MAX_TIME_BEFORE_DEAD = 5000; //ms
    private final RingBuffer timestamps = new RingBuffer(5);
    private String ipAddress;
    private SPLMessage currentMesage;
    
    public RemoteRobotPanel(String ipAddress, SPLMessage msg) {
        initComponents();
        
        this.ipAddress = ipAddress;
        this.currentMesage = msg;
        this.jlAddress.setText(this.ipAddress);
        
        try {
            nao_body = ImageIO.read(getClass().getResource("/de/naoth/rc/res/nao_body.png"));
        
            Color fade_color = new Color(230,245,255,200);
            Graphics2D g2d = nao_body.createGraphics();
            g2d.setColor(fade_color);
            g2d.fillRect(0, 0, nao_body.getWidth(), nao_body.getHeight());
            
        } catch (IOException e) {
        }
    }
    
    public boolean isReadyToBind() {
        return this.btBind.isSelected();
    }
    
    public void resetReadyToBind() {
        this.btBind.setSelected(false);
    }
    
    public SPLMessage getMessage() {
        return currentMesage;
    }
    
    public void setStatus(long timestamp, SPLMessage msg)
    {
      this.currentMesage = msg;
      this.jlPlayerNumber.setText("" + msg.playerNum);
      
      long currentTime = System.currentTimeMillis();
      
      //this.jlTimestamp.setText("" + timeDelta);
      
      // don't add the timestamp if it did not change compared to the last time
      long lastSeen = Long.MIN_VALUE;
      if(!timestamps.isEmpty()) 
      {
        lastSeen = timestamps.get(timestamps.size()-1);
      }
      if(lastSeen < timestamp)
      {
        timestamps.add(timestamp);
        lastSeen = timestamp;
      }
      double msgPerSecond = calculateMsgPerSecond();
      if((currentTime - lastSeen) > MAX_TIME_BEFORE_DEAD || msgPerSecond <= 0.0)
      {
        this.jlTimestamp.setText("DEAD");
        this.jlTimestamp.setForeground(Color.red);
      }
      else
      {
        this.jlTimestamp.setText(String.format("%4.2f msg/s", msgPerSecond));
        this.jlTimestamp.setForeground(Color.black);
      }
      
      //this.jlFallenTime.setText(msg.fallen == 1 ? "FALLEN" : "NOT FALLEN");
      this.jlBallAge.setText("" + msg.ballAge + "s");
      
      if(msg.user != null)
      {
        temperatureBar.setValue((int)msg.user.getTemperature());
        temperatureBar.setString(String.format("TEMP %3.1f °C", msg.user.getTemperature()));
        
        batteryBar.setValue((int)(msg.user.getBatteryCharge()*100.0f));
        batteryBar.setString(String.format("LADUNG %3.1f%%", msg.user.getBatteryCharge()*100.0f));
        
        if(msg.user.getTemperature() >= 80.0f){
          temperatureBar.setBackground(Color.red);
        } else if(msg.user.getTemperature() >= 60.0f) {
          temperatureBar.setBackground(darkOrange);
        } else {
          temperatureBar.setBackground(Color.green);
        }
        
        if(msg.user.getBatteryCharge() <= 0.1f) {
          batteryBar.setBackground(Color.red);
        } else if(msg.user.getBatteryCharge() <= 0.3f) {
          batteryBar.setBackground(darkOrange);
        } else {
          batteryBar.setBackground(Color.green);
        }

        this.jlTeamNumber.setText("" + msg.teamNum);
      }
    }
    
    private BufferedImage nao_body;
    
    // this is called by the drawingPanel.paintComponent(...)
    // see: (NetBeans) Properties: "Custom Creation Code" for drawingPanel
    public void paintDrawingComponent(Graphics g)
    {
      Graphics2D g2d = (Graphics2D) g;
      
      g2d.drawImage(nao_body, null, 0, 0);
      
      g2d.setFont(new Font("TimesRoman", Font.BOLD, 100));
      g2d.drawString("" + currentMesage.playerNum, 100, 100);
      
      
    }//end paintComponent
    
    

    private double calculateMsgPerSecond()
    {
      long sumOfDiffs = 0;
      int numberOfEntries = 0;
      Iterator<Long> it = timestamps.iterator();
      if(it.hasNext())
      {
        long t1 = it.next();
        while(it.hasNext())
        {
          long t2 = it.next();
          long diff = t2-t1;
          if(diff > 0)
          {
            sumOfDiffs += (t2-t1);
            numberOfEntries++;
          }
          t1 = t2;
        }
      }
      if(numberOfEntries > 0) {
        return 1000.0 / ((double) sumOfDiffs / (double) numberOfEntries);
      } else {
        return 0.0;
      }
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
        btBind = new javax.swing.JToggleButton();
        temperatureBar = new javax.swing.JProgressBar();
        batteryBar = new javax.swing.JProgressBar();
        drawingPanel = new javax.swing.JPanel()
        {
            public synchronized void paintComponent(Graphics g)
            {
                super.paintComponent(g);
                // call the parent painter method
                paintDrawingComponent(g); 
            }
        };
        jlTeamNumber = new javax.swing.JLabel();
        jlPlayerNumber = new javax.swing.JLabel();
        jlBallAge = new javax.swing.JLabel();
        jlAddress = new javax.swing.JLabel();
        jlTimestamp = new javax.swing.JLabel();

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
        setMaximumSize(new java.awt.Dimension(400, 600));
        setMinimumSize(new java.awt.Dimension(200, 300));
        setPreferredSize(new java.awt.Dimension(200, 300));

        btBind.setText("BIND");
        btBind.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btBindActionPerformed(evt);
            }
        });

        temperatureBar.setOrientation(1);
        temperatureBar.setValue(37);
        temperatureBar.setStringPainted(true);

        batteryBar.setOrientation(1);
        batteryBar.setValue(37);
        batteryBar.setStringPainted(true);

        jlTeamNumber.setIcon(new javax.swing.ImageIcon(getClass().getResource("/de/naoth/rc/res/system-users.png"))); // NOI18N
        jlTeamNumber.setText("TN");
        jlTeamNumber.setToolTipText("Team Number");

        jlPlayerNumber.setFont(new java.awt.Font("Tahoma", 0, 48)); // NOI18N
        jlPlayerNumber.setIcon(new javax.swing.ImageIcon(getClass().getResource("/de/naoth/rc/res/user-info.png"))); // NOI18N
        jlPlayerNumber.setText("PN");
        jlPlayerNumber.setToolTipText("Player Number");

        jlBallAge.setIcon(new javax.swing.ImageIcon(getClass().getResource("/de/naoth/rc/res/media-record.png"))); // NOI18N
        jlBallAge.setText("BA");
        jlBallAge.setToolTipText("Ball age");

        jlAddress.setIcon(new javax.swing.ImageIcon(getClass().getResource("/de/naoth/rc/res/network-idle.png"))); // NOI18N
        jlAddress.setText("-");

        jlTimestamp.setIcon(new javax.swing.ImageIcon(getClass().getResource("/de/naoth/rc/res/appointment-new.png"))); // NOI18N
        jlTimestamp.setText("DEAD");

        javax.swing.GroupLayout drawingPanelLayout = new javax.swing.GroupLayout(drawingPanel);
        drawingPanel.setLayout(drawingPanelLayout);
        drawingPanelLayout.setHorizontalGroup(
            drawingPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(drawingPanelLayout.createSequentialGroup()
                .addGap(64, 64, 64)
                .addComponent(jlPlayerNumber, javax.swing.GroupLayout.PREFERRED_SIZE, 131, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jlBallAge, javax.swing.GroupLayout.PREFERRED_SIZE, 63, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap(47, Short.MAX_VALUE))
            .addGroup(drawingPanelLayout.createSequentialGroup()
                .addComponent(jlAddress, javax.swing.GroupLayout.PREFERRED_SIZE, 119, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jlTeamNumber, javax.swing.GroupLayout.PREFERRED_SIZE, 66, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jlTimestamp, javax.swing.GroupLayout.PREFERRED_SIZE, 70, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(0, 0, Short.MAX_VALUE))
        );
        drawingPanelLayout.setVerticalGroup(
            drawingPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(drawingPanelLayout.createSequentialGroup()
                .addGroup(drawingPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jlAddress, javax.swing.GroupLayout.PREFERRED_SIZE, 38, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(jlTeamNumber, javax.swing.GroupLayout.PREFERRED_SIZE, 38, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(jlTimestamp, javax.swing.GroupLayout.PREFERRED_SIZE, 38, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addGroup(drawingPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(drawingPanelLayout.createSequentialGroup()
                        .addGap(39, 39, 39)
                        .addComponent(jlPlayerNumber, javax.swing.GroupLayout.PREFERRED_SIZE, 143, javax.swing.GroupLayout.PREFERRED_SIZE))
                    .addGroup(drawingPanelLayout.createSequentialGroup()
                        .addGap(27, 27, 27)
                        .addComponent(jlBallAge, javax.swing.GroupLayout.PREFERRED_SIZE, 65, javax.swing.GroupLayout.PREFERRED_SIZE)))
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addComponent(batteryBar, javax.swing.GroupLayout.PREFERRED_SIZE, 40, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(layout.createSequentialGroup()
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(btBind, javax.swing.GroupLayout.PREFERRED_SIZE, 145, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addGap(0, 0, Short.MAX_VALUE))
                    .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(drawingPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(temperatureBar, javax.swing.GroupLayout.PREFERRED_SIZE, 38, javax.swing.GroupLayout.PREFERRED_SIZE))
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addComponent(drawingPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(btBind, javax.swing.GroupLayout.PREFERRED_SIZE, 36, javax.swing.GroupLayout.PREFERRED_SIZE))
            .addComponent(temperatureBar, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, 343, Short.MAX_VALUE)
            .addComponent(batteryBar, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
        );
    }// </editor-fold>//GEN-END:initComponents

    private void btBindActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btBindActionPerformed
        
    }//GEN-LAST:event_btBindActionPerformed


    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JProgressBar batteryBar;
    private javax.swing.JToggleButton btBind;
    private javax.swing.JPanel drawingPanel;
    private javax.swing.JLabel jLabel5;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JLabel jlAddress;
    private javax.swing.JLabel jlBallAge;
    private javax.swing.JLabel jlPlayerNumber;
    private javax.swing.JLabel jlTeamNumber;
    private javax.swing.JLabel jlTimestamp;
    private javax.swing.JProgressBar temperatureBar;
    // End of variables declaration//GEN-END:variables

}
