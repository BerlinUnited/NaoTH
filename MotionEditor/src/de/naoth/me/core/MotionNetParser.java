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
public class MotionNetParser
{

  private Scanner scanner;
//  private String currentToken;
  private int numberOfJoints;
  //private JointPrototypeConfiguration jointPrototypeConfiguration;
  private JointPrototypeConfiguration JointDefaultConfiguration;
  private String[] Joints = null;
  private boolean occuredError = false;
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
    endOfLine,
    separator,
    keyFrame,
    transition,
    comment,
    numberOfKeyWordID,
    nao
  };

  private MotionNet motionNet;

  MotionNetParser(File file)
  {
    try
    {
      this.scanner = new Scanner(new FileReader(file));
      this.JointDefaultConfiguration = new JointDefaultConfiguration();
      this.Joints = new String[JointID.values().length - 1];
      this.motionNet = new MotionNet(this.JointDefaultConfiguration);
      this.numberOfJoints = 0;
//      this.currentToken = null;
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
    isTokenAndEat(KeyWordID.nao);
    parseMotionNet();
    //System.out.println("parsed: " + this.motionNet.toString());
    return this.motionNet;
  }//end parse



  public void parseMotionNet() throws IOException
  {
    parseJointIndex();

    while(!this.occuredError && isToken(KeyWordID.keyFrame) )
    {
      parseKeyFrame();
    }//end while

    while(!this.occuredError && isToken(KeyWordID.transition) )
    {
      parseTransition();
    }//end while

  }//end parseMotionNet



  public void parseJointIndex() throws IOException
  {
    while(!this.occuredError && !isToken(KeyWordID.transition) && !isToken(KeyWordID.keyFrame))
    {
      //aktuelle Joint ID
      JointID id = jointIDFromName(scanner.buffer);

      eat(); // eat the joint name
      isTokenAndEat(KeyWordID.separator); // ";"

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
          //System.out.println(getJointName(id));
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

      isTokenAndEat(KeyWordID.keyFrame); // keyword "keyframe"
      isTokenAndEat(KeyWordID.separator); // ";"
      int id = parseInt(); // id

      // skip the coordinates
      isTokenAndEat(KeyWordID.separator); // ";"
      int x = parseInt(); // x
      isTokenAndEat(KeyWordID.separator); // ";"
      int y = parseInt(); // y

      if(this.LastKeyFrame != null)
      {
        this.KeyFrameNodeX += 100 * this.PaintDirection;
        if(this.KeyFrameNodeX >= 500 || this.KeyFrameNodeX <= 0)
        {
          this.PaintDirection *= -1;
          this.KeyFrameNodeX += 100 * this.PaintDirection;
          //this.KeyFrameNodeX = 0;
          this.KeyFrameNodeY += 100;
        }
        if(x <= 0 || y <= 0)
        {
          x = this.KeyFrameNodeX;
          y = this.KeyFrameNodeY;
        }
      }

      // parse joint values
      int i = 0;
      KeyFrame Frame = new KeyFrame(this.JointDefaultConfiguration, id, x, y);

      while( !isToken(KeyWordID.keyFrame) && isToken(KeyWordID.separator))
      {
        isTokenAndEat(KeyWordID.separator); // ";"
        if(i < this.Joints.length)//numberOfJoints)
        {
          Keys[i] = Math.round(parseFloat() * 100) / 100;
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
      this.LastKeyFrame = Frame;
  }
  //end parseKeyFrame


  public void parseTransition() throws IOException
  {
      // transition;id_from;id_to;duration;condition

      isTokenAndEat(KeyWordID.transition); // keyword "transition"
      isTokenAndEat(KeyWordID.separator); // ";"
      int fromFrame = parseInt(); // id from
      KeyFrame fromKeyFrame = this.motionNet.getKeyFrame(fromFrame);
      if(fromKeyFrame == null)
      {
        JOptionPane.showMessageDialog(null,
          "Error : Transition from unknown keyframe " + fromFrame + " detected",
          "Error", JOptionPane.ERROR_MESSAGE);
          this.occuredError = true;
        return;
      }
      isTokenAndEat(KeyWordID.separator); // ";"
      int toFrame = parseInt(); // id to
      KeyFrame toKeyFrame = this.motionNet.getKeyFrame(toFrame);
      if(fromKeyFrame == null)
      {
        JOptionPane.showMessageDialog(null,
          "Error : Transition to unknown keyframe " + toFrame + " detected",
          "Error", JOptionPane.ERROR_MESSAGE);
          this.occuredError = true;
        return;
      }
      isTokenAndEat(KeyWordID.separator); // ";"
      float duration = parseFloat(); // duration

      isTokenAndEat(KeyWordID.separator); // ";"

      // get the condition and eat
      String condition = scanner.buffer;
      eat();

      Transition transition = new Transition(condition, duration);
      this.motionNet.addTransition(transition, fromKeyFrame, toKeyFrame);
  }//end parseTransition

  // pars a integer (something like "- 20" is not allowed, correct would be "-20")
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
      case endOfLine: return "\n";
      case separator: return ";";
      case keyFrame: return "keyframe";
      case transition: return "transition";
      case comment: return "#";
      case nao: return "nao";
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
