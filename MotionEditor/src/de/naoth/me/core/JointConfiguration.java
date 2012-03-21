/*
 * 
 */

package de.naoth.me.core;

import java.util.HashMap;
import java.util.Vector;

/**
 *
 * @author Heinrich Mellmann
 */
public class JointConfiguration extends JointPrototypeConfiguration
{
    private HashMap<String,Joint> jointMap;
    private Vector<Joint> jointValueVector;
    
    public JointConfiguration(JointPrototypeConfiguration prototypeConfiguration)
    {
        this.jointMap = new HashMap<String,Joint>(prototypeConfiguration.jointValuePrototypeList.size());
        this.jointValueVector = new Vector<Joint>(prototypeConfiguration.jointValuePrototypeList.size());
        for(JointPrototype prototype: prototypeConfiguration.jointValuePrototypeList)
        {
            Joint jointValue = new Joint(prototype);
            jointValueVector.add(jointValue);
            jointMap.put(prototype.getId(),jointValue);
        }
    }
    
    public void setJointValue(String JointName, double Value)
    {
      Joint jointValue = getJoint(JointName);
      int index = jointValueVector.indexOf(jointValue);
      jointValue.setValue(Value);
      jointValue.setValid(true);
      jointMap.put(JointName, jointValue);
      jointValueVector.set(index, jointValue);
    }

    public Joint getJoint(String id)
    {
        // TODO: NoSuchJointValueException...
        return this.jointMap.get(id);
    }//end getJointValue

    public Joint getJoint(int index)
    {
        // TODO: NoSuchJointValueException...
        return this.jointValueVector.get(index);
    }//end getJointValue
    
    public HashMap<String,Joint> getJointMap()
    {
        return this.jointMap;       
    }
    
    public Vector<Joint> getJointVector()
    {
        return this.jointValueVector;       
    }
}//end class JointConfiguration
