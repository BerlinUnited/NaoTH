/*
 * 
 */
package de.naoth.rc.manager;

import de.naoth.rc.core.manager.AbstractManagerPlugin;
import com.google.protobuf.ByteString;
import com.google.protobuf.InvalidProtocolBufferException;
import de.naoth.rc.dataformats.ImageConversions;
import de.naoth.rc.dataformats.JanusImage;
import de.naoth.rc.messages.FrameworkRepresentations.Image;
import de.naoth.rc.server.Command;
import java.awt.image.BufferedImage;
import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.imageio.ImageIO;
import javax.imageio.ImageReader;
import net.xeoh.plugins.base.annotations.PluginImplementation;

/**
 * Manager for images.
 * @author thomas
 */
@PluginImplementation
public class ImageManagerTopImpl extends AbstractManagerPlugin<JanusImage>
  implements ImageManagerTop
{

  public ImageManagerTopImpl()
  {
  }

  @Override
  public JanusImage convertByteArrayToType(byte[] result) throws IllegalArgumentException
  {
    try
    {
      Image img = Image.parseFrom(result);

      BufferedImage dst = new BufferedImage(
        img.getWidth(), img.getHeight(), BufferedImage.TYPE_INT_RGB);
      
      ByteString src = img.getData();

      if(img.getFormat() == Image.Format.YUV) {
        ImageConversions.convertYUV888toYUV888(src, dst);
      } else if(img.getFormat() == Image.Format.YUV422) {
        ImageConversions.convertYUV422toYUV888(src, dst);
      } else if(img.getFormat() == Image.Format.JPEG) {
        dst = ImageIO.read( src.newInput() );
        return new JanusImage(dst, false);
      }

      return new JanusImage(dst, true);
    }
    catch(IOException ex)
    {
      Logger.getLogger(ImageManagerTopImpl.class.getName()).log(Level.SEVERE, "could not parse message", ex);
    }

    return null;
  }//end convertByteArrayToType

  @Override
  public Command getCurrentCommand()
  {
    //return new Command("image").addArg("top");
    return new Command("Cognition:representation:get").addArg("ImageJPEGTop");
  }

}//end class ImageManager

