/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.naoth.rc.manager;

import de.naoth.rc.core.manager.ManagerPlugin;
import de.naoth.rc.dataformats.ImageConversions;
import de.naoth.rc.dataformats.JanusImage;

/**
 *
 * @author thomas
 */
public interface ImageManagerTop extends ManagerPlugin<JanusImage>
{
    // the format in which to request the image from the robot
    void setFormat(ImageConversions.Format format);
}
