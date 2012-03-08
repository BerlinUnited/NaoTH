/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.naoth.me.emulation;

import js.math.vector.Vec3d;
/**
 *
 * @author claas
 */
public class BoxD extends GeometryD
{
  public Vec3d size;
  BoxD()
  {
    size = new Vec3d(0);
  }

  BoxD( Vec3d s)
  {
     size = s;
  }

  BoxD( Vec3d s, String n)
  {
     size = s;
     name = n;
  }
}
