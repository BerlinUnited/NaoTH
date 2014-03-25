package de.naoth.me.core;

import java.util.ArrayList;
import java.util.List;

/**
 *
 * @author Heinrich Mellmann
 */
public class JointPrototypeConfiguration 
{
    public final ArrayList<JointPrototype> jointValuePrototypeList = new ArrayList<JointPrototype>();
    
    public JointPrototypeConfiguration()
    {
    
    }

    protected void addJointValuePrototype(JointPrototype prototype)
    {
        this.jointValuePrototypeList.add(prototype);
    }

    @Override
    public String toString()
    {
      String result = "";
      for(JointPrototype joint: jointValuePrototypeList)
      {
        if(joint.isValid())
        {
          result += joint.getId() + ";\n";
        }
      }
      return result;
    }//end toString
    
}//end class JointPrototypeConfiguration
