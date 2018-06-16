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
    
}//end class Vector2D
