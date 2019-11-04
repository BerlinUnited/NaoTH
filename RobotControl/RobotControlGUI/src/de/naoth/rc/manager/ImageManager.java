/*
 * 
 */
package de.naoth.rc.manager;

import de.naoth.rc.core.manager.AbstractManagerPlugin;
import com.google.protobuf.ByteString;
import de.naoth.rc.dataformats.ImageConversions;
import de.naoth.rc.dataformats.JanusImage;
import de.naoth.rc.messages.FrameworkRepresentations.Image;
import java.awt.image.BufferedImage;
import java.awt.image.Raster;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.imageio.ImageIO;
import javax.imageio.ImageReader;

/**
 * Manager for images.
 * @author heinrich
 */
abstract class ImageManager extends AbstractManagerPlugin<JanusImage>
{

  private final ImageReader jpegReader = ImageConversions.getJPEGReader();
  
  @Override
  public JanusImage convertByteArrayToType(byte[] result) throws IllegalArgumentException
  {
    try
    {
      Image img = Image.parseFrom(result);

      BufferedImage dst = new BufferedImage(
        img.getWidth(), img.getHeight(), BufferedImage.TYPE_INT_RGB);
      
      ByteString src = img.getData();
      
      if(img.getFormat() == null) {
          throw new Exception("Image format is null.");
      }
          
      switch (img.getFormat()) {
        case YUV:
            ImageConversions.convertYUV888toYUV888(src, dst);
            break;
        case YUV422:
            ImageConversions.convertYUV422toYUV888(src, dst);
            break;
        case JPEG:
            //dst = ImageIO.read( src.newInput() );
            //return new JanusImage(dst, false);

            jpegReader.setInput(ImageIO.createImageInputStream(src.newInput()));
            Raster raster = jpegReader.readRaster(0, null);
            ImageConversions.convertYUV422toYUV888(raster, dst);
            break;
        default:
            throw new Exception("Unknown image format: " + img.getFormat());
        }

      return new JanusImage(dst, true);
    }
    catch(Exception ex)
    {
      Logger.getLogger(ImageManager.class.getName()).log(Level.SEVERE, "could not parse message", ex);
    }

    return null;
  }//end convertByteArrayToType

  
}//end class ImageManager

