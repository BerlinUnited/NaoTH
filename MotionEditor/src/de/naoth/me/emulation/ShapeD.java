/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.naoth.me.emulation;

/**
 *
 * @author claas
 */
public class ShapeD extends ObjectVRML
{
  public AppearanceD appearance;
  public GeometryD geometry;
  public String name;

  public ShapeD()
  {
    appearance = null;
    geometry = null;
    name = "";
  }

  public ShapeD(String n)
  {
    appearance = null;
    geometry = null;
    name = n;
  }


}
