/*
 * RobotStatus.java
 *
 * Created on 09.11.2010, 23:10:57
 */

package de.naoth.rc.components;

import de.naoth.rc.dataformats.SPLMessage;
import java.awt.Color;
import java.awt.Font;
import java.awt.FontMetrics;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.RenderingHints;
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
    
    
    private BufferedImage nao_body;
    private BufferedImage battery_ico;
    private BufferedImage temperatur_ico;
    private Color colorInfo = new Color(0.0f, 1.0f, 0.0f, 0.5f);
    private Color colorInfoBlue = new Color(0.0f, 0.0f, 1.0f, 0.5f);
    private Color colorWarning = new Color(0.5f, 0.5f, 0.0f, 0.5f);
    private Color colorDanger = new Color(1.0f, 0.0f, 1.0f, 0.5f);
    
    
    public RemoteRobotPanel(String ipAddress, SPLMessage msg) {
        initComponents();
        
        this.ipAddress = ipAddress;
        this.currentMesage = msg;
        this.jlAddress.setText(this.ipAddress);
        
        try {
            nao_body = ImageIO.read(getClass().getResource("/de/naoth/rc/res/nao-nice.png"));
            battery_ico = ImageIO.read(getClass().getResource("/de/naoth/rc/res/battery-black.png"));
            temperatur_ico = ImageIO.read(getClass().getResource("/de/naoth/rc/res/temperature.png"));
        
            Color fade_color = new Color(230,245,255,100);
            Graphics2D g2d = nao_body.createGraphics();
            g2d.setColor(fade_color);
            g2d.fillRect(0, 0, nao_body.getWidth(), nao_body.getHeight());
            
            this.setBackground(Color.black);
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
      //this.jlPlayerNumber.setText("" + msg.playerNum);
      
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
        /*
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
        */
          
        this.jlTeamNumber.setText("" + msg.teamNum);
      }
    }
    
    
    @Override
    public synchronized void paintComponent(Graphics g)
    {
      super.paintComponent(g);
      Graphics2D g2d = (Graphics2D) g;
      
      g2d.setRenderingHint(RenderingHints.KEY_INTERPOLATION, RenderingHints.VALUE_INTERPOLATION_BICUBIC );
      g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON );
      
      double hPanel = (double) this.getHeight() - 2;
      double wPanel = (double) this.getWidth() - 2;
      
      double hImg = (double) nao_body.getHeight(this);
      double wImg = (double) nao_body.getWidth(this);
      
      double ratioH = hPanel / hImg;
      double ratioW = wPanel / wImg;
      
      double posX = 0.0;
      double posY = 0.0;
      
      g2d.translate((posX + 1), (posY + 1));
      g2d.scale(ratioW, ratioH);

      g2d.drawImage(nao_body, 0, 0, (int) wImg, (int) hImg, this);
      
      g2d.scale(1.0/ratioW, 1.0/ratioH);
      g2d.translate(-(posX + 1), -(posY + 1));
      
      
      g2d.setFont(new Font("TimesRoman", Font.BOLD, 100));
      String playyerNumber = "" + currentMesage.playerNum;
      FontMetrics fm = g.getFontMetrics();
      g2d.drawString(playyerNumber, (int)((wPanel-fm.stringWidth(playyerNumber))/2), (int)(hPanel*0.5-20));
      
      // temperature
      if(getMessage().user.getTemperature() >= 80.0f){
        g2d.setColor(colorDanger);
      } else if(getMessage().user.getTemperature() >= 60.0f) {
        g2d.setColor(colorWarning);
      } else {
          g2d.setColor(colorInfoBlue);
      }
      g2d.drawImage(temperatur_ico, 0, 0, (int)(wPanel*0.1), (int)(wPanel*0.1), this);
      double temperatureValue = getMessage().user.getTemperature()/100.0*hPanel;
      g2d.fillRect(0, (int)(hPanel-temperatureValue), (int)(wPanel*0.1), (int)temperatureValue);
      
      // battery
      if(getMessage().user.getBatteryCharge() <= 0.3f){
        g2d.setColor(colorDanger);
      } else if(getMessage().user.getBatteryCharge() <= 0.6f) {
        g2d.setColor(colorWarning);
      } else {
        g2d.setColor(colorInfo);
      }
      g2d.drawImage(battery_ico, (int)(wPanel*0.9), 0, (int)(wPanel*0.1), (int)(wPanel*0.1), this);
      double batteryValue = getMessage().user.getBatteryCharge()*hPanel;
      g2d.fillRect((int)(wPanel*0.9), (int)(hPanel-batteryValue), (int)(wPanel), (int)batteryValue);
      
      //g2d.rotate(Math.PI*0.5);
      g2d.setFont(new Font("TimesRoman", Font.PLAIN, 20));
      g2d.drawString(String.format("TEMP %3.1f °C", getMessage().user.getTemperature()), 10, (int)hPanel / 2);
      //g2d.rotate(-Math.PI*0.5);
      
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
        jlAddress = new javax.swing.JLabel();
        jlBallAge = new javax.swing.JLabel();
        jlTeamNumber = new javax.swing.JLabel();
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

        jlAddress.setIcon(new javax.swing.ImageIcon(getClass().getResource("/de/naoth/rc/res/network-idle.png"))); // NOI18N
        jlAddress.setText("-");

        jlBallAge.setIcon(new javax.swing.ImageIcon(getClass().getResource("/de/naoth/rc/res/media-record.png"))); // NOI18N
        jlBallAge.setText("BA");
        jlBallAge.setToolTipText("Ball age");

        jlTeamNumber.setIcon(new javax.swing.ImageIcon(getClass().getResource("/de/naoth/rc/res/system-users.png"))); // NOI18N
        jlTeamNumber.setText("TN");
        jlTeamNumber.setToolTipText("Team Number");

        jlTimestamp.setIcon(new javax.swing.ImageIcon(getClass().getResource("/de/naoth/rc/res/appointment-new.png"))); // NOI18N
        jlTimestamp.setText("DEAD");

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(layout.createSequentialGroup()
                        .addContainerGap()
                        .addComponent(jlBallAge, javax.swing.GroupLayout.PREFERRED_SIZE, 63, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addGap(29, 29, 29)
                        .addComponent(jlTeamNumber, javax.swing.GroupLayout.PREFERRED_SIZE, 66, javax.swing.GroupLayout.PREFERRED_SIZE))
                    .addGroup(layout.createSequentialGroup()
                        .addGap(26, 26, 26)
                        .addComponent(jlAddress, javax.swing.GroupLayout.PREFERRED_SIZE, 119, javax.swing.GroupLayout.PREFERRED_SIZE)))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 34, Short.MAX_VALUE)
                .addComponent(jlTimestamp, javax.swing.GroupLayout.PREFERRED_SIZE, 70, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(25, 25, 25))
            .addGroup(layout.createSequentialGroup()
                .addGap(31, 31, 31)
                .addComponent(btBind, javax.swing.GroupLayout.PREFERRED_SIZE, 145, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addGap(165, 165, 165)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jlBallAge, javax.swing.GroupLayout.PREFERRED_SIZE, 65, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(jlTeamNumber, javax.swing.GroupLayout.PREFERRED_SIZE, 38, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addComponent(jlAddress, javax.swing.GroupLayout.PREFERRED_SIZE, 38, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 67, Short.MAX_VALUE)
                .addComponent(btBind, javax.swing.GroupLayout.PREFERRED_SIZE, 36, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap())
            .addGroup(layout.createSequentialGroup()
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addComponent(jlTimestamp, javax.swing.GroupLayout.PREFERRED_SIZE, 38, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(182, 182, 182))
        );
    }// </editor-fold>//GEN-END:initComponents

    private void btBindActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btBindActionPerformed
        
    }//GEN-LAST:event_btBindActionPerformed


    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JToggleButton btBind;
    private javax.swing.JLabel jLabel5;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JLabel jlAddress;
    private javax.swing.JLabel jlBallAge;
    private javax.swing.JLabel jlTeamNumber;
    private javax.swing.JLabel jlTimestamp;
    // End of variables declaration//GEN-END:variables

}
