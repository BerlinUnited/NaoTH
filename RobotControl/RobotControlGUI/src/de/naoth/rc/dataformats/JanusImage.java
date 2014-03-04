package de.naoth.rc.dataformats;

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
    if(isYCbCr) {
      this.yCbCr = img;
    } else {
      this.rgb = img;
    }
  }
  
  /** Return RGB image version */
  public BufferedImage getRgb()
  {
    if(this.rgb == null) {
      this.rgb = new BufferedImage(
        this.yCbCr.getWidth(), this.yCbCr.getHeight(), BufferedImage.TYPE_INT_RGB);
      ImageConversions.convertYUV888toRGB888Precise(this.yCbCr, this.rgb);
    }
    return this.rgb;
  }

  /** Return YCbCr image version */
  public BufferedImage getYCbCr()
  {
    if(this.yCbCr == null) {
      this.yCbCr = new BufferedImage(
        this.rgb.getWidth(), this.rgb.getHeight(), BufferedImage.TYPE_INT_RGB);
      ImageConversions.convertRGB888toYUV888Precise(this.rgb, this.yCbCr);
    }
    return this.yCbCr;
  }
  
}//end class
