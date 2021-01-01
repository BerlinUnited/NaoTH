package de.naoth.rc.dataformats;

import java.awt.image.BufferedImage;

/**
 * Holds an image as RGB and YUV image
 * @author thomas
 */
public class JanusImage
{

  private BufferedImage yuv = null;
  private BufferedImage rgb = null;

  
  public static JanusImage createFromRGB(BufferedImage rgb) {
      return new JanusImage(rgb, null);
  }
  
  public static JanusImage createFromYUV(BufferedImage yuv) {
      return new JanusImage(null, yuv);
  }
  
  /**
   * Common internal constructor for YUV and RGB images
   * @param img The image data
   * @param isYCbCr Whether the image data is YUV
   */
  private JanusImage(BufferedImage rgb, BufferedImage yuv)
  {
    this.yuv = yuv;
    this.rgb = rgb;
  }
  
  /** Return RGB image version */
  public BufferedImage getRGB()
  {
    if(this.rgb == null) {
      //this.rgb = new BufferedImage(this.yCbCr.getWidth(), this.yCbCr.getHeight(), BufferedImage.TYPE_INT_RGB);
      this.rgb = ImageConversions.createCompatibleImage(this.yuv.getWidth(), this.yuv.getHeight());
      ImageConversions.convertYUV888toRGB888Precise(this.yuv, this.rgb);
    }
    return this.rgb;
  }

  /** Return YUV image version */
  public BufferedImage getYUV()
  {
    if(this.yuv == null) {
      //this.yuv = new BufferedImage(this.rgb.getWidth(), this.rgb.getHeight(), BufferedImage.TYPE_INT_RGB);
      this.yuv = ImageConversions.createCompatibleImage(this.rgb.getWidth(), this.rgb.getHeight());
      ImageConversions.convertRGB888toYUV888Precise(this.rgb, this.yuv);
    }
    return this.yuv;
  }
  
}//end class
