/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.naoth.me.emulation;

/**
 *
 * @author claas
 */
public class SphereD extends GeometryD
{
  public double radius;
  SphereD()
  {
    radius = 0;
  }

  SphereD(int r)
  {
    radius = r;
  }

  SphereD(int r, String n)
  {
    radius = r;
    name = n;
  }
}
