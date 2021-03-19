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
public class Vector3D implements Serializable
{
    public final double x;
    public final double y;
    public final double z;
    
    public Vector3D() {
      this(0.0, 0.0, 0.0);
    }
    
    /** Creates a new instance of Vector2D */
    public Vector3D(double x, double y, double z) 
    {
        this.x = x;
        this.y = y;
        this.z = z;
    }
    
    public Vector3D add(Vector3D vector)
    {
        return new Vector3D(x + vector.x, y + vector.y, z + vector.z);
    }
    
    public Vector3D subtract(Vector3D vector)
    {
        return new Vector3D(x - vector.x, y - vector.y, z - vector.z);
    }
    
    public Vector3D multiply(double a)
    {
        return new Vector3D(a*x, a*y, a*z);
    }
    
    public double length()
    {
        return java.lang.Math.sqrt(x*x + y*y + z*z);
    }
    
    public Vector3D normalize() {
        double l = length();
        if(l == 0) {
            return null; // TODO: need an exception here
        } else {
            return new Vector3D(x/l, y/l, z/l);
        }
    }
    
    public static Vector3D e1()
    {
        return new Vector3D(1.0, 0.0, 0.0);
    }
    
    public static Vector3D e2()
    {
        return new Vector3D(0.0, 1.0, 0.0);
    }
    
    public static Vector3D e3()
    {
        return new Vector3D(0.0, 0.0, 1.0);
    }
    
    @Override
    public String toString() {
        return "[ x = " + x + ", y = " + y + ", z = " + y + "]";
    }
}//end class Vector2D
