package de.naoth.me.core;


/**
 *
 * @author Heinrich Mellmann
 */
public class Transition extends MotionNet.Edge<KeyFrame>
{
    private KeyFrame fromKeyFrame;
    private KeyFrame toKeyFrame;
    
    private double duration;

    public void setDuration(double duration) {
        this.duration = duration;
    }
    private String condition;

    public static final String DEFAULT_CONDITION = "*";
    
    public Transition(String condition, double duration)
    {
        this.duration = duration;
        this.condition = condition;
        
        this.fromKeyFrame = null;
        this.toKeyFrame = null;
    }
    
    public Transition(double duration)
    {
        this(DEFAULT_CONDITION, duration);
    }
    
    
    public String getCondition() {
        return condition;
    }

    public void setCondition(String condition) {
        this.condition = condition;
    }
    
    public double getDuration() {
        return duration;
    }

    public KeyFrame getFromKeyFrame() {
        return fromKeyFrame;
    }

    public KeyFrame getToKeyFrame() {
        return toKeyFrame;
    }
    
    
    
    protected void setFromNode(KeyFrame keyFrame)
    {
        this.fromKeyFrame = keyFrame;
    }
    
    protected void setToNode(KeyFrame keyFrame)
    {
        this.toKeyFrame = keyFrame;
    }
    
    @Override
    public String toString()
    {
        String result = "transition;" + 
                fromKeyFrame.getId() + ";" + 
                toKeyFrame.getId() + ";" +
                duration + ";" + 
                condition;
        
        return result;
    }//end toString
    
    public String toTxt()
    {
        return ""+(int)duration;
    }//end toString
}//end class Transition
