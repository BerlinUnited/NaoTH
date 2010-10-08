package de.hu_berlin.informatik.ki.nao.dataformats;

import java.awt.Color;
import java.awt.image.BufferedImage;

/**
 * Holds an image as RGB and YCbCr image
 * @author thomas
 */
public class JanusImage
{

  private BufferedImage yCbCr;
  private BufferedImage rgb;

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

      this.rgb = new BufferedImage(img.getWidth(), img.getHeight(), BufferedImage.TYPE_INT_RGB);



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

          this.rgb.setRGB(xx, yy, rgbCol.getRGB());

        }
      }
    }
    else
    {
      this.rgb = img;
      this.yCbCr = new BufferedImage(img.getWidth(), img.getHeight(), BufferedImage.TYPE_INT_RGB);

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

          this.yCbCr.setRGB(xx, yy, yCbCrCol.getRGB());
        }
      }
    }
  }

  /** Return RGB image version */
  public BufferedImage getRgb()
  {
    return rgb;
  }

  /** Return YCbCr image version */
  public BufferedImage getYCbCr()
  {
    return yCbCr;
  }
}
