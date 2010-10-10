/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.hu_berlin.informatik.ki.nao.interfaces;

import net.xeoh.plugins.base.Plugin;

/**
 *
 * @author thomas
 */
public interface ByteRateUpdateHandler extends Plugin
{

  public void setSentByteRate(double rate);
  public void setReceiveByteRate(double rate);

}
