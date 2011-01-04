/*
 * 
 */
package de.hu_berlin.informatik.ki.nao.manager;

import com.google.protobuf.ByteString;
import com.google.protobuf.InvalidProtocolBufferException;
import de.hu_berlin.informatik.ki.nao.dataformats.JanusImage;
import de.hu_berlin.informatik.ki.nao.messages.ImageMsg.Image;
import de.hu_berlin.informatik.ki.nao.server.Command;
import java.awt.image.BufferedImage;
import java.util.logging.Level;
import java.util.logging.Logger;
import net.xeoh.plugins.base.annotations.PluginImplementation;

/**
 * Manager for images.
 * @author thomas
 */
@PluginImplementation
public class ImageManager extends AbstractManager<JanusImage>
{

  public ImageManager()
  {
  }

  @Override
  public JanusImage convertByteArrayToType(byte[] result) throws IllegalArgumentException
  {
    JanusImage janusImage = null;
    try
    {
      Image img = Image.parseFrom(result);

      int width = img.getWidth();
      int height = img.getHeight();

      BufferedImage newImage = new BufferedImage(
        width, height, BufferedImage.TYPE_INT_ARGB);

      int[] rgbBuffer = new int[width * height];

      ByteString bs = img.getData();
      if(img.getFormat() == Image.Format.YUV)
      {
        for(int i = 0; i < width * height; i++)
        {
          byte y = bs.byteAt(i*3);
          byte u = bs.byteAt(i * 3 + 1);
          byte v = bs.byteAt(i * 3 + 2);

          rgbBuffer[i] =
            0xff000000 |
            (0x00ff0000 & y << 16) |
            (0x0000ff00 & u << 8) |
            (0x000000ff & v);
        }
      }
      else if(img.getFormat() == Image.Format.YUV422)
      {
        for(int i = 0; i < (bs.size()/4); i++)
        {
          byte y1 = bs.byteAt(i * 4);
          byte u = bs.byteAt(i * 4 + 1);
          byte y2 = bs.byteAt(i * 4 + 2);
          byte v = bs.byteAt(i * 4 + 3);

          rgbBuffer[i*2] =
            0xff000000 |
            (0x00ff0000 & y1 << 16) |
            (0x0000ff00 & u << 8) |
            (0x000000ff & v);

          rgbBuffer[i*2+1] =
            0xff000000 |
            (0x00ff0000 & y2 << 16) |
            (0x0000ff00 & u << 8) |
            (0x000000ff & v);
        }
      }
      newImage.setRGB(0, 0, width, height, rgbBuffer, 0, width);

      return new JanusImage(newImage);
    }
    catch(InvalidProtocolBufferException ex)
    {
      Logger.getLogger(ImageManager.class.getName()).log(Level.SEVERE, "could not parse stream", ex);
    }

    return janusImage;
  }//end convertByteArrayToType

  @Override
  public Command getCurrentCommand()
  {
    return new Command("image");
  }//end getCurrentCommand
}//end class ImageManager

