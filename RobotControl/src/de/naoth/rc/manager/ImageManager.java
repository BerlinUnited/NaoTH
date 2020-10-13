/*
 * 
 */
package de.naoth.rc.manager;

import de.naoth.rc.core.manager.AbstractManagerPlugin;
import com.google.protobuf.ByteString;
import de.naoth.rc.dataformats.ImageConversions;
import de.naoth.rc.dataformats.JanusImage;
import de.naoth.rc.core.messages.FrameworkRepresentations.Image;
import java.awt.image.BufferedImage;
import java.awt.image.Raster;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.imageio.ImageIO;
import javax.imageio.ImageReader;
import javax.imageio.stream.ImageInputStream;

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
      
      BufferedImage dst = ImageConversions.createCompatibleImage(img.getWidth(), img.getHeight());

      // NOTE: ByteString is a protobuf construct. Converting it to byte[] array implies a copy, which is slow.
      //       img.getData().asReadOnlyByteBuffer() returns a java.nio.ByteBuffer object without copy.
      //       We could change all conversion routines to work on ByteBuffer instead of ByteString to reduce
      //       unnecessary dependency on protobuf.
      ByteString src = img.getData();
      
      if(img.getFormat() == null) {
          throw new Exception("Image format is null.");
      }
          
      switch (img.getFormat()) {
        case YUV:
            ImageConversions.convertYUV888toYUV888(src, dst);
            break;
        case YUV422:
            // NOTE: direct conversion from yuv422 to rgb seems to be a bit slower than yuv422->yuv->rgb
            //ImageConversions.convertYUV422toRGB888Precise(src, dst);
            //return JanusImage.createFromRGB(dst);

            ImageConversions.convertYUV422toYUV888(src, dst);
            break;
        case JPEG:
            //dst = ImageIO.read( src.newInput() );
            //return new JanusImage(dst, false);

            ImageInputStream is = ImageIO.createImageInputStream(src.newInput());
            jpegReader.setInput(is);
            Raster raster = jpegReader.readRaster(0, null);
            is.close();
            ImageConversions.convertYUV422toYUV888(raster, dst);
            break;
        default:
            throw new Exception("Unknown image format: " + img.getFormat());
        }

      return JanusImage.createFromYUV(dst);
    }
    catch(Exception ex)
    {
      Logger.getLogger(ImageManager.class.getName()).log(Level.SEVERE, "could not parse message", ex);
    }

    return null;
  }//end convertByteArrayToType

  
}//end class ImageManager

