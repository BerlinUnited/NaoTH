/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.naoth.rc.drawings;

import java.awt.Graphics2D;
import java.awt.image.BufferedImage;

/**
 *
 * @author thomas
 */
public class ImageDrawing implements Drawable
{

  private BufferedImage image;

  public ImageDrawing()
  {
  }

  public ImageDrawing(BufferedImage image)
  {
    this.image = image;
  }

  public BufferedImage getImage()
  {
    return image;
  }

  public void setImage(BufferedImage image)
  {
    this.image = image;
  }


  @Override
  public void draw(Graphics2D g2d)
  {
    if(image != null)
    {
      g2d.drawImage(image, 0, 0, null);
    }
  }

}
