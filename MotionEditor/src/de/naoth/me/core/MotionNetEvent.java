/*
 * This class describes a event thrown by a motion net 
 */

package de.naoth.me.core;

/**
 *
 * @author Heinrich Mellmann
 */
public class MotionNetEvent
{
    public static enum EventType{
        EVENT_KEYFRAME_ADDED,
        EVENT_KEYFRAME_REMOVED
    }
    
    private EventType eventType;
    private MotionNet sender;
    private Object value;

    public Object getValue() {
        return value;
    }

    public MotionNet getSender() {
        return sender;
    }
    
    public EventType getEventType()
    {
        return this.eventType;
    }

    public MotionNetEvent(MotionNet sender, Object value, EventType eventType)
    {
        this.sender = sender;
        this.value = value;
        this.eventType = eventType;
    }
}//end class MotionNetEvent
