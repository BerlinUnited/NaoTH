/*
 * Pose2D.java
 *
 * Created on 8. Dezember 2007, 02:04
 *
 */

package de.naoth.rc.math;

import java.io.Serializable;

/**
 *
 * @author Heinrich Mellmann
 */
public class Pose2D implements Serializable
{
    public final Vector2D translation;
    public final double rotation;

    public Pose2D() {
      this(new Vector2D(), 0.0);
    }

    /** Creates a new instance of Pose2D */
    public Pose2D(double x, double y, double rotation) 
    {
        this(new Vector2D(x, y),rotation);
    }//end Pose2D
    
    public Pose2D(Vector2D translation, double rotation) 
    {
        this.translation = translation;
        this.rotation = Common.normalizeAngle(rotation);
    }//end Pose2D
    
    
    public Vector2D multiply(Vector2D point)
    {	
        double s = Math.sin(rotation);
        double c = Math.cos(rotation);
        return (new Vector2D(point.x*c-point.y*s , point.x*s+point.y*c)).add(translation);
    }//end multiply
    
    public Pose2D add(Pose2D pose)
    {
        return new Pose2D(this.multiply(pose.translation), this.rotation+pose.rotation);
    }//end add
    
    public Pose2D subtract(Pose2D pose)
    {       
        return new Pose2D(0,0,-pose.rotation).add(new Pose2D(this.translation.subtract(pose.translation), this.rotation));
    }//end subtract
    
    
    public String toString()
    {
        return "[ rotation = " + rotation + ", translation = " + translation + "]";
    }//end toString
}//end Pose2D
