package de.naoth.rc.dataformats;

import java.awt.Color;
import java.awt.image.BufferedImage;

/**
 * Holds an image as RGB and YCbCr image
 * @author thomas
 */
public class JanusImage
{

  private BufferedImage yCbCr = null;
  private BufferedImage rgb = null;

  /**
   * Constructor for YCbCr-images
   * @param YCbCr the image as YCbCr format
   */
  public JanusImage(BufferedImage yCbCr)
  {
    this(yCbCr, true);
  }

  /**
   * Common constructor for YCbCr and RGB images
   * @param img The image data
   * @param isYCbCr Whether the image data is YCbCr
   */
  public JanusImage(BufferedImage img, boolean isYCbCr)
  {
    if(isYCbCr)
    {
      this.yCbCr = img;
    }
    else
    {
      this.rgb = img;
    }
  }//

  private BufferedImage RGB2YCbCr(BufferedImage img)
  {
    if(img == null) return null;
    
    BufferedImage yCbCrImg = new BufferedImage(img.getWidth(), img.getHeight(), BufferedImage.TYPE_INT_RGB);

    for(int xx = 0; xx < img.getWidth(); xx++)
    {
      for(int yy = 0; yy < img.getHeight(); yy++)
      {
        Color rgbCol = new Color(img.getRGB(xx, yy));
        double r = rgbCol.getRed();
        double g = rgbCol.getGreen();
        double b = rgbCol.getBlue();

        double y = 0.2990 * r + 0.5870 * g + 0.1140 * b;
        double cb = -0.1687 * r - 0.3313 * g + 0.5000 * b;
        double cr = 0.5000 * r - 0.4187 * g - 0.0813 * b;

        y = Math.max(0, Math.min(255, y));
        cb = Math.max(0, Math.min(255, cb));
        cr = Math.max(0, Math.min(255, cr));

        Color yCbCrCol = new Color(
          (int) y, (int) cb, (int) cr);

        yCbCrImg.setRGB(xx, yy, yCbCrCol.getRGB());
      }//end for
    }//end for

    return yCbCrImg;
  }//end RGB2YCbCr

  
  private BufferedImage YCbCr2RGB(BufferedImage img)
  {
    if(img == null) return null;
    
    BufferedImage rgbImg = new BufferedImage(img.getWidth(), img.getHeight(), BufferedImage.TYPE_INT_RGB);

    for(int xx = 0; xx < img.getWidth(); xx++)
    {
      for(int yy = 0; yy < img.getHeight(); yy++)
      {
        Color yCbCrCol = new Color(img.getRGB(xx, yy));
        double y = yCbCrCol.getRed();
        double cr = yCbCrCol.getGreen();
        double cb = yCbCrCol.getBlue();

        double r =  y + 1.4021 * (cb - 128);
        double g = y - 0.3456 * (cr - 128) - 0.71448 * (cb - 128);
        double b = y + 1.7710 * (cr - 128);

        r = Math.max(0, Math.min(255, r));
        g = Math.max(0, Math.min(255, g));
        b = Math.max(0, Math.min(255, b));

        Color rgbCol = new Color(
          (int) r, (int) g, (int) b);

        rgbImg.setRGB(xx, yy, rgbCol.getRGB());
      }//end for
    }//end for

    return rgbImg;
  }//end YCbCr2RGB

  
  /** Return RGB image version */
  public BufferedImage getRgb()
  {
    if(this.rgb == null)
      this.rgb = YCbCr2RGB(this.yCbCr);
      
    return this.rgb;
  }//end getRgb

  /** Return YCbCr image version */
  public BufferedImage getYCbCr()
  {
    if(this.yCbCr == null)
      this.yCbCr = RGB2YCbCr(this.rgb);
    
    return this.yCbCr;
  }//end getYCbCr
  
}//end class
