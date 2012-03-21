/*
 * 
 */

package de.naoth.me.core;

import java.util.ArrayList;

/**
 *
 * @author Heinrich Mellmann
 */
public class JointDefaultConfiguration extends JointPrototypeConfiguration
{

    public JointDefaultConfiguration()
    {
        ArrayList<JointPrototype> JointPrototypeList = new ArrayList<JointPrototype>();
        JointPrototype headYaw = new JointPrototype("HeadYaw", -120, 120, 0);
        JointPrototype headPitch = new JointPrototype("HeadPitch", -40, 40, 0);
        
        JointPrototype rShoulderPitch = new JointPrototype("RShoulderPitch", -120, 120, 119);
        JointPrototype lShoulderPitch = new JointPrototype("LShoulderPitch", -120, 120, 119);
        JointPrototype rShoulderRoll = new JointPrototype("RShoulderRoll", -95, 0, 0);
        JointPrototype lShoulderRoll = new JointPrototype("LShoulderRoll", 0, 95, 0);
        
        JointPrototype rElbowYaw = new JointPrototype("RElbowYaw", -90, 120, 119);
        JointPrototype lElbowYaw = new JointPrototype("LElbowYaw", -120, 90, -119);
        JointPrototype rElbowRoll = new JointPrototype("RElbowRoll", 0, 90, 89);
        JointPrototype lElbowRoll = new JointPrototype("LElbowRoll", -90, 0, -89);
        
        JointPrototype rHipYawPitch = new JointPrototype("RHipYawPitch", -65, 0, 0);
        JointPrototype rHipPitch = new JointPrototype("RHipPitch", -90, 25, -31.5);
        JointPrototype rHipRoll = new JointPrototype("RHipRoll", -45, 21, 0);
        JointPrototype rKneePitch = new JointPrototype("RKneePitch", -5, 120, 63);
        JointPrototype rAnklePitch = new JointPrototype("RAnklePitch", -65, 45, -31.5);
        JointPrototype rAnkleRoll = new JointPrototype("RAnkleRoll", -20, 25, 0);
        
        JointPrototype lHipYawPitch = new JointPrototype("LHipYawPitch", -65, 0, 0);
        JointPrototype lHipPitch = new JointPrototype("LHipPitch", -90, 25, -31.5);
        JointPrototype lHipRoll = new JointPrototype("LHipRoll", -21, 45, 0);
        JointPrototype lKneePitch = new JointPrototype("LKneePitch", -5, 120, 63);
        JointPrototype lAnklePitch = new JointPrototype("LAnklePitch", -65, 45, -31.5);
        JointPrototype lAnkleRoll = new JointPrototype("LAnkleRoll", -25, 20, 0);

        JointPrototypeList.add(headYaw);
        JointPrototypeList.add(headPitch);
        
        JointPrototypeList.add(rShoulderPitch);
        JointPrototypeList.add(lShoulderPitch);
        JointPrototypeList.add(rShoulderRoll);
        JointPrototypeList.add(lShoulderRoll);
        
        JointPrototypeList.add(rElbowYaw);
        JointPrototypeList.add(lElbowYaw);
        JointPrototypeList.add(rElbowRoll);
        JointPrototypeList.add(lElbowRoll);
        
        JointPrototypeList.add(rHipYawPitch);
        JointPrototypeList.add(rHipPitch);
        JointPrototypeList.add(rHipRoll);
        JointPrototypeList.add(rKneePitch);
        JointPrototypeList.add(rAnklePitch);
        JointPrototypeList.add(rAnkleRoll);
        
        JointPrototypeList.add(lHipYawPitch);
        JointPrototypeList.add(lHipPitch);
        JointPrototypeList.add(lHipRoll);
        JointPrototypeList.add(lKneePitch);
        JointPrototypeList.add(lAnklePitch);
        JointPrototypeList.add(lAnkleRoll);
        this.jointValuePrototypeList = JointPrototypeList;
    }//end loadFromFile


    
}//end ConfigurationLoader
