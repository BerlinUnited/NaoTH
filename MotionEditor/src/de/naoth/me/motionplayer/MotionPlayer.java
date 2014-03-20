package de.naoth.me.motionplayer;

import com.google.protobuf.InvalidProtocolBufferException;
import de.naoth.me.controls.motionneteditor.MotionNetEditorPanel;
import de.naoth.me.core.Joint;
import de.naoth.me.core.KeyFrame;
import de.naoth.me.core.MotionNet;
import de.naoth.me.core.Transition;
//import de.naoth.rc.messages.Messages.CMDArg;
//import de.naoth.rc.messages.Messages.CMD;
import de.naoth.rc.messages.CommonTypes.JointID;
import de.naoth.rc.messages.FrameworkRepresentations.JointData;
import de.naoth.rc.messages.FrameworkRepresentations.SensorJointData;
import de.naoth.rc.server.Command;
import de.naoth.rc.server.CommandSender;

import de.naoth.rc.server.MessageServer;

/*
 * MotionPlayer.java
 *
 * Created on 12. Maerz 2008, 18:52
 */
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.Timer;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.PrintStream;
import javax.swing.JOptionPane;
/**
 *
 * @author  Heinrich Mellmann
 */
public class MotionPlayer extends javax.swing.JPanel implements PropertyChangeListener
{

  private MessageServer messageServer;
  private MotionNet currentMotionNet;
  private String currentCondition = "*";
  private Transition transition;
  private KeyFrame fromKey;
  private KeyFrame toKey;
  private Double duration;
  private boolean run;
  private Double time;
  private KeyFrame currentKeyFrame = null;
  public MotionNetEditorPanel motionNetEditorPanel;
  private final String getRepresentationBase = "representation:getbinary";
  private final String representationName = "SensorJointData";
  private final String sendMotionNetToRobotCommandBase = "file::write";
  private final String motionNetPath = "Config/motionnet/play_editor_motionnet.mef";
  private Timer getJointsTimer;
  /** Creates new form MotionPlayer */

  private PrintStream outStream = System.out;

  public MotionPlayer()
  {
    initComponents();

    int delay = 200; //milliseconds
    ActionListener taskPerformer = new ActionListener()
    {
      public void actionPerformed(ActionEvent evt)
      {
        Command cmd = new Command(getRepresentationBase).addArg(representationName);
        sendCommand(cmd);
      }
    };
    getJointsTimer = new Timer(delay, taskPerformer);
    this.messageServer = null;
    this.currentMotionNet = null;
  }

  public void setMotionNet(MotionNet motionNet)
  {
    if(motionNet == null)
    {
      return;
    }

    for(KeyFrame keyFrame : motionNet.getKeyFrameMap().values())
    {
      for(Transition transition_ : keyFrame.getTransitions())
      {
        addConditionItem(transition_.getCondition());
      }
    }//end for

    this.currentMotionNet = motionNet;
  }//end setMotionNet

  private void addConditionItem(String condition)
  {
    for(int i = 0; i < this.jComboBoxConditionSet.getItemCount(); i++)
    {
      if(this.jComboBoxConditionSet.getItemAt(i).equals(condition))
      {
        return;
      }
    }

    this.jComboBoxConditionSet.addItem(condition);
  }//end addConditionItem

  private void play()
  {
    if(run)
    {
      return;
    }

    time = 0.0;
    run = true;
    fromKey = this.currentMotionNet.getKeyFrame(0);
    transition = fromKey.getTransition(currentCondition);
    if(transition == null)
    {
      return;
    }
    duration = transition.getDuration();
    toKey = transition.getToKeyFrame();

    new Thread(new Runnable()
    {

      public void run()
      {
        try
        {
          while(run)
          {
            nextstep();
            Thread.sleep(40);
            //System.out.println("test");
          }
        }
        catch(InterruptedException e)
        {
        }
      }
    }).start();
  }

  private void nextstep()
  {
    if(time > 1.0)
    {
      fromKey = toKey;
      transition = fromKey.getTransition(currentCondition);
      if(transition == null)
      {
        transition = fromKey.getTransition(Transition.DEFAULT_CONDITION);
      }

      if(transition == null)
      {
        this.run = false;
        return;
      }
      duration = transition.getDuration();
      toKey = transition.getToKeyFrame();
      time -= 1.0;
    }

    for(int m = 0; m < 22; m++)
    {
      Joint joint = toKey.getJointValue(m);
      double value = (1.0 - time) * fromKey.getJointValue(m).getValue() +
        time * toKey.getJointValue(m).getValue();

      Command cmd = new Command("motion:set").addArg(joint.getId(), String.valueOf(value));

      sendCommand(cmd);
    }

    if(motionNetEditorPanel != null)
    {
      double x = (1.0 - time) * (double) (fromKey.getX() + 15.0) + time * (double) (toKey.getX() + 15.0);
      double y = (1.0 - time) * (double) (fromKey.getY() + 15.0) + time * (double) (toKey.getY() + 15.0);
      motionNetEditorPanel.setMarker((int) x, (int) y);
    }

    time = time + 40.0 / duration;
  }

  public void propertyChange(PropertyChangeEvent event)
  {
    if
    (
      event.getPropertyName().compareTo("selectedKeyFrame") == 0 &&
      event.getNewValue() instanceof KeyFrame
    )
    {
     currentKeyFrame = (KeyFrame) event.getNewValue();
    }
    if(jCheckBoxSendDirectChange.isSelected())
    {
      Command cmdPlayOff = new Command("MotionBehaviorControl:motion:play_editor_motionnet").addArg("off");

      Command cmdForceOn = new Command("MotionBehaviorControl:motion:force_joints").addArg("on");

      sendCommand(cmdPlayOff);
      sendCommand(cmdForceOn);

      if
      (
        event.getPropertyName().compareTo("selectedKeyFrame") == 0 &&
        event.getNewValue() instanceof KeyFrame
      )
      {
       currentKeyFrame = (KeyFrame) event.getNewValue();

       Command cmdMotionSetBuilder = new Command("motion:set");

       for(Joint joint : currentKeyFrame.getJointConfiguration().getJointVector())
       {
         if(jCheckBoxSendDirectChange.isSelected())
         {
           cmdMotionSetBuilder.addArg(joint.getId(), String.valueOf(joint.getRadiant()));
         }
       }

       sendCommand(cmdMotionSetBuilder);
      }
    }

    if(event.getPropertyName().compareTo("jointValueChanged") == 0)
    {
//      Joint joint = (Joint) event.getNewValue();
//      if(jCheckBoxSendDirectChange.isSelected())
//      {
//        Command cmd = Command.newBuilder().setName("motion:set").addArgs(CMDArg.newBuilder().setName(joint.getId()).setValue("" + joint.getRadiant())).build();
//        sendCommand(cmd);
//        System.out.println("motion:set " + joint.getId() + "=" + joint.getRadiant());
//      }
      if(jCheckBoxSendDirectChange.isSelected())
      {
        Command cmdMotionSetBuilder = new Command("motion:set");

        Joint newJoint = (Joint) event.getNewValue();
        for(Joint joint : currentKeyFrame.getJointConfiguration().getJointVector())
        {
          if(joint.getId().equals(newJoint.getId()))
          {
            joint = newJoint;
          }
          if(jCheckBoxSendDirectChange.isSelected())
          {
            cmdMotionSetBuilder.addArg(joint.getId(), String.valueOf(joint.getRadiant()));
            //System.out.println("motion:set " + joint.getId() + "=" + joint.getRadiant());
          }
        }
        sendCommand(cmdMotionSetBuilder);
      }
    }
  }//end propertyChange

  /** This method is called from within the constructor to
   * initialize the form.
   * WARNING: Do NOT modify this code. The content of this method is
   * always regenerated by the Form Editor.
   */
  // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
  private void initComponents() {

    jPanelPlayer = new javax.swing.JPanel();
    jComboBoxConditionSet = new javax.swing.JComboBox();
    jButtonPlay = new javax.swing.JButton();
    jButtonStop = new javax.swing.JButton();
    jButtonReadSensorData = new javax.swing.JButton();
    jCheckBoxSetStiffness = new javax.swing.JCheckBox();
    jCheckBoxSendDirectChange = new javax.swing.JCheckBox();
    jCheckBoxEnableKneadMode = new javax.swing.JCheckBox();
    jCheckBoxEnableKneadModeSensorUpdate = new javax.swing.JCheckBox();

    jPanelPlayer.setBackground(new java.awt.Color(204, 204, 255));

    jComboBoxConditionSet.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "*" }));
    jComboBoxConditionSet.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        jComboBoxConditionSetActionPerformed(evt);
      }
    });

    jButtonPlay.setText("Play");
    jButtonPlay.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        jButtonPlayActionPerformed(evt);
      }
    });

    jButtonStop.setText("Stop");
    jButtonStop.addMouseListener(new java.awt.event.MouseAdapter() {
      public void mouseReleased(java.awt.event.MouseEvent evt) {
        jButtonStopMouseReleased(evt);
      }
    });
    jButtonStop.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        jButtonStopActionPerformed(evt);
      }
    });

    jButtonReadSensorData.setText("Read Sensor Data");
    jButtonReadSensorData.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        jButtonReadSensorDataActionPerformed(evt);
      }
    });

    jCheckBoxSetStiffness.setText("Set Stiffness");
    jCheckBoxSetStiffness.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        jCheckBoxSetStiffnessActionPerformed(evt);
      }
    });

    jCheckBoxSendDirectChange.setText("directly send joint changes");
    jCheckBoxSendDirectChange.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        jCheckBoxSendDirectChangeActionPerformed(evt);
      }
    });

    jCheckBoxEnableKneadMode.setText("enable knead mode");
    jCheckBoxEnableKneadMode.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        jCheckBoxEnableKneadModeActionPerformed(evt);
      }
    });

    jCheckBoxEnableKneadModeSensorUpdate.setText("and read sensor data");
    jCheckBoxEnableKneadModeSensorUpdate.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        jCheckBoxEnableKneadModeSensorUpdateActionPerformed(evt);
      }
    });

    javax.swing.GroupLayout jPanelPlayerLayout = new javax.swing.GroupLayout(jPanelPlayer);
    jPanelPlayer.setLayout(jPanelPlayerLayout);
    jPanelPlayerLayout.setHorizontalGroup(
      jPanelPlayerLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(jPanelPlayerLayout.createSequentialGroup()
        .addContainerGap()
        .addGroup(jPanelPlayerLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
          .addGroup(jPanelPlayerLayout.createSequentialGroup()
            .addComponent(jCheckBoxEnableKneadMode)
            .addGap(18, 18, 18)
            .addComponent(jCheckBoxEnableKneadModeSensorUpdate)
            .addGap(18, 18, 18)
            .addComponent(jCheckBoxSendDirectChange)
            .addGap(18, 18, 18)
            .addComponent(jCheckBoxSetStiffness))
          .addGroup(jPanelPlayerLayout.createSequentialGroup()
            .addComponent(jComboBoxConditionSet, javax.swing.GroupLayout.PREFERRED_SIZE, 134, javax.swing.GroupLayout.PREFERRED_SIZE)
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
            .addComponent(jButtonPlay)
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
            .addComponent(jButtonStop)
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
            .addComponent(jButtonReadSensorData)))
        .addContainerGap(25, Short.MAX_VALUE))
    );
    jPanelPlayerLayout.setVerticalGroup(
      jPanelPlayerLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(jPanelPlayerLayout.createSequentialGroup()
        .addContainerGap()
        .addGroup(jPanelPlayerLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
          .addComponent(jComboBoxConditionSet, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
          .addComponent(jButtonPlay)
          .addComponent(jButtonStop)
          .addComponent(jButtonReadSensorData))
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
        .addGroup(jPanelPlayerLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
          .addComponent(jCheckBoxEnableKneadMode)
          .addComponent(jCheckBoxEnableKneadModeSensorUpdate)
          .addComponent(jCheckBoxSendDirectChange)
          .addComponent(jCheckBoxSetStiffness))
        .addGap(17, 17, 17))
    );

    javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
    this.setLayout(layout);
    layout.setHorizontalGroup(
      layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addComponent(jPanelPlayer, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
    );
    layout.setVerticalGroup(
      layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addComponent(jPanelPlayer, javax.swing.GroupLayout.PREFERRED_SIZE, 85, javax.swing.GroupLayout.PREFERRED_SIZE)
    );
  }// </editor-fold>//GEN-END:initComponents

    private void jComboBoxConditionSetActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jComboBoxConditionSetActionPerformed
      currentCondition = (String) jComboBoxConditionSet.getSelectedItem();
    }//GEN-LAST:event_jComboBoxConditionSetActionPerformed

    private void jButtonStopMouseReleased(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_jButtonStopMouseReleased
      run = false;
    }//GEN-LAST:event_jButtonStopMouseReleased

    private void jButtonReadSensorDataActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButtonReadSensorDataActionPerformed
      Command cmd = new Command(getRepresentationBase).addArg(representationName);
      sendCommand(cmd);
    }//GEN-LAST:event_jButtonReadSensorDataActionPerformed

    private void jCheckBoxSetStiffnessActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jCheckBoxSetStiffnessActionPerformed

      Command cmdStiffnessBuilder = new Command("stiffness");

      if(jCheckBoxSetStiffness.isSelected())
      {
        cmdStiffnessBuilder.addArg("on");
      }
      else
      {
        cmdStiffnessBuilder.addArg("off");
      }
      sendCommand(cmdStiffnessBuilder);
    }//GEN-LAST:event_jCheckBoxSetStiffnessActionPerformed

    private void jButtonPlayActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButtonPlayActionPerformed
      //play();

      // enable or disable the knead mode
      Command cmdKneadModeOff = new Command("MotionBehaviorControl:motion:knead_mode").addArg("off");
      sendCommand(cmdKneadModeOff);

      // enable or disable force joints mode
      Command cmdForceJointsOff = new Command("MotionBehaviorControl:motion:force_joints").addArg("off");
      sendCommand(cmdForceJointsOff);

      // send the motionnet to the robot
      Command cmdSendMotionNet = new Command(sendMotionNetToRobotCommandBase);
      cmdSendMotionNet.addArg("path", motionNetPath);
      cmdSendMotionNet.addArg("content", this.currentMotionNet.toString());
      sendCommand(cmdSendMotionNet);

      // reload the motion-net which has been sent before
      Command cmdLoadEditor = new Command("motion:load_editor_motionnet").addArg("condition", currentCondition);
      sendCommand(cmdLoadEditor);

      // play the motion
      Command cmdPlayOn = new Command("MotionBehaviorControl:motion:play_editor_motionnet").addArg("on");
      sendCommand(cmdPlayOn);
      
      //TODO: rueckmeldung behandeln
    }//GEN-LAST:event_jButtonPlayActionPerformed

    private void jButtonStopActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButtonStopActionPerformed

      Command cmdPlayOff = new Command("MotionBehaviorControl:motion:play_editor_motionnet").addArg("off");
      sendCommand(cmdPlayOff);

      Command cmdForceJointsOff = new Command("MotionBehaviorControl:motion:force_joints").addArg("off");
      sendCommand(cmdForceJointsOff);

      if(jCheckBoxEnableKneadMode.isSelected())
      {
        Command cmdKneadModeOn = new Command("MotionBehaviorControl:motion:knead_mode").addArg("on");
        sendCommand(cmdKneadModeOn);
      }
      else
      {
        Command cmdStandOn = new Command("MotionBehaviorControl:motion:stand").addArg("on");
        sendCommand(cmdStandOn);
      }
      //sendCommand("MotionBehaviorControl:motion:play_editor_motionnet off");
      // TODO add your handling code here:
    }//GEN-LAST:event_jButtonStopActionPerformed

    private void jCheckBoxSendDirectChangeActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jCheckBoxSendDirectChangeActionPerformed
      if(jCheckBoxSendDirectChange.isSelected())
      {
        Command cmdPlayOff = new Command("MotionBehaviorControl:motion:play_editor_motionnet").addArg("off");
        Command cmdForceOn = new Command("MotionBehaviorControl:motion:force_joints").addArg("on");

        sendCommand(cmdPlayOff);
        sendCommand(cmdForceOn);

        if(currentKeyFrame != null)
        {
          Command cmdMotionSetBuilder = new Command("motion:set");

          for(Joint joint : currentKeyFrame.getJointConfiguration().getJointVector())
          {
            if(jCheckBoxSendDirectChange.isSelected())
            {
              cmdMotionSetBuilder.addArg(joint.getId(), String.valueOf(joint.getRadiant()));
            }
          }//end for

          sendCommand(cmdMotionSetBuilder);
        }
      }
      else
      {
        Command cmdForceOff = new Command("MotionBehaviorControl:motion:force_joints").addArg("off");

        sendCommand(cmdForceOff);
      }
}//GEN-LAST:event_jCheckBoxSendDirectChangeActionPerformed

    private void jCheckBoxEnableKneadModeActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jCheckBoxEnableKneadModeActionPerformed
      if(jCheckBoxEnableKneadMode.isSelected())
      {
        Command cmdKneadModeOn = new Command("MotionBehaviorControl:motion:knead_mode").addArg("on");
        sendCommand(cmdKneadModeOn);
        if(jCheckBoxEnableKneadModeSensorUpdate.isSelected())
        {
          getJointsTimer.start();
        }
        else if(getJointsTimer.isRunning())
        {
          getJointsTimer.stop();
        }
      }
      else
      {
        if(getJointsTimer.isRunning())
        {
          getJointsTimer.stop();
        }
        Command cmdKneadModeOff = new Command("MotionBehaviorControl:motion:knead_mode").addArg("off");
        sendCommand(cmdKneadModeOff);
        Command cmdStandOn = new Command("MotionBehaviorControl:motion:stand").addArg("on");
        sendCommand(cmdStandOn);
      }
    }//GEN-LAST:event_jCheckBoxEnableKneadModeActionPerformed

    private void jCheckBoxEnableKneadModeSensorUpdateActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jCheckBoxEnableKneadModeSensorUpdateActionPerformed
      if(jCheckBoxEnableKneadMode.isSelected() && jCheckBoxEnableKneadModeSensorUpdate.isSelected())
      {
        getJointsTimer.start();
      }
      else if(getJointsTimer.isRunning())
      {
        getJointsTimer.stop();
      }
    }//GEN-LAST:event_jCheckBoxEnableKneadModeSensorUpdateActionPerformed

  final MotionPlayer thisFinal = this;
  private void sendCommand(Command command)
  {
    if(this.messageServer == null)
    {
      return;
    }

    final Command commandToExecute = command;
    this.messageServer.executeSingleCommand(new CommandSender()
    {

      public void handleResponse(byte[] result, Command originalCommand)
      {

        if(commandToExecute.getName().compareTo(getRepresentationBase) != 0 || currentKeyFrame == null)
        {
          String msg = new String(result);
          /*
          JOptionPane.showMessageDialog(thisFinal,
            msg, originalCommand.getName(), JOptionPane.ERROR_MESSAGE);
           * */
          outStream.println(originalCommand.getName() + " : " + msg);
          return;
        }

        try
        {
          SensorJointData data = SensorJointData.parseFrom(result);

          JointData joints = data.getJointData();

          for(int i=0; i < joints.getPositionCount(); i++)
          {
            String jointName = JointID.valueOf(i).name();
            double minValue = currentKeyFrame.getJointValue(jointName).getMinValue();
            double maxValue = currentKeyFrame.getJointValue(jointName).getMaxValue();
            double currentValue = joints.getPosition(i);

            if(!(minValue <= currentValue && currentValue <= maxValue))
            {
//              JOptionPane.showMessageDialog(null,
//                "Error : Wert ausserhalb der Grenzen: " + jointName + " " + minValue + " <= " + currentValue + " <= " + maxValue,
//                "Error", JOptionPane.ERROR_MESSAGE);
              if(currentValue < minValue)
              {
                currentValue = minValue;
              }
              if(currentValue > maxValue)
              {
                currentValue = maxValue;
              }
            }
            currentKeyFrame.getJointValue(jointName).setRadiant(currentValue);
          }
        }
        catch(InvalidProtocolBufferException ex)
        {
          Logger.getLogger(MotionPlayer.class.getName()).log(Level.SEVERE, null, ex);
        }
      }//end handleResponse

      public void handleError(int code)
      {
        JOptionPane.showMessageDialog(thisFinal,
              "Error occured, code " + code, "ERROR", JOptionPane.ERROR_MESSAGE);
        outStream.println("Error occured, code " + code);
      }

      public Command getCurrentCommand()
      {
        return commandToExecute;
      }
    }, command);
  }
  // Variables declaration - do not modify//GEN-BEGIN:variables
  private javax.swing.JButton jButtonPlay;
  private javax.swing.JButton jButtonReadSensorData;
  private javax.swing.JButton jButtonStop;
  private javax.swing.JCheckBox jCheckBoxEnableKneadMode;
  private javax.swing.JCheckBox jCheckBoxEnableKneadModeSensorUpdate;
  private javax.swing.JCheckBox jCheckBoxSendDirectChange;
  private javax.swing.JCheckBox jCheckBoxSetStiffness;
  private javax.swing.JComboBox jComboBoxConditionSet;
  private javax.swing.JPanel jPanelPlayer;
  // End of variables declaration//GEN-END:variables

  public void setMessageServer(MessageServer messageServer)
  {
    this.messageServer = messageServer;
  }
}
