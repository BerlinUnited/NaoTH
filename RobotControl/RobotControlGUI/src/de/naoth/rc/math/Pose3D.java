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
public class Pose3D implements Serializable
{
    public final Vector3D translation;
    public final Matrix3D rotation;

    public Pose3D() {
      this(new Vector3D(), new Matrix3D());
    }

    public Pose3D(Vector3D translation, Matrix3D rotation) 
    {
        this.translation = translation;
        this.rotation = rotation;
    }
    
    public String toString()
    {
        return "[ rotation = " + rotation + ", translation = " + translation + "]";
    }
}//end Pose2D
