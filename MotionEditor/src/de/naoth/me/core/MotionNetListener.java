/*
 * This interface declares the basic functionality to process the 
 * MotionNet events
 */

package de.naoth.me.core;

/**
 *
 * @author Heinrich Mellmann
 */
public interface MotionNetListener
{
    void keyFrameAdded(MotionNetEvent event);
    void keyFrameRemoved(MotionNetEvent event);
}//end MotionNetChangedListener
