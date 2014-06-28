package de.naoth.rc.dialogs;

import com.google.protobuf.InvalidProtocolBufferException;
import de.naoth.rc.AbstractDialog;
import de.naoth.rc.DialogPlugin;
import de.naoth.rc.LogSimulator;
import de.naoth.rc.RobotControl;
import de.naoth.rc.manager.GenericManagerFactory;
import de.naoth.rc.messages.Representations;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.DefaultListModel;
import javax.swing.JOptionPane;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;
import org.apache.commons.collections4.map.LinkedMap;

/**
 *
 * @author Heinrich
 * @author Peter
 */
public class GroundTruthReader extends AbstractDialog {
    
     private Map<Integer, Boolean> topBall,
            bottomBall;
     private Map<Integer, Integer> topGoal,
            bottomGoal;
     DefaultListModel listModel;

    private class MissmatchSaver{
        int frame;
        Boolean topBall = null,
            bottomBall = null,
            missingFrame = false;
        Integer  topGoalGT = null,
            bottomGoalGT = null,
                topGoalSeen = null,
                bottomGoalSeen = null;        
        
        @Override
        public String toString() {
            String back = "[" +frame +"] ";
            if (missingFrame){
                back += "missingFrame ";
            }
            if (topBall!=null) {
                if (topBall) back += "missed topBall ";
                else back+= "false topBall ";
            }
            if (bottomBall!=null) {
                if (bottomBall) back += "missed bottomBall ";
                else back+= "false bottomBall ";
            }
            if (topGoalGT != null) {
                back += "Top goalPostsGT: " +topGoalGT +" goalPostsSeen: " +topGoalSeen +" ";
            }
            if (bottomGoalGT != null) {
                back += "Bottom goalPostsGT: " +bottomGoalGT +" goalPostsSeen: " +bottomGoalSeen +" ";
            }            
            return back;
        } 
    }
    Map<Integer,MissmatchSaver> missmatches;
   

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
  
    private final GroundTruthReader.LogPerceptListener logPerceptListener = new GroundTruthReader.LogPerceptListener();

    public GroundTruthReader() {
        listModel = new DefaultListModel();
        initComponents();
        LogSimulator.LogSimulatorManager.getInstance().addListener(logPerceptListener);
        
     // Plugin.genericManagerFactory.getManager(getBallDataCommand).addListener(ballListener);
        //Plugin.genericManagerFactory.getManager(getBallTopDataCommand).addListener(ballTopListener);

    }

     private void openFile() {
        if (!this.jToggleButton3.isSelected()) return;
        String fileName = LogfilePlayer.getFileName();
        if (fileName.equals("")) {
            JOptionPane.showMessageDialog(null, "No opend logfile", "Error", JOptionPane.ERROR_MESSAGE);
            this.jToggleButton3.setSelected(false);
            return;
        }
        fileName = fileName.substring(0, fileName.lastIndexOf(".") + 1) + "gts";
        ObjectInputStream o;
        try {

            o = new ObjectInputStream(new FileInputStream(fileName));
            topBall = (Map<Integer, Boolean>) o.readObject();
            bottomBall = (Map<Integer, Boolean>) o.readObject();
            topGoal = (Map<Integer, Integer>) o.readObject();
            bottomGoal = (Map<Integer, Integer>) o.readObject();

        } catch (IOException ex) {
            JOptionPane.showMessageDialog(null, "No Groundtruth for this logfile", "Error", JOptionPane.ERROR_MESSAGE);
            this.jToggleButton3.setSelected(false);
        } catch (ClassNotFoundException ex) {
            Logger.getLogger(GroundTruthReader.class.getName()).log(Level.SEVERE, null, ex);
        }
        missmatches = new LinkedMap<>();
    }

    /**
     * This method is called from within the constructor to initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is always
     * regenerated by the Form Editor.
     */
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        buttonGroup1 = new javax.swing.ButtonGroup();
        buttonGroup2 = new javax.swing.ButtonGroup();
        jToggleButton3 = new javax.swing.JToggleButton();
        jScrollPane2 = new javax.swing.JScrollPane();
        jList1 = new javax.swing.JList();

        jToggleButton3.setText("listen");
        jToggleButton3.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jToggleButton3ActionPerformed(evt);
            }
        });

        jList1.setModel(listModel);
        jScrollPane2.setViewportView(jList1);

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addComponent(jToggleButton3, javax.swing.GroupLayout.PREFERRED_SIZE, 81, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(jScrollPane2, javax.swing.GroupLayout.DEFAULT_SIZE, 584, Short.MAX_VALUE)
                .addContainerGap())
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addComponent(jToggleButton3)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jScrollPane2, javax.swing.GroupLayout.PREFERRED_SIZE, 362, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap(46, Short.MAX_VALUE))
        );
    }// </editor-fold>//GEN-END:initComponents

    private void jToggleButton3ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jToggleButton3ActionPerformed
        if (jToggleButton3.isSelected()) {           
                openFile();
        }
    }//GEN-LAST:event_jToggleButton3ActionPerformed

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.ButtonGroup buttonGroup1;
    private javax.swing.ButtonGroup buttonGroup2;
    private javax.swing.JList jList1;
    private javax.swing.JScrollPane jScrollPane2;
    private javax.swing.JToggleButton jToggleButton3;
    // End of variables declaration//GEN-END:variables

    class LogPerceptListener implements LogSimulator.LogSimulatorActionListener {

        @Override
        public void frameChanged(LogSimulator.BlackBoard b, int frameNumber) {            
            if (!GroundTruthReader.this.jToggleButton3.isSelected()) {
                return;
            }
            MissmatchSaver missmatch = new MissmatchSaver();
            boolean foundMissmatch = false;
            try {               
                /**
                 * ************BallPercept*****************************************************
                 */
                byte[]data = b.getRepresentation("BallPercept");
                Representations.BallPercept ballPercept = Representations.BallPercept.parseFrom(data);
                Boolean ballHere = bottomBall.get(frameNumber); 
                missmatch.frame = frameNumber;
                if (ballHere == null) {
                    missmatch.missingFrame = true;
                    foundMissmatch=true;                    
                } else {
                    if (ballHere != ballPercept.getBallWasSeen()) {
                        if (ballHere) {
                            missmatch.bottomBall = true;
                            foundMissmatch=true;  
                        } else {
                            missmatch.bottomBall = false;
                            foundMissmatch=true;
                        }
                    }
                }
                /**
                 * ************BallPercept*****************************************************
                 */
                /**
                 * ************BallPerceptTop**************************************************
                 */
                data = b.getRepresentation("BallPerceptTop");
                ballPercept = Representations.BallPercept.parseFrom(data);
                ballHere = topBall.get(frameNumber);
                if (ballHere == null) {
                    missmatch.missingFrame = true;
                    foundMissmatch=true; 
                } else {
                    if (ballHere != ballPercept.getBallWasSeen()) {
                        if (ballHere) {
                            missmatch.topBall = true;
                            foundMissmatch=true;
                        } else {
                            missmatch.topBall = false;
                            foundMissmatch=true;
                        }
                    }
                }
                /**
                 * ************BallPerceptTop**************************************************
                 */
                /**
                 * ************GoalPercept**************************************************
                 */
                data = b.getRepresentation("GoalPercept");
                Representations.GoalPercept goalPercept = Representations.GoalPercept.parseFrom(data);
                int postCount = goalPercept.getPostCount();
                Integer goalHere = bottomGoal.get(frameNumber);
                if (goalHere == null) {
                    missmatch.missingFrame = true;
                    foundMissmatch=true; 
                }
                if (goalHere != postCount) {
                    missmatch.bottomGoalGT = goalHere;
                    missmatch.bottomGoalSeen = postCount;
                    foundMissmatch=true;
                } 
                /**
                 * ************GoalPercept**************************************************
                 */
                /**
                 * ************GoalPerceptTop***********************************************
                 */
                data = b.getRepresentation("GoalPerceptTop");
                Representations.GoalPercept goalPerceptTop = Representations.GoalPercept.parseFrom(data);
                postCount = goalPerceptTop.getPostCount();
                goalHere = topGoal.get(frameNumber);
                if (goalHere == null) {
                    missmatch.missingFrame = true;
                    foundMissmatch=true; 
                }
                if (goalHere != postCount) {
                    missmatch.topGoalGT = goalHere;
                    missmatch.topGoalSeen = postCount;
                    foundMissmatch=true;
                }
                /**
                 * ************GoalPerceptTop***********************************************
                 */

            } catch (InvalidProtocolBufferException ex) {
                ex.printStackTrace(System.err);
            }
            if (foundMissmatch) {
                missmatches.put(missmatch.frame, missmatch);               
                listModel.addElement(missmatch);
            }
        }

        @Override
        public void logfileOpened(LogSimulator.BlackBoard b, String path) {
            GroundTruthReader.this.openFile();
        }
    }

}//end class GroundTruthCreator

