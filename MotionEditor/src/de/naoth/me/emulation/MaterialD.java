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
public class MaterialD
{
	public double ambientIntensity;
	public double shininess;
	public Vec3d diffuseColor;
	public Vec3d specularColor;
  public Vec3d emissiveColor;
  public String name;
  public MaterialD()
  {
   ambientIntensity = 0;
   shininess = 0;
   diffuseColor = new Vec3d(0);
   specularColor = new Vec3d(0);
   emissiveColor = new Vec3d(0);
   name = "";
  }


}
