/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.naoth.rc.dialogs.drawings;

import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.geom.AffineTransform;
import java.awt.image.BufferedImage;

/**
 *
 * @author gxy
 */
public class SpaceInvader implements Drawable
{
  static BufferedImage image = new BufferedImage(11,8,BufferedImage.TYPE_INT_ARGB);
  
  static
  {
    int o = new Color(0,0,0,0).getRGB();
    int x = new Color(0,0,0,255).getRGB();

    int[] data = new int[]{
      o,o,x,o,o,o,o,o,x,o,o,
      o,o,o,x,o,o,o,x,o,o,o,
      o,o,x,x,x,x,x,x,x,o,o,
      o,x,x,o,x,x,x,o,x,x,o,
      x,x,x,x,x,x,x,x,x,x,x,
      x,o,x,x,x,x,x,x,x,o,x,
      x,o,x,o,o,o,o,o,x,o,x,
      o,o,o,x,x,o,x,x,o,o,o
    };
    image.setRGB(0,0,11,8,data,0,11);
  }

  public SpaceInvader(String[] args)
  {
    
  }

  public SpaceInvader()
  {

  }

  public void draw(Graphics2D g2d)
  {
    g2d.scale(100, 100);
    g2d.transform(new AffineTransform(1,0,0,-1,0,0));
    g2d.drawImage(image, 0, 0, null);
    g2d.transform(new AffineTransform(1,0,0,-1,0,0));
    g2d.scale(0.01, 0.01);
  }//end draw
}//end class SpaceInvader
