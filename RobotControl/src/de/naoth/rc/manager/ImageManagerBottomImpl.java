/*
 * 
 */
package de.naoth.rc.manager;

import de.naoth.rc.dataformats.ImageConversions;
import de.naoth.rc.core.server.Command;
import net.xeoh.plugins.base.annotations.PluginImplementation;

/**
 * Manager for images.
 * @author thomas
 */
@PluginImplementation
public class ImageManagerBottomImpl extends ImageManager implements ImageManagerBottom
{

  private ImageConversions.Format format = ImageConversions.Format.JPEG;
    
  // the format in which to request the image from the robot
  @Override
  public void setFormat(ImageConversions.Format format) {
      this.format = format;
  }

  @Override
  public Command getCurrentCommand()
  {
    switch(format) {
        case JPEG:
            return new Command("Cognition:representation:get").addArg("ImageJPEG");
        case RAW:
        default:
            return new Command("image");
    }
  }

}//end class ImageManager

