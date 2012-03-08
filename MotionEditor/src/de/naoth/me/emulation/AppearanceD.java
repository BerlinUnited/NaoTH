/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.naoth.me.emulation;

/**
 *
 * @author claas
 */
public class AppearanceD
{
  public MaterialD material;
  public Texture texture;
  public String name;
  public AppearanceD()
  {
    material = null;
    texture = null;
    name = "";
  }

  public AppearanceD(String n)
  {
    material = null;
    texture = null;
    name = n;
  }

}
