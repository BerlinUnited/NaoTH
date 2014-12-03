/*
 */

package de.naoth.rc.dialogs.Tools;

import java.io.File;

/**
 *
 * @author Heinrich Mellmann
 */
public class PNGImageFileFilter extends javax.swing.filechooser.FileFilter
{

  final private String fileExtension = "png";
  final private String description = "Portable Network Graphics (*.png)";

  @Override
  public boolean accept(File file)
  {
        if(file.isDirectory()) return true;
    String filename = file.getName();
    return filename.toLowerCase().endsWith("." + this.fileExtension);
  }

  @Override
  public String getDescription()
  {
    return this.description;
  }

  @Override
  public String toString()
  {
    return fileExtension;
  }
}//end class PNGImageFileFilter
