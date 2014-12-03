/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.naoth.rc;

import net.xeoh.plugins.base.Plugin;

/**
 *
 * @author thomas
 */
public interface ByteRateUpdateHandler extends Plugin
{

  public void setSentByteRate(double rate);
  public void setReceiveByteRate(double rate);
  public void setServerLoopFPS(double fps);
}
