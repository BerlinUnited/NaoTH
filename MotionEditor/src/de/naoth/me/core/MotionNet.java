package de.naoth.me.core;

import java.io.*;
import java.util.ArrayList;
import java.util.HashMap;
import java.text.DecimalFormat;
import java.text.DecimalFormatSymbols;

/**
 *
 * @author Heinrich Mellmann
 */
public class MotionNet extends MotionNetEventHandler
{
    // contains the KeyFrames (nodes)
    private final HashMap<Integer,KeyFrame> keyFrameMap;
    // contains outgoing Transitions for every KeyFrame
    private final HashMap<KeyFrame, Transition> transitionMap;

    // the configuration of joints
    // the net can contain only KeyFrames with the same configuration
    private final JointPrototypeConfiguration configuration;
    
    
    public MotionNet(JointPrototypeConfiguration configuration)
    {
        this.configuration = configuration;
        this.keyFrameMap = new HashMap<Integer,KeyFrame>(50);
        this.transitionMap = new HashMap<KeyFrame, Transition>(50);
    }
    
    private int getNextFreeKeyFrameId()
    {
        for (int id = 0; id < keyFrameMap.size(); id++)
        {
            if (keyFrameMap.get(id) == null) return id;
        }//end for

        return keyFrameMap.size();
    }//end getNextFreeKeyFrameId
    
    public int sizeKeyFrame()
    {
        return keyFrameMap.size();
    }
    
    public void removeKeyFrame(KeyFrame keyFrame)
    {
        this.keyFrameMap.remove(keyFrame.getId());
        performMotionNetEvent(new MotionNetEvent(this, keyFrame, MotionNetEvent.EventType.EVENT_KEYFRAME_REMOVED));
    }//end removeKeyFrame
    
    public void removeTransition(Transition transition)
    {
        transition.getFromKeyFrame().removeOutEdge(transition);
    }//end removeKeyFrame
    
    
    
    public KeyFrame addKeyFrame(KeyFrame keyFrame)
    {
        // ACHTUNG: alter KeyFrame wird eventuell ueberschrieben
        this.keyFrameMap.put(keyFrame.getId(), keyFrame);
        performMotionNetEvent(new MotionNetEvent(this, keyFrame, MotionNetEvent.EventType.EVENT_KEYFRAME_ADDED));
        return keyFrame;
    }//end addKeyFrame
    
    // adds a new keyFrame
    public KeyFrame addKeyFrame(int x, int y)
    {
        return addKeyFrame(new KeyFrame(configuration, getNextFreeKeyFrameId(), x, y));
    }//end addKeyFrame
    
    public void addTransition(Transition transition, KeyFrame fromKeyFrame, KeyFrame toKeyFrame)
    {
        fromKeyFrame.addOutEdge(transition);
        ((Edge<KeyFrame>)transition).setFromNode(fromKeyFrame);
        ((Edge<KeyFrame>)transition).setToNode(toKeyFrame);
    }//end addTransition
    
    public void addTransition(Transition transition, int fromId, int toId)
    {
        getKeyFrame(fromId).addOutEdge(transition);
        ((Edge<KeyFrame>)transition).setFromNode(getKeyFrame(fromId));
        ((Edge<KeyFrame>)transition).setToNode(getKeyFrame(toId));
    }//end addTransition
    
    public KeyFrame getKeyFrame(int id)
    {
        return this.keyFrameMap.get(id);
    }//end addKeyFrame
     
    public HashMap<Integer,KeyFrame> getKeyFrameMap()
    {
        return this.keyFrameMap;
    }//end getKeyFrameMap
    
    public static abstract class Edge<T extends Node> {
        protected abstract void setFromNode(T node);
        protected abstract void setToNode(T node);
    }//end class Edge  
    
    public static abstract class Node<T extends Edge>
    {
        protected abstract void addOutEdge(T edge);
    }//end class Node
       
    @Override
    public String toString()
    {
      String result = "nao\n\n";

      // print the configuration
      result += this.configuration + "\n";

      // print the joints and transitions
      String transitionString = "";

      for(KeyFrame keyFrame: keyFrameMap.values())
      {
          result += keyFrame + "\n";

          for(Transition transition: keyFrame.getTransitions())
          {
              transitionString += transition + "\n";
          }//end for
      }//end for

//    DecimalFormatSymbols dfs = new DecimalFormatSymbols();
//    dfs.setDecimalSeparator('.');
//    DecimalFormat numberFormat = new DecimalFormat( "##0.##", dfs);

     return result + "\n" + transitionString;
    }//end toString
    
    public String toTxt()
    {
      String result = "";
      KeyFrame initialFrame = null;
      for(KeyFrame keyFrame: keyFrameMap.values()) 
          if(keyFrame.getId()==0) initialFrame = keyFrame;
      while(!initialFrame.getTransitions().isEmpty()){
          result += initialFrame.getTransitions().get(0).toTxt() + 
                        initialFrame.getTransitions().get(0).getToKeyFrame().toTxt() + "\r\n";
          initialFrame = initialFrame.getTransitions().get(0).getToKeyFrame();
      }

//    DecimalFormatSymbols dfs = new DecimalFormatSymbols();
//    dfs.setDecimalSeparator('.');
//    DecimalFormat numberFormat = new DecimalFormat( "##0.##", dfs);

     return result;
    }//end toTxt
}//end class MotionNet
