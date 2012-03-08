/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.naoth.me.core;

import java.io.*;
import javax.swing.JOptionPane;

//import de.hu_berlin.informatik.ki.motioneditor.core.Scanner;

/**
 *
 * @author claas
 */
public class MotionNetImport
{

  private Scanner scanner;
  private String currentToken;
  private int numberOfJoints;
  //private JointPrototypeConfiguration jointPrototypeConfiguration;
  private JointPrototypeConfiguration JointDefaultConfiguration;
  private String[] Joints = null;
  private boolean occuredError = false;
  private int KeyFrameCount = 0;
  private int LastKeyFrameTime = 0;
  private KeyFrame LastKeyFrame = null;
  private int KeyFrameNodeX = 50;
  private int KeyFrameNodeY = 50;
  private int PaintDirection = 1;


  private enum JointID
  {
    HeadPitch,
    HeadYaw,
    RShoulderRoll,
    LShoulderRoll,
    RShoulderPitch,
    LShoulderPitch,
    RElbowRoll,
    LElbowRoll,
    RElbowYaw,
    LElbowYaw,
    RHipYawPitch,
    LHipYawPitch,
    RHipPitch,
    LHipPitch,
    RHipRoll,
    LHipRoll,
    RKneePitch,
    LKneePitch,
    RAnklePitch,
    LAnklePitch,
    RAnkleRoll,
    LAnkleRoll,
    numOfJoint
  };

  private enum KeyWordID
  {
    EndOfLine,
    Separator,
    TimeSeparator,
    Head,
    Version,
    NumberOfKeyWordID
  };

  private MotionNet motionNet;

  MotionNetImport(File file)
  {
    try
    {
      this.scanner = new Scanner(new FileReader(file));
      this.scanner.setLineCommentChar(null);
      this.JointDefaultConfiguration = new JointDefaultConfiguration();
      this.Joints = new String[JointID.values().length - 1];
      this.motionNet = new MotionNet(this.JointDefaultConfiguration);
      this.numberOfJoints = 0;
      this.currentToken = null;
      scanner.getNextChar();
      eat();
    }
    catch (IOException e)
    {
      JOptionPane.showMessageDialog(null,
        "Error : Could not open " + file.getName(),
        "Error", JOptionPane.ERROR_MESSAGE);
      this.occuredError = true;
      return;
    }
  }


  private void eat() throws IOException
  {
    scanner.getNextToken();
  }
  //end eat

  private boolean isToken(KeyWordID tokenId)
  {
    return scanner.isToken(getKeyWord(tokenId));
  }//end isToken

  private void isTokenAndEat(KeyWordID tokenId) throws IOException
  {
    if(isToken(tokenId))
      eat();
    else
      ;//throw "ERROR: Syntaxfehler in Zeile %d.\n";
  }
  //end isTokenAndEat

  public MotionNet parse() throws IOException
  {
    eat();
    isTokenAndEat(KeyWordID.Head);
    isTokenAndEat(KeyWordID.Separator);
    isTokenAndEat(KeyWordID.Version);

    parseMotionNet();
  //  System.out.println("imported: " + this.motionNet.toString());
    return this.motionNet;
  }//end parse



  public void parseMotionNet() throws IOException
  {
    parseJointIndex();

    while(!this.occuredError && NextIsInt() )
    {
      parseKeyFrame();
    }//end while

    //while(!this.occuredError && isToken(KeyWordID.transition) )
    //{
    //  parseTransition();
    //}//end while

  }//end parseMotionNet



  public void parseJointIndex() throws IOException
  {
    while(!this.occuredError && !NextIsInt())
    {
      isTokenAndEat(KeyWordID.Separator); // ","
      //aktuelle Joint ID
      JointID id = jointIDFromName(scanner.buffer);

      eat(); // eat the joint name

      if(id == JointID.numOfJoint)
      {
        JOptionPane.showMessageDialog(null,
          "Error : " + getJointName(id) + " => next token: " + scanner.buffer,
          "Error", JOptionPane.ERROR_MESSAGE);
        this.occuredError = true;
        return;
      }
      else
      {
        if(this.numberOfJoints < JointID.values().length - 1)//motionNet.numberOfJoints < numberOfJoints)
        {
          this.Joints[this.numberOfJoints++] = getJointName(id);
        }
        else
        {
          JOptionPane.showMessageDialog(null,
            "Error : To many joint indices detected",
            "Error", JOptionPane.ERROR_MESSAGE);
          this.occuredError = true;
          return;
        }
      //obsolete  this.Joints[this.numberOfJoints] = getJointName(JointID.numOfJoint);
      }
    }//end while
  }//end parseJointIndex

  public void parseKeyFrame() throws IOException
  {
      float[] Keys = new float[numberOfJoints];//numberOfJoints

      int Minutes = parseInt();
      isTokenAndEat(KeyWordID.TimeSeparator); // ":"
      int Seconds = parseInt();
      isTokenAndEat(KeyWordID.TimeSeparator); // ":"
      int MilliSeconds = parseInt() + 1000 * (Seconds + 60 * Minutes);
      isTokenAndEat(KeyWordID.Separator); // ";"
      while(!isToken(KeyWordID.Separator))
      {
        eat();
      }
      int id = this.KeyFrameCount++;

      // parse joint values
      int i = 0;

      if(this.LastKeyFrame != null)
      {
        this.KeyFrameNodeX += 50 * this.PaintDirection;
        if(this.KeyFrameNodeX >= 500 || this.KeyFrameNodeX <= 0)
        {
          this.PaintDirection *= -1;
          this.KeyFrameNodeX += 50 * this.PaintDirection;
          //this.KeyFrameNodeX = 0;
          this.KeyFrameNodeY += 50;
        }
      }

      KeyFrame Frame = new KeyFrame(this.JointDefaultConfiguration, id, this.KeyFrameNodeX, this.KeyFrameNodeY);

      while(isToken(KeyWordID.Separator))
      {
        isTokenAndEat(KeyWordID.Separator); // ";"
        if(i < this.Joints.length)//numberOfJoints)
        {
          Keys[i] = Math.round(parseFloat() / Math.PI * 18000) / 100;
          Frame.setJointValue(this.Joints[i], Keys[i]);
        }
        else
        {
          JOptionPane.showMessageDialog(null,
            "Error : To much keys in frame " + id + " detected",
            "Error", JOptionPane.ERROR_MESSAGE);
          this.occuredError = true;
          return;
        }
         i++;
      }
      //end while
      this.motionNet.addKeyFrame(Frame);
      if(this.LastKeyFrame != null)
      {
        Transition transition = new Transition("*", MilliSeconds - this.LastKeyFrameTime);
        this.motionNet.addTransition(transition, this.LastKeyFrame, Frame);
      }
      this.LastKeyFrame = Frame;
      this.LastKeyFrameTime = MilliSeconds;


  }
  //end parseKeyFrame


  // pars a integer (something like "- 20" is not allowed, correct would be "-20")
  public boolean NextIsInt()
  {
    try
    {
      Integer.parseInt(scanner.buffer);
    }
    catch(Exception e)
    {
      return false;
    }
    
    return true;
  }
  //end parseInt

  public int parseInt() throws IOException
  {
    int result = Integer.parseInt(scanner.buffer);
    eat();
    return result;
  }
  //end parseInt


  // pars a float
  public float parseFloat() throws IOException
  {
    float result = (float) Float.parseFloat(scanner.buffer);
    eat();

    return result;
  }
  //end parseInt


  private String getKeyWord(KeyWordID keyWordID)
  {
    switch(keyWordID)
    {
      case EndOfLine: return "\n";
      case Separator: return ",";
      case TimeSeparator: return ":";
      case Head: return "WEBOTS_MOTION";
      case Version: return "V1.0";
      default: return "unknown";
    }//end switch
  }
  //end getKeyWord

  private String getJointName(JointID joint)
  {
    switch(joint)
    {
      case HeadPitch: return "HeadPitch";
      case HeadYaw: return "HeadYaw";
      case RShoulderRoll: return "RShoulderRoll";
      case LShoulderRoll: return "LShoulderRoll";
      case RShoulderPitch: return "RShoulderPitch";
      case LShoulderPitch: return "LShoulderPitch";
      case RElbowRoll: return "RElbowRoll";
      case LElbowRoll: return "LElbowRoll";
      case RElbowYaw: return "RElbowYaw";
      case LElbowYaw: return "LElbowYaw";
      case LHipYawPitch: return "LHipYawPitch";
      case RHipYawPitch: return "RHipYawPitch";
      case RHipPitch: return "RHipPitch";
      case LHipPitch: return "LHipPitch";
      case RHipRoll: return "RHipRoll";
      case LHipRoll: return "LHipRoll";
      case RKneePitch: return "RKneePitch";
      case LKneePitch: return "LKneePitch";
      case RAnklePitch: return "RAnklePitch";
      case LAnklePitch: return "LAnklePitch";
      case RAnkleRoll: return "RAnkleRoll";
      case LAnkleRoll: return "LAnkleRoll";
      default: return "Unknown Joint";
    }//end switch
  }
  //end getJointName


  private JointID jointIDFromName(String name)
  {
    try
    {
      return JointID.valueOf(name);
    }
    catch(IllegalArgumentException e)
    {
      return JointID.numOfJoint;
    }
  }
  //end getJointName


}
