/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.naoth.me.emulation;

/**
 *
 * @author claas
 */
public class CylinderD extends GeometryD
{
  public double radius;
  public double height;
  public boolean top;
  public boolean bottom;

  CylinderD()
  {
    radius = 0;
    height = 0;
    top = true;
    bottom = true;
  }

  CylinderD(int r, int h)
  {
    radius = r;
    height = h;
    top = true;
    bottom = true;
  }

  CylinderD(int r, int h, String n)
  {
    radius = r;
    height = h;
    top = true;
    bottom = true;
    name = n;
  }

  CylinderD(int r, int h, boolean t, boolean b)
  {
    radius = r;
    height = h;
    top = t;
    bottom = b;
  }

  CylinderD(int r, int h, boolean t, boolean b, String n)
  {
    radius = r;
    height = h;
    top = t;
    bottom = b;
    name = n;
  }
}
