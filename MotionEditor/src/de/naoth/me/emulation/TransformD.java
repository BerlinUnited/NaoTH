/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.naoth.me.emulation;

import js.math.vector.Vec3d;
import java.util.ArrayList;

/**
 *
 * @author claas
 */
public class TransformD extends ObjectVRML
{
  public double angle;
  public Vec3d rotation;
  public Vec3d translation;
  public ArrayList<ObjectVRML> children;

  public TransformD()
  {
    angle = 0;
    rotation = new Vec3d(0);
    translation = new Vec3d(0);
    children = new ArrayList<ObjectVRML>();
  }

}
