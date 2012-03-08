package de.naoth.me.core;

import java.util.ArrayList;
//import java.util.HashMap;

/**
 *
 * @author Heinrich Mellmann
 */
public class KeyFrame extends MotionNet.Node<Transition>
{
    public static final int INVALID_KEYFRAME_ID = -1;
    
    private int id;
    private int x;
    private int y;
    
    // outgoing transition accesible by condition
    //private final HashMap<String,Transition> transitions;
    private final ArrayList<Transition> transitions;

    // the configuration contains the joint-data of this keyFrame
    private final JointConfiguration configuration;

    
    public KeyFrame(JointPrototypeConfiguration configuration, int id, int x, int y)
    {
        this.id = id;
        this.x = x;
        this.y = y;
        this.transitions = new ArrayList<Transition>();
        this.configuration = new JointConfiguration(configuration);
    }
    
    public void setJointValue(String JointName, double Value)
    {
     this.configuration.setJointValue(JointName, Value);
    }

    public Joint getJointValue(String id)
    {
        return this.configuration.getJoint(id);
    }//end setJointValue
    
    public Joint getJointValue(int index)
    {
        return this.configuration.getJoint(index);
    }//end setJointValue
    
    // get the first transition with corresponding condition
    public Transition getTransition(String condition) {
        //return transitions.get(condition);
        for(Transition transition: this.transitions)
        {
            if(transition.getCondition().compareTo(condition) == 0)
                return transition;
        }
        return null;
    }//end getTransition
    
    public int getId()
    {
        return id;
    }

    public void setId(int id)
    {
        this.id = id;
    }

    public int getX()
    {
        return x;
    }

    public void setX(int x)
    {
        this.x = x;
    }

    public int getY()
    {
        return y;
    }

    public void setY(int y)
    {
        this.y = y;
    }
    
    public ArrayList<Transition> getTransitions()
    {
        return transitions;
    }//end getTransitions
    
    public JointConfiguration getJointConfiguration()
    {
        return this.configuration;
    }//end getJointConfiguration
    
    
    
    protected void addOutEdge(Transition transition)
    {
        this.transitions.add(transition);
    }
    
    protected void removeOutEdge(Transition transition)
    {
      this.transitions.remove(transition);
    }
    
    @Override
    public String toString()
    {
      String result = "keyframe;" + id + ";" + x + ";" + y;
      for(Joint joint: configuration.getJointVector())
      {
        if(joint.isValid())
        {
          result += ";" + joint.getValue();
        }
      }
      return result;
    }//end toString
    
    public String toTxt()
    {
    String JointID[]= {"HeadPitch","HeadYaw",
    "RShoulderPitch",
    "LShoulderPitch",
    "RShoulderRoll",
    "LShoulderRoll",  
    "RElbowRoll",
    "LElbowRoll",
    "RElbowYaw",
    "LElbowYaw",
    "RHipYawPitch",
    "LHipYawPitch",
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
      String result =  "";
      
      for(int i = 0;i<22; i++){
          result += " " + this.getJointValue(JointID[i]).getValue();
      }
      
      //for(Joint joint: configuration.getJointVector())
     // {
      //  if(joint.isValid())
      //  {
            
       //   result += " " + joint.getValue();
      //  }
     // }
      return result;
    }//end toTxt
    
    public void fromString(String keyFrameString)
    {
        if(keyFrameString != null)
        {
          String[] token = keyFrameString.split(";");
          int tokenIndex = 4;
          for(Joint joint: this.configuration.getJointVector())
          {
            if(joint.isValid())
            {
              joint.setValue(Double.parseDouble(token[tokenIndex++]));
            }
          }
        }
    }//end toString
}//end class KeyFrame
