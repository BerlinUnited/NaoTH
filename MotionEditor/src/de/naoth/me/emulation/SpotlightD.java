/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.naoth.me.emulation;

import js.math.vector.*;

/**
 *
 * @author claas
 */
public class SpotlightD extends ObjectVRML
{
  public boolean on;
  public float intensity;
  public float ambientIntensity;
  public Vec3f color;
  public Vec3f location;
  public Vec3f direction;
  public Vec3f attenuation;
  public float radius;
  public float beamWidth;
  public float cutOffAngle;
  public String type = "spotlight";

  public SpotlightD()
  {
    on = false;
    intensity = 1.0f;
    ambientIntensity = 0.0f;
    color = new Vec3f(1, 1, 1);
    location = new Vec3f(0, 1, 10);
    direction = new Vec3f(0, 0, -1);
    attenuation = new Vec3f(1, 0, 0);
    radius = 100.0f;
    beamWidth = 1.57f;
    cutOffAngle = 0.78f;
    name = "";
  }

  public SpotlightD(String n)
  {
    on = false;
    intensity = 1.0f;
    ambientIntensity = 0.0f;
    color = new Vec3f(1, 1, 1);
    location = new Vec3f(0, 1, 10);
    direction = new Vec3f(0, 0, -1);
    attenuation = new Vec3f(1, 0, 0);
    radius = 100.0f;
    beamWidth = 1.57f;
    cutOffAngle = 0.78f;
    name = n;
  }
}
