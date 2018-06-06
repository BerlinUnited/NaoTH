/*
 * RobotStatus.java
 *
 * Created on 09.11.2010, 23:10:57
 */

package de.naoth.rc.dialogs.bdr;

import de.naoth.rc.dataformats.SPLMessage;
import de.naoth.rc.messages.TeamMessageOuterClass;
import de.naoth.rc.server.ConnectionStatusEvent;
import de.naoth.rc.server.ConnectionStatusListener;
import de.naoth.rc.server.MessageServer;
import java.awt.AlphaComposite;
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

// TODO: isConnected? -> disable Connect button

/**
 *
 * @author Heinrich Mellmann
 */
public class RobotPanel extends javax.swing.JPanel {

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
    
    private final MessageServer messageServer;
    private String ipAddress;
    private SPLMessage currentMesage;
    private boolean hideConnectButton = false;
    
    static class Images {
        private BufferedImage nao_body;
        private BufferedImage battery_ico;
        private BufferedImage temperatur_ico;

        public Images() {
            try {
                nao_body = ImageIO.read(RobotPanel.class.getResource("/de/naoth/rc/res/nao-nice.png"));
                battery_ico = ImageIO.read(RobotPanel.class.getResource("/de/naoth/rc/res/battery-white.png"));
                temperatur_ico = ImageIO.read(RobotPanel.class.getResource("/de/naoth/rc/res/temperature-white.png"));

                Color fade_color = new Color(230,245,255,100);
                Graphics2D g2d = nao_body.createGraphics();
                g2d.setColor(fade_color);
                g2d.fillRect(0, 0, nao_body.getWidth(), nao_body.getHeight());

            } catch (IOException ex) {
                java.util.logging.Logger.getLogger(RobotPanel.class.getName()).log(java.util.logging.Level.SEVERE, "Coult not load images", ex);
            }
        }
        
        public BufferedImage getNaoBody() { return nao_body; }
        public BufferedImage getBatteryIco() { return battery_ico; }
        public BufferedImage getTemperatureIco() { return temperatur_ico; }
    };
    
    private static final Images IMAGES = new Images();
    
    private final Color colorInfo = new Color(0.0f, 1.0f, 0.0f, 0.5f);
    private final Color colorInfoBlue = new Color(0.1f, 0.0f, 1.0f, 0.5f);
    private final Color colorWarning = new Color(1.0f, 1.0f, 0.0f, 0.5f);
    private final Color colorDanger = new Color(1.0f, 0.0f, 0.0f, 0.5f);

    private final Color disabledColor = Color.GRAY;
    private final Color disabledTextColor = Color.BLACK;
    private final Color disabledChestColor = new Color(0.0f,0.0f,0.0f,0.7f);
    
    private Color chestColor = new Color(0.0f,0.0f,0.0f,0.7f);
    
    public RobotPanel(String ipAddress, SPLMessage msg) {
        this(null, ipAddress, msg);
    }
    
    public RobotPanel(MessageServer messageServer, String ipAddress, SPLMessage msg) {
        initComponents();
        
        this.messageServer = messageServer;
        this.ipAddress = ipAddress;
        this.currentMesage = msg;
        this.jlAddress.setText(this.ipAddress);

        this.setBackground(Color.black);
        lblActivity.setVisible(false);
        
        if(this.messageServer != null) {
            this.messageServer.addConnectionStatusListener(new ConnectionStatusListener() {

                @Override
                public void connected(ConnectionStatusEvent event) {
                    // my ip
                    if(getAddress().equals(event.getAddress().getHostString())) {
                        connectButton.setText("Disconnect");
                    } else {
                        setEnableConnectButton(false);
                    }
                }

                @Override
                public void disconnected(ConnectionStatusEvent event) {
                    connectButton.setText("Connect");
                    setEnableConnectButton(true);
                }
            });
        }
    }
    
    public SPLMessage getMessage() {
        return currentMesage;
    }
    
    public String getAddress() {
        return ipAddress;
    }
    
    public Long getLastTimestamp() {
        return timestamps.size() > 0 ? timestamps.get(timestamps.size()-1) : System.currentTimeMillis();
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
        this.jlTimestamp.setText("OFFLINE");
        this.jlTimestamp.setForeground(Color.black);
      }
      else
      {
        this.jlTimestamp.setText(String.format("%4.2f msg/s", msgPerSecond));
        this.jlTimestamp.setForeground(Color.white);
      }
      
      //this.jlFallenTime.setText(msg.fallen == 1 ? "FALLEN" : "NOT FALLEN");
      //this.jlBallAge.setText("" + msg.ballAge + "s");
      
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
        if(msg.user.hasRobotState()) {
            this.jlRobotState.setText(msg.getRobotState());
            
            if(msg.user.getRobotState() == TeamMessageOuterClass.RobotState.playing) {
                lblActivity.setVisible(true);
                lblActivity.setText(msg.user.getBdrPlayerState().getActivity().name().toUpperCase());
            } else {
                lblActivity.setVisible(false);
            }
        }
      }
      
      // update color - in order it changed
      if(msg.user.hasTeamColor()) {
          setChestColor(msg.getTeamColor());
      }
      
      jlCpuTemp.setText(msg.user.getCpuTemperature() + " °C");
      
      this.repaint();
    }
    
    public void setChestColor(Color chestColor) {
        this.chestColor = chestColor;
        this.repaint();
    }
    
    public Color getChestColor() {
        return chestColor;
    }

    @Override
    public void setEnabled(boolean enabled) {
        super.setEnabled(enabled);
        if(!hideConnectButton) {
            connectButton.setEnabled(enabled);
            connectButton.setVisible(enabled);
        }
    }
    
    public void setHideConnectButton(boolean hide) {
        hideConnectButton = hide;
        connectButton.setEnabled(!hide);
        connectButton.setVisible(!hide);
    }
    
    public void setEnableConnectButton(boolean enabled) {
        if(!hideConnectButton) {
            connectButton.setEnabled(enabled);
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
      
      double hImg = (double) IMAGES.getNaoBody().getHeight(this);
      double wImg = (double) IMAGES.getNaoBody().getWidth(this);
      
      double ratioH = hPanel / hImg;
      double ratioW = wPanel / wImg;
      
      double posX = 0.0;
      double posY = 0.0;
      
      g2d.translate((posX + 1), (posY + 1));
      g2d.scale(ratioW, ratioH);

      if(!isEnabled()) {
        g2d.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC_OVER, .5f));
      }
      g2d.drawImage(IMAGES.getNaoBody(), 0, 0, (int) wImg, (int) hImg, this);
      
      g2d.scale(1.0/ratioW, 1.0/ratioH);
      g2d.translate(-(posX + 1), -(posY + 1));
      
      // player number
      
      //isEnabled() ? chestColor : disabledChestColor
      g2d.setColor(disabledChestColor);
      int r = 50;
      g2d.fillOval((int)(wPanel/2)-r, (int)(hPanel*0.5)-r, 2*r, 2*r);

      g2d.setColor(isEnabled() ? chestColor : disabledColor);
      g2d.setFont(new Font("TimesRoman", Font.BOLD, 100));
      String playyerNumber = "" + currentMesage.playerNum;
      FontMetrics fm = g.getFontMetrics();
      int y = (fm.getAscent() + ((int)(hPanel+0.5) - (fm.getAscent() + fm.getDescent())) / 2);
      int x = (int)((wPanel-fm.stringWidth(playyerNumber))/2);
      g2d.drawString(playyerNumber, x, y);
      
        // temperature
        if (isEnabled()) {
            if (getMessage().user.getTemperature() >= 80.0f) {
                g2d.setColor(colorDanger);
            } else if (getMessage().user.getTemperature() >= 60.0f) {
                g2d.setColor(colorWarning);
            } else {
                g2d.setColor(colorInfoBlue);
            }
        } else {
            g2d.setColor(disabledColor);
        }
      double temperatureValue = getMessage().user.getTemperature()/100.0*hPanel;
      g2d.fillRect(0, (int)(hPanel-temperatureValue), (int)(wPanel*0.1), (int)temperatureValue);
      g2d.drawImage(IMAGES.getTemperatureIco(), 0, 0, (int)(wPanel*0.1), (int)(wPanel*0.1), this);
      
        // battery
        if (isEnabled()) {
            if (getMessage().user.getBatteryCharge() <= 0.3f) {
                g2d.setColor(colorDanger);
            } else if (getMessage().user.getBatteryCharge() <= 0.6f) {
                g2d.setColor(colorWarning);
            } else {
                g2d.setColor(colorInfo);
            }
        } else {
            g2d.setColor(disabledColor);
        }
      double batteryValue = getMessage().user.getBatteryCharge()*hPanel;
      g2d.fillRect((int)(wPanel*0.9), (int)(hPanel-batteryValue), (int)(wPanel), (int)batteryValue);
      g2d.translate(20, (int)hPanel / 2);
      g2d.rotate(Math.PI*-0.5);
      g2d.setColor(isEnabled() ? Color.white : disabledTextColor);
      g2d.setFont(new Font("TimesRoman", Font.PLAIN, 18));
      g2d.drawString(String.format("%3.1f °C", getMessage().user.getTemperature()), 0, 0);
      g2d.rotate(Math.PI*0.5);
      g2d.translate(-20, -(int)hPanel / 2);
      
      g2d.translate((int)(wPanel - 10), (int)hPanel / 2);
      g2d.rotate(Math.PI*-0.5);
      g2d.setColor(isEnabled() ? Color.white : disabledTextColor);
      g2d.setFont(new Font("TimesRoman", Font.PLAIN, 18));
      if(getMessage().user.getIsCharging()) {
        g2d.drawString(String.format("%3.0f%% (charging)", getMessage().user.getBatteryCharge()*100), 0,0);
      } else {
        g2d.drawString(String.format("%3.0f%%", getMessage().user.getBatteryCharge()*100), 0,0);
      }
      g2d.rotate(Math.PI*0.5);
      g2d.translate(-(int)(wPanel - 10), -(int)hPanel / 2);
      g2d.drawImage(IMAGES.getBatteryIco(), (int)(wPanel*0.9), 0, (int)(wPanel*0.1), (int)(wPanel*0.1), this);
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

    public static interface UnbindListerer
    {
        public abstract void unbind();
    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {
        java.awt.GridBagConstraints gridBagConstraints;

        jLabel5 = new javax.swing.JLabel();
        jPanel1 = new javax.swing.JPanel();
        labelPanel = new javax.swing.JPanel();
        filler1 = new javax.swing.Box.Filler(new java.awt.Dimension(0, 0), new java.awt.Dimension(0, 0), new java.awt.Dimension(0, 0));
        jlAddress = new javax.swing.JLabel();
        jlRobotState = new javax.swing.JLabel();
        jlTimestamp = new javax.swing.JLabel();
        jlCpuTemp = new javax.swing.JLabel();
        filler2 = new javax.swing.Box.Filler(new java.awt.Dimension(0, 0), new java.awt.Dimension(0, 0), new java.awt.Dimension(0, 0));
        jPanel2 = new javax.swing.JPanel();
        lblActivity = new javax.swing.JLabel();
        buttonPanel = new javax.swing.JPanel();
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
        setMaximumSize(new java.awt.Dimension(400, 600));
        setMinimumSize(new java.awt.Dimension(200, 300));
        setPreferredSize(new java.awt.Dimension(200, 300));
        setLayout(new java.awt.BorderLayout());

        labelPanel.setOpaque(false);
        labelPanel.setLayout(new java.awt.GridBagLayout());
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.weightx = 0.4;
        labelPanel.add(filler1, gridBagConstraints);

        jlAddress.setFont(new java.awt.Font("Tahoma", 1, 14)); // NOI18N
        jlAddress.setForeground(new java.awt.Color(255, 255, 255));
        jlAddress.setIcon(new javax.swing.ImageIcon(getClass().getResource("/de/naoth/rc/res/network-idle.png"))); // NOI18N
        jlAddress.setText("127.0.0.1");
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.weightx = 0.3;
        labelPanel.add(jlAddress, gridBagConstraints);

        jlRobotState.setFont(new java.awt.Font("Tahoma", 1, 14)); // NOI18N
        jlRobotState.setForeground(new java.awt.Color(255, 255, 255));
        jlRobotState.setIcon(new javax.swing.ImageIcon(getClass().getResource("/toolbarButtonGraphics/general/Information16.gif"))); // NOI18N
        jlRobotState.setText("INITIAL");
        jlRobotState.setToolTipText("Team Number");
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        labelPanel.add(jlRobotState, gridBagConstraints);

        jlTimestamp.setFont(new java.awt.Font("Tahoma", 1, 14)); // NOI18N
        jlTimestamp.setForeground(new java.awt.Color(255, 255, 255));
        jlTimestamp.setIcon(new javax.swing.ImageIcon(getClass().getResource("/de/naoth/rc/res/appointment-new.png"))); // NOI18N
        jlTimestamp.setText("OFFLINE");
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 2;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHEAST;
        gridBagConstraints.weightx = 0.3;
        labelPanel.add(jlTimestamp, gridBagConstraints);

        jlCpuTemp.setFont(new java.awt.Font("Tahoma", 1, 14)); // NOI18N
        jlCpuTemp.setForeground(new java.awt.Color(255, 255, 255));
        jlCpuTemp.setIcon(new javax.swing.ImageIcon(getClass().getResource("/de/naoth/rc/res/thermometer.png"))); // NOI18N
        jlCpuTemp.setText("0.0 °C");
        jlCpuTemp.setToolTipText("Team Number");
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 2;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHEAST;
        labelPanel.add(jlCpuTemp, gridBagConstraints);
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.weightx = 0.3;
        labelPanel.add(filler2, gridBagConstraints);

        add(labelPanel, java.awt.BorderLayout.NORTH);

        jPanel2.setOpaque(false);
        jPanel2.setLayout(new javax.swing.BoxLayout(jPanel2, javax.swing.BoxLayout.Y_AXIS));

        lblActivity.setFont(new java.awt.Font("Dialog", 1, 18)); // NOI18N
        lblActivity.setForeground(new java.awt.Color(0, 102, 51));
        lblActivity.setHorizontalAlignment(javax.swing.SwingConstants.CENTER);
        lblActivity.setText("activity");
        lblActivity.setAlignmentX(0.5F);
        lblActivity.setBorder(javax.swing.BorderFactory.createEmptyBorder(10, 10, 10, 10));
        lblActivity.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        lblActivity.setOpaque(true);
        jPanel2.add(lblActivity);

        buttonPanel.setOpaque(false);
        buttonPanel.setPreferredSize(new java.awt.Dimension(50, 40));
        buttonPanel.setLayout(new java.awt.BorderLayout());

        connectButton.setText("Connect");
        connectButton.setMaximumSize(new java.awt.Dimension(50, 50));
        connectButton.setMinimumSize(new java.awt.Dimension(50, 50));
        connectButton.setPreferredSize(new java.awt.Dimension(50, 23));
        connectButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                connectButtonActionPerformed(evt);
            }
        });
        buttonPanel.add(connectButton, java.awt.BorderLayout.CENTER);

        jPanel2.add(buttonPanel);

        add(jPanel2, java.awt.BorderLayout.SOUTH);
    }// </editor-fold>//GEN-END:initComponents

    private void connectButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_connectButtonActionPerformed
        if(messageServer != null) {
            if(!this.messageServer.isConnected()) {
                try {
                    this.messageServer.connect(this.ipAddress, 5401);
                    //connectButton.setText("Disconnect");
                } catch(IOException ex) {
                    java.util.logging.Logger.getLogger(RobotPanel.class.getName()).log(java.util.logging.Level.SEVERE, "Coult not connect.", ex);
                }
            } else {
                this.messageServer.disconnect();
                //connectButton.setText("Connect");
            }
        }
    }//GEN-LAST:event_connectButtonActionPerformed


    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JPanel buttonPanel;
    private javax.swing.JButton connectButton;
    private javax.swing.Box.Filler filler1;
    private javax.swing.Box.Filler filler2;
    private javax.swing.JLabel jLabel5;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JPanel jPanel2;
    private javax.swing.JLabel jlAddress;
    private javax.swing.JLabel jlCpuTemp;
    private javax.swing.JLabel jlRobotState;
    private javax.swing.JLabel jlTimestamp;
    private javax.swing.JPanel labelPanel;
    private javax.swing.JLabel lblActivity;
    // End of variables declaration//GEN-END:variables

}
