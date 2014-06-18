package de.naoth.rc.dialogs;

import com.google.protobuf.InvalidProtocolBufferException;
import de.naoth.rc.AbstractDialog;
import de.naoth.rc.DialogPlugin;
import de.naoth.rc.RobotControl;
import de.naoth.rc.manager.GenericManagerFactory;
import de.naoth.rc.manager.ObjectListener;
import de.naoth.rc.messages.Representations;
import de.naoth.rc.server.Command;
import java.awt.Color;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.util.HashMap;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.JOptionPane;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 *
 * @author  Thomas
 * @author  Heinrich
 */
public class GroundTruthReader extends AbstractDialog
{

    private void openFile() {
       String fileName = LogfilePlayer.getFileName();
       fileName = fileName.substring(0, fileName.lastIndexOf(".")+1) +"gts";
       ObjectInputStream o;
        try {

            o = new ObjectInputStream(new FileInputStream
                                (fileName));
            topBall = (Map<Integer,Boolean>) o.readObject();
            bottomBall = (Map<Integer,Boolean>) o.readObject();
            topGoal = (Map<Integer,Boolean>) o.readObject();
            bottomGoal = (Map<Integer,Boolean>) o.readObject();
            
        } catch (IOException ex) {
             JOptionPane.showMessageDialog(null,"No Groundtruth for this lopgfile", "Error", JOptionPane.ERROR_MESSAGE);
        } catch (ClassNotFoundException ex) {
            Logger.getLogger(GroundTruthReader.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
    
    @PluginImplementation
    public static class Plugin extends DialogPlugin<GroundTruthReader> {
    @InjectPlugin
    public static RobotControl parent;  
    @InjectPlugin
    public static GenericManagerFactory genericManagerFactory;
  /*  @InjectPlugin
    public static Representation frameworkRepresentation;
    */
  }//end Plugin

  private final Command getBallDataCommand = new Command("Cognition:representation:getbinary").addArg("BallPercept");
  private final Command getBallTopDataCommand = new Command("Cognition:representation:getbinary").addArg("BallPerceptTop");
  private final Command getGoalTopDataCommand = new Command("Cognition:representation:getbinary").addArg("GoalPerceptTop");
  private final Command getGoalDataCommand = new Command("Cognition:representation:getbinary").addArg("GoalPerceptTop");
  //private final Command getFrameInfoCommand = new Command("Cognition:representation:getbinary").addArg("FrameInfo");
          
  
  
  private final BallDataListener ballTopListener = new BallDataListener(getBallTopDataCommand, true);
  private final BallDataListener ballListener = new BallDataListener(getBallDataCommand, false);
  private final GoalDataListener goalListener =  new GoalDataListener(getGoalDataCommand, false);
  private final GoalDataListener goalTopListener = new GoalDataListener(getGoalTopDataCommand, true);
  //private final FrameInfoListener frameInfoListener = new FrameInfoListener(getFrameInfoCommand);
  
  private Map<Integer,Boolean> topBall,
                               bottomBall,
                               topGoal,
                               bottomGoal;
  
 
  


    public GroundTruthReader()    {
         
        initComponents();
     // Plugin.genericManagerFactory.getManager(getBallDataCommand).addListener(ballListener);
      //Plugin.genericManagerFactory.getManager(getBallTopDataCommand).addListener(ballTopListener);

    }
  
    public boolean initListeners () {
        try {
            Plugin.genericManagerFactory.getManager(getBallDataCommand).addListener(ballListener);
            Plugin.genericManagerFactory.getManager(getBallTopDataCommand).addListener(ballTopListener);
            Plugin.genericManagerFactory.getManager(getGoalDataCommand).addListener(goalListener);
            Plugin.genericManagerFactory.getManager(getGoalDataCommand).addListener(goalTopListener);
        } catch (java.nio.channels.NotYetConnectedException ex) {
            return false;
        }
        return true;
    }
  
    @Override
    public void dispose()
    {
        Plugin.genericManagerFactory.getManager(getBallDataCommand).removeListener(ballListener);
        Plugin.genericManagerFactory.getManager(getBallTopDataCommand).removeListener(ballTopListener);
        Plugin.genericManagerFactory.getManager(getGoalDataCommand).removeListener(goalListener);
        Plugin.genericManagerFactory.getManager(getGoalTopDataCommand).removeListener(goalTopListener);
       // Plugin.genericManagerFactory.getManager(getFrameInfoCommand).removeListener(frameInfoListener);
    }

  /** This method is called from within the constructor to
   * initialize the form.
   * WARNING: Do NOT modify this code. The content of this method is
   * always regenerated by the Form Editor.
   */
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        buttonGroup1 = new javax.swing.ButtonGroup();
        buttonGroup2 = new javax.swing.ButtonGroup();
        jToggleButton3 = new javax.swing.JToggleButton();
        jScrollPane1 = new javax.swing.JScrollPane();
        jTextArea1 = new javax.swing.JTextArea();

        jToggleButton3.setText("listen");
        jToggleButton3.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jToggleButton3ActionPerformed(evt);
            }
        });

        jTextArea1.setColumns(20);
        jTextArea1.setRows(5);
        jScrollPane1.setViewportView(jTextArea1);

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addComponent(jToggleButton3, javax.swing.GroupLayout.PREFERRED_SIZE, 81, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(123, 523, Short.MAX_VALUE))
            .addComponent(jScrollPane1)
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addComponent(jToggleButton3)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(jScrollPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 392, Short.MAX_VALUE)
                .addContainerGap())
        );
    }// </editor-fold>//GEN-END:initComponents

    private void jToggleButton3ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jToggleButton3ActionPerformed
       if (jToggleButton3.isSelected()) {
            if (initListeners ()) {
                openFile();
            } else {
                jToggleButton3.setSelected(false);
                JOptionPane.showMessageDialog(null,"Not connected to Robot", "Error", JOptionPane.ERROR_MESSAGE);
            }
            
        } else {
            dispose();
        }
       
    }//GEN-LAST:event_jToggleButton3ActionPerformed

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.ButtonGroup buttonGroup1;
    private javax.swing.ButtonGroup buttonGroup2;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JTextArea jTextArea1;
    private javax.swing.JToggleButton jToggleButton3;
    // End of variables declaration//GEN-END:variables
class BallDataListener implements ObjectListener<byte[]>
{  
    private final Command getBallDataCommand;
    private final boolean topCam;

    BallDataListener (Command getBallDataCommand, boolean topCam) {
        this.getBallDataCommand = getBallDataCommand;
        this.topCam = topCam;
    }

    @Override
    public void errorOccured(String cause) {
        RobotHealth.Plugin.genericManagerFactory.getManager(getBallDataCommand).removeListener(this);
    }

    @Override
    public void newObjectReceived(byte[] object) 
    {
      try
      {
        Representations.BallPercept ballData = 
                Representations.BallPercept.parseFrom(object);
        Boolean ballGT;
        if (topCam) {
            ballGT = topBall.get(LogfilePlayer.getCurrentFrame());
        //    int currentFrame = LogfilePlayer.getCurrentFrame(); 
            if (ballGT==null){
                String msg = "Missing frame " +LogfilePlayer.getCurrentFrame() +" in Groundtruth\n";
                GroundTruthReader.this.jTextArea1.append(msg);
            } else if (ballGT != ballData.getBallWasSeen()){
                if (ballGT) {
                 //  GroundTruthReader.this.jTextArea1.set(Color.red);
                   GroundTruthReader.this.jTextArea1.append("[" +LogfilePlayer.getCurrentFrame()  +"] missed ball on topcamera\n");
                  // GroundTruthReader.this.jTextArea1.setForeground(Color.black);
                } else {
                  // GroundTruthReader.this.jTextArea1.setForeground(Color.red);
                   GroundTruthReader.this.jTextArea1.append("[" +LogfilePlayer.getCurrentFrame()  +"] false ball on topcamera\n");
                //   GroundTruthReader.this.jTextArea1.setForeground(Color.black);
                }
            }            
        } else {
            ballGT = bottomBall.get(LogfilePlayer.getCurrentFrame());
            if (ballGT==null){
                String msg = "Missing frame " +LogfilePlayer.getCurrentFrame() +" in Groundtruth\n";
                GroundTruthReader.this.jTextArea1.append(msg);
            } else if (ballGT != ballData.getBallWasSeen()){
                if (ballGT) {
                   GroundTruthReader.this.jTextArea1.setForeground(Color.red);
                   GroundTruthReader.this.jTextArea1.append("" +LogfilePlayer.getCurrentFrame()  +": missed ball on botttomcamera\n");
                   GroundTruthReader.this.jTextArea1.setForeground(Color.black);
                } else {
                   GroundTruthReader.this.jTextArea1.setForeground(Color.red);
                   GroundTruthReader.this.jTextArea1.append("" +LogfilePlayer.getCurrentFrame()  +": false ball on bottomcamera\n");
                   GroundTruthReader.this.jTextArea1.setForeground(Color.black);
                }
            }
           
        }           
      }
      catch (InvalidProtocolBufferException ex)
      {
        //ex.printStackTrace();
        Logger.getLogger(RobotHealth.class.getName()).log(Level.SEVERE, null, ex);
        errorOccured(ex.getMessage());
        String msg = new String(object);
        JOptionPane.showMessageDialog(null,msg, "Error", JOptionPane.ERROR_MESSAGE);
      }
    }//end newObjectReceived
    }//end class BallDataListener

class GoalDataListener implements ObjectListener<byte[]> {
    private final Command getGoalDataCommand;
    private final boolean topCam;
    
    GoalDataListener (Command getGoalDataCommand, boolean topCam) {
        this.getGoalDataCommand = getGoalDataCommand;
        this.topCam = topCam;
    }

    @Override
    public void errorOccured(String cause) {
        RobotHealth.Plugin.genericManagerFactory.getManager(getGoalDataCommand).removeListener(this);
    }

    @Override
    public void newObjectReceived(byte[] object) 
    {
      try
      {
        Representations.GoalPercept goalData = 
                Representations.GoalPercept.parseFrom(object);
        if (topCam) {
                      
                      
        } else {
                           
           
           
        }           
      }
      catch (InvalidProtocolBufferException ex)
      {
        //ex.printStackTrace();
        Logger.getLogger(RobotHealth.class.getName()).log(Level.SEVERE, null, ex);
        errorOccured(ex.getMessage());
        String msg = new String(object);
        JOptionPane.showMessageDialog(null,msg, "Error", JOptionPane.ERROR_MESSAGE);
      }
    }//end newObjectReceived
    
   
}//end class GoalDataListener

/*
class FrameInfoListener implements ObjectListener<byte[]>
{  
    private final Command getFrameInfoCommand; 
    public int currentFrame = 0;

    FrameInfoListener (Command getFrameInfoCommand) {
        this.getFrameInfoCommand = getFrameInfoCommand;      
    }

    @Override
    public void errorOccured(String cause) {
        RobotHealth.Plugin.genericManagerFactory.getManager(getFrameInfoCommand).removeListener(this);
    }

    @Override
    public void newObjectReceived(byte[] object) 
    {
      try
      {
        FrameInfo frameInfo = 
                FrameInfo.parseFrom(object);
        this.currentFrame = frameInfo.getFrameNumber();
               
      }
      catch (InvalidProtocolBufferException ex)
      {
        //ex.printStackTrace();
        Logger.getLogger(RobotHealth.class.getName()).log(Level.SEVERE, null, ex);
        errorOccured(ex.getMessage());
        String msg = new String(object);
        JOptionPane.showMessageDialog(null,msg, "Error", JOptionPane.ERROR_MESSAGE);
      }
    }//end newObjectReceived
}//end class FrameInfoListener
*/
 

  
}//end class GroundTruthCreator

