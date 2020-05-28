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
public class Matrix3D implements Serializable
{
    public final Vector3D[] c;
    
    public Matrix3D() {
        this(new Vector3D(), new Vector3D(), new Vector3D());
    }
    
    public Matrix3D(Vector3D c0, Vector3D c1, Vector3D c2) {
        c = new Vector3D[] {c0, c1, c2};
    }
    
    public Vector3D multiply(Vector3D v)
    {
        return (c[0].multiply(v.x).add(c[1].multiply(v.y)).add(c[2].multiply(v.z)));
    }
    
    public double getYAngle()
    {
        double h = Math.sqrt(this.c[2].x * this.c[2].x + this.c[2].z * this.c[2].z);
        return (h != 0) ? Math.acos(this.c[2].z / h) * (this.c[2].x > 0 ? 1 : -1) : 0;
    }
    
    public double getZAngle()
    {
        double h = Math.sqrt(this.c[0].x * this.c[0].x + this.c[0].y * this.c[0].y);
        return (h != 0) ? Math.acos(this.c[0].x / h) * (this.c[0].y < 0 ? -1 : 1) : 0;
    }
    
    public double getXAngle()
    {
        double h = Math.sqrt(this.c[2].y * this.c[2].y + this.c[2].z * this.c[2].z);
        return (h != 0) ? Math.acos(this.c[2].z / h) * (this.c[2].y > 0 ? -1 : 1) : 0;
    }
    
}//end class Vector2D
