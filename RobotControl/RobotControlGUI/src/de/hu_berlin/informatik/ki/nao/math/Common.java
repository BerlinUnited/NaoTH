/*
 * Common.java
 *
 * Created on 2. Dezember 2007, 17:02
 *
 */

package de.hu_berlin.informatik.ki.nao.math;

/**
 *
 * @author Heinrich Mellmann
 */
public class Common 
{
    // es sollen keine Instanzen dieser Klasse angelegt werden
    private Common() {}
    
    
    
    
    public static Vector2D fieldToRelativeCoord(Pose2D pose, Vector2D point)
    {
        return point.subtract(pose.translation).rotate(-pose.rotation);
    }//end transformCoordinateSystem
    public static Vector2D relativeToFieldCoord(Pose2D pose, Vector2D point)
    {
        return point.rotate(pose.rotation).add(pose.translation);
    }//end inverseTransformCoordinateSystem
    
    public static double normalizeAngle(double angle)
    {
        if(angle > Math.PI){ return angle-2*Math.PI; }
        if(angle < -Math.PI){ return angle+2*Math.PI; }
        return angle;
    }//end normalize
    
    public static Vector2D polarToCartesian(double angle, double distance)
    {
        return new Vector2D(distance*Math.cos(angle), distance*Math.sin(angle));
    }//end normalize
    
}//end Common
