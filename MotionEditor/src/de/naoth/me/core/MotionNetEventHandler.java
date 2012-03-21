/*
 * This class implements the basic functionality to handle the 
 * MotionNetEvent-events
 */

package de.naoth.me.core;

import java.util.LinkedList;
import java.util.List;

/**
 *
 * @author Heinrich Mellmann
 */
public class MotionNetEventHandler 
{
    private List<MotionNetListener> motionNetChangedListenerList;
    
    public MotionNetEventHandler()
    {
        motionNetChangedListenerList = new LinkedList<MotionNetListener>();
    }
    
    public void addMotionNetListener(MotionNetListener listener)
    {
        motionNetChangedListenerList.add(listener);
    }//end addMotionNetChangedListener
    
    public void removeMotionNetListener(MotionNetListener listener)
    {
        motionNetChangedListenerList.remove(listener);
    }//end addMotionNetChangedListener
    
    
    protected void performMotionNetEvent(MotionNetEvent event)
    {
        for(MotionNetListener listener: motionNetChangedListenerList)
        {
            switch(event.getEventType())
            {
                case EVENT_KEYFRAME_ADDED: 
                    listener.keyFrameAdded(event);
                    break;
                case EVENT_KEYFRAME_REMOVED: 
                    listener.keyFrameRemoved(event);
                    break;
            }//end switch
        }//end for
    }//end performMotionNetChangedEvent
}//end class MotionNetEventHandler
