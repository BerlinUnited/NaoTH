/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * RobotHealth.java
 *
 * Created on 22.02.2012, 19:12:12
 */
package de.naoth.rc.dialogs;

import com.google.protobuf.InvalidProtocolBufferException;
import de.naoth.rc.AbstractDialog;
import de.naoth.rc.DialogPlugin;
import de.naoth.rc.RobotControl;
import de.naoth.rc.dialogs.drawings.Drawable;
import de.naoth.rc.manager.GenericManagerFactory;
import de.naoth.rc.manager.ObjectListener;
import de.naoth.rc.messages.FrameworkRepresentations;
import de.naoth.rc.messages.Representations;
import de.naoth.rc.server.Command;
import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.image.BufferedImage;
import java.io.IOException;
import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.imageio.ImageIO;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 *
 * @author admin
 */
public class RobotHealth extends AbstractDialog
{

    
  @PluginImplementation
  public static class Plugin extends DialogPlugin<RobotHealth>
  {
      @InjectPlugin
      public static RobotControl parent;
      @InjectPlugin
      public static GenericManagerFactory genericManagerFactory;
  }//end Plugin
    
  private final Command getFSRDataCommand = new Command("representation:getbinary").addArg("FSRData");
  private final Command getSensorJointDataCommand = new Command("representation:getbinary").addArg("SensorJointData");
  
  
  FSRDataListener fSRDataListener = new FSRDataListener();
  SensorJointDataListener sensorJointDataListener = new SensorJointDataListener();
  
  BufferedImage nao_body;
  BufferedImage nao_feet;
  
  final int nao_feet_offset_x = 300;
  final int nao_feet_offset_y = 0;
  
  
  private final String jointNames[] = {
      "HeadPitch",
      "HeadYaw",

      "RShoulderRoll",
      "LShoulderRoll",
      "RShoulderPitch",
      "LShoulderPitch",
      "RElbowRoll",
      "LElbowRoll",
      "RElbowYaw",
      "LElbowYaw",

      "(L/R)HipYawPitch",
      "",
      
      "RHipPitch",
      "LHipPitch",
      "RHipRoll",
      "LHipRoll",
      "RKneePitch",
      "LKneePitch",
      "RAnklePitch",
      "LAnklePitch",
      "RAnkleRoll",
      "LAnkleRoll" 
  };
  
  private final int joints[] = { // order equals "JointData.h"
                     0,  1080, // HeadPitch
                     0,  920, // HeadYaw
                     
                   340,  760, // RShoulderRoll
                  -340,  760, // LShoulderRoll
                   420,  600, // RShoulderPitch
                  -420,  600, // LShoulderPitch
                  
                   600,  200, // RElbowRoll
                  -600,  200, // LElbowRoll
                   600,  360, // RElbowYaw
                  -600,  360, // LElbowYaw
                  
                     0,  110, // RHipYawPitch
                   - 0,  110, // LHipYawPitch
                   
                   260, -100, // RHipPitch
                  -260, -100, // LHipPitch
                   260, -260, // RHipRoll
                  -260, -260, // LHipRoll
                  
                   260, -500, // RKneePitch
                  -260, -500, // LKneePitch
                  
                   260, -850, // RAnklePitch
                  -260, -850, // LAnklePitch
                   260, -1010, // RAnkleRoll
                  -260, -1010  // LAnkleRoll
      };
  
  private final MicroPlot[] jointPlots = new MicroPlot[joints.length];
  
  
  // ATTENTION: the order is the same as the in the NaoTH-2011
  final FSRState[] theFSRStates = new FSRState[]{
      new FSRState(45, 84, 10), // theLFsrFL
      new FSRState(139, 85, 10), // theLFsrFR
      new FSRState(45, 262, 10), // theLFsrBL
      new FSRState(133, 262, 10), // theLFsrBR

      new FSRState(262, 84, 10), // theRFsrFL
      new FSRState(356, 85, 10), // theRFsrFR
      new FSRState(270, 262, 10), // theRFsrBL
      new FSRState(356, 262, 10) // theRFsrBR
   };
  
    /** Creates new form RobotHealth */
    public RobotHealth() {
        initComponents();
        
        try {
            nao_body = ImageIO.read(getClass().getResource("/de/naoth/rc/res/nao_body.png"));
            nao_feet = ImageIO.read(getClass().getResource("/de/naoth/rc/res/nao_feet.jpg"));
        
            Color fade_color = new Color(230,245,255,200);
            Graphics2D g2d = nao_body.createGraphics();
            g2d.setColor(fade_color);
            g2d.fillRect(0, 0, nao_body.getWidth(), nao_body.getHeight());
            g2d = nao_feet.createGraphics();
            g2d.setColor(fade_color);
            g2d.fillRect(0, 0, nao_feet.getWidth(), nao_feet.getHeight());
            
        } catch (IOException e) {
        }
        
        for(int i = 0; i < joints.length; i++)
        {
            jointPlots[i] = new MicroPlot(-30, 30);
        }
    }
    
    @Override
    public void dispose()
    {
        Plugin.genericManagerFactory.getManager(getFSRDataCommand).removeListener(fSRDataListener);
        Plugin.genericManagerFactory.getManager(getSensorJointDataCommand).removeListener(sensorJointDataListener);
    }
    
    // this is called by the drawingPanel.paintComponent(...)
    // see: (NetBeans) Properties: "Custom Creation Code" for drawingPanel
      public synchronized void paintDrawingComponent(Graphics g)
      {
        Graphics2D g2d = (Graphics2D) g;
        
        g2d.drawImage(nao_body, null, 0, 0);
        g2d.drawImage(nao_feet, null, nao_feet_offset_x, nao_feet_offset_y);
        /*
        g2d.setColor(new Color(200,230,255,180));
        g2d.fillRect(0, 0, 1000, 1000);
        g2d.setColor(Color.black);
        */
        g2d.translate(nao_feet_offset_x, nao_feet_offset_y);
        for(int i = 0; i < theFSRStates.length; i++)
        {
            theFSRStates[i].draw(g2d);
        }
        g2d.translate(-nao_feet_offset_x, -nao_feet_offset_y);
        
        
        g2d.translate(150, 40);
        if(sensorJointData != null)
        {
            for(int i = 0; i < sensorJointData.getTemperatureCount(); i++)
            {
                int x = (joints[0] - joints[i*2])/5;// mirror horizontal
                int y = (joints[1] - joints[i*2+1])/5; // mirror vertical
                int radius = 10;
                
                double value = sensorJointData.getTemperature(i);
                //double value = sensorJointData.getJointData().getPosition(i);
                
                g2d.setColor(getColorMix(0, 85f, (float)value));
                g2d.fillOval(x-radius, y-radius, radius*2, radius*2);
                g2d.setColor(Color.black);
                g2d.drawString(df.format(value), x+10, y+5);
                g2d.drawString(jointNames[i], x+10, y-10);
                
                g2d.translate(x, y);
                jointPlots[i].draw(g2d);
                g2d.translate(-x,-y);
            }
        }//end if
        g2d.translate(-150, -40);
        
      }//end paintComponent

      
    class FSRDataListener implements ObjectListener<byte[]>
    {
        @Override
        public void errorOccured(String cause) {
            Plugin.genericManagerFactory.getManager(getFSRDataCommand).removeListener(this);
        }

        @Override
        public void newObjectReceived(byte[] object) 
        {
          try
          {
            FrameworkRepresentations.FSRData fsrData = FrameworkRepresentations.FSRData.parseFrom(object);

            for(int i = 0; i < theFSRStates.length; i++)
            {
                theFSRStates[i].setValue(fsrData.getForce(i));
            }

            
            RobotHealth.this.repaint();
          }
          catch (InvalidProtocolBufferException ex)
          {
            //ex.printStackTrace();
            Logger.getLogger(RobotHealth.class.getName()).log(Level.SEVERE, null, ex);
            errorOccured(ex.getMessage());
          }
        }//end newObjectReceived
    }//end class FSRDataListener
      
    
    FrameworkRepresentations.SensorJointData sensorJointData;
    class SensorJointDataListener implements ObjectListener<byte[]>
    {
        @Override
        public void errorOccured(String cause) {
            Plugin.genericManagerFactory.getManager(getSensorJointDataCommand).removeListener(this);
        }

        @Override
        public void newObjectReceived(byte[] object) 
        {
          try
          {
            sensorJointData = FrameworkRepresentations.SensorJointData.parseFrom(object);
            
            for(int i = 0; i < sensorJointData.getTemperatureCount(); i++)
            {
                //double v = sensorJointData.getJointData().getPosition(i)*10;
                double v = sensorJointData.getElectricCurrent(i)*10;
                jointPlots[i].setValue(v);
            }
            
            RobotHealth.this.repaint();
          }
          catch (InvalidProtocolBufferException ex)
          {
            //ex.printStackTrace();
            Logger.getLogger(RobotHealth.class.getName()).log(Level.SEVERE, null, ex);
            errorOccured(ex.getMessage());
          }
        }//end newObjectReceived
    }//end class SensorJointDataListener
    
    
      
      
    DecimalFormat df =   new DecimalFormat  ( "##0.00" );
    class FSRState implements Drawable
    {
        private double value;
        private int x;
        private int y;
        private int radius;

        public FSRState(int x, int y, int radius) {
            this.x = x;
            this.y = y;
            this.radius = radius;
        }
        
        public void setValue(double value){ this.value = value; }
        
        @Override
        public void draw(Graphics2D g2d) 
        {
            g2d.setColor(getColor(0, 12.5f, (float)value));
            g2d.fillOval(x-radius, y-radius, radius*2, radius*2);
            
            g2d.setColor(Color.black);
            g2d.drawOval(x-radius, y-radius, radius*2, radius*2);
            
            g2d.drawString("["+df.format(value)+"]", x+radius+3, y);
        }
    }
    
    private Color getColor(float min, float max, float v)
    {
        //float min = 0;
        //float max = 12.5f;

        if(v > max)
            return Color.black;

        float t = Math.max(Math.min(v,max),min);
        float d = (t-min)/(max - min);

        return new Color(1.0f,0,0.0f, d);
    }//end getColor
    
    
    private Color getColorMix(float min, float max, float v)
    {
        //float min = 0;
        //float max = 12.5f;

        if(v > max)
            return Color.black;

        float t = Math.max(Math.min(v,max),min);
        float d = (t-min)/(max - min);

        return new Color(d,1.0f-d,0.0f);
    }//end getColor
    
    
    
    
    class MicroPlot implements Drawable
    {
        private int min;
        private int max;
        private int samples = 20;
        private int step = 3;
        private int height = 30;

        private ArrayList<Double> values = new ArrayList<Double>();
        
        public MicroPlot(int min, int max) {
            this.min = min;
            this.max = max;
        }
        
        public void setValue(double value)
        {
            values.add(value);
            if(values.size() > samples)
                values.remove(0);
        }
        
        @Override
        public void draw(Graphics2D g2d) 
        {
            //g2d.setColor(Color.white);
            //g2d.fillRect(0, 0, samples, height);
            
            if(values.isEmpty()) return;
            
            double last = values.get(0);
            int lastX = 0;
            
            for(Double d: values)
            {
                // HACK: invert because of coordinate system
                double value = -d.doubleValue();
                g2d.setColor(getColorMix(min, max, (float)value));
                g2d.drawLine(lastX, (int)last, lastX+step, (int)value);
                
                last = value;
                lastX += step+1;
            }
        }
    }//end class MicroPlot
    
    

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        jToolBar1 = new javax.swing.JToolBar();
        btReceiveDrawings = new javax.swing.JToggleButton();
        drawingPanel = new javax.swing.JPanel()
        {
            public synchronized void paintComponent(Graphics g)
            {
                super.paintComponent(g);
                // call the parent painter method
                paintDrawingComponent(g); 
            }
        };

        jToolBar1.setFloatable(false);
        jToolBar1.setRollover(true);

        btReceiveDrawings.setText("Receive");
        btReceiveDrawings.setFocusable(false);
        btReceiveDrawings.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btReceiveDrawings.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btReceiveDrawings.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btReceiveDrawingsActionPerformed(evt);
            }
        });
        jToolBar1.add(btReceiveDrawings);

        javax.swing.GroupLayout drawingPanelLayout = new javax.swing.GroupLayout(drawingPanel);
        drawingPanel.setLayout(drawingPanelLayout);
        drawingPanelLayout.setHorizontalGroup(
            drawingPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 400, Short.MAX_VALUE)
        );
        drawingPanelLayout.setVerticalGroup(
            drawingPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 275, Short.MAX_VALUE)
        );

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jToolBar1, javax.swing.GroupLayout.DEFAULT_SIZE, 400, Short.MAX_VALUE)
            .addComponent(drawingPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addComponent(jToolBar1, javax.swing.GroupLayout.PREFERRED_SIZE, 25, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(0, 0, 0)
                .addComponent(drawingPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );
    }// </editor-fold>//GEN-END:initComponents

    private void btReceiveDrawingsActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btReceiveDrawingsActionPerformed
        if(btReceiveDrawings.isSelected()) {
            if(Plugin.parent.checkConnected()) {
                Plugin.genericManagerFactory.getManager(getFSRDataCommand).addListener(fSRDataListener);
                Plugin.genericManagerFactory.getManager(getSensorJointDataCommand).addListener(sensorJointDataListener);
            } else {
                btReceiveDrawings.setSelected(false);
            }
        } else {
                Plugin.genericManagerFactory.getManager(getFSRDataCommand).removeListener(fSRDataListener);
                Plugin.genericManagerFactory.getManager(getSensorJointDataCommand).removeListener(sensorJointDataListener);
        }
}//GEN-LAST:event_btReceiveDrawingsActionPerformed

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JToggleButton btReceiveDrawings;
    private javax.swing.JPanel drawingPanel;
    private javax.swing.JToolBar jToolBar1;
    // End of variables declaration//GEN-END:variables
}
