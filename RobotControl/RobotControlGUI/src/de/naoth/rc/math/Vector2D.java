/*
 * Vector2D.java
 *
 * Created on 8. Dezember 2007, 02:01
 *
 */

package de.naoth.rc.math;

import java.io.Serializable;

/**
 *
 * @author Heinrich Mellmann
 */
public class Vector2D implements Serializable
{
    public final double x;
    public final double y;

    public Vector2D() {
      this(0.0,0.0);
    }
    
    /** Creates a new instance of Vector2D */
    public Vector2D(double x, double y) 
    {
        this.x = x;
        this.y = y;
    }//end constructor Vector2D
    
    public Vector2D add(Vector2D vector)
    {
        return new Vector2D(x + vector.x, y + vector.y);
    }//end add
    
    public Vector2D subtract(Vector2D vector)
    {
        return new Vector2D(x - vector.x, y - vector.y);
    }//end add
    
    public Vector2D multiply(double a)
    {
        return new Vector2D(a*x, a*y);
    }//end add
    
    public Vector2D rotate(double alpha)
    {
        double cos = Math.cos(alpha);
        double sin = Math.sin(alpha);
        return new Vector2D(cos*x - sin*y, sin*x + cos*y);
    }//end rotate
    
    public double length()
    {
        return java.lang.Math.sqrt(x*x + y*y);
    }//end length
    
    
    public static Vector2D e1()
    {
        return new Vector2D(1.0, 0.0);
    }//end e1
    
    public static Vector2D e2()
    {
        return new Vector2D(0.0, 1.0);
    }//end e2
    
    public double angleTo(Vector2D vector)
    {
        Vector2D tmp = vector.subtract(this);
        double angle = Math.atan2(tmp.y, tmp.x);
        if(Double.isNaN(angle) || Double.isInfinite(angle))
            System.out.println("ALARM: Vector2.angleTo is illegal!!!");
        return angle;
    }//end angleTo
    
    public String toString()
    {
        return "[ x = " + x + ", y = " + y + "]";
    }//end toString
}//end class Vector2D
