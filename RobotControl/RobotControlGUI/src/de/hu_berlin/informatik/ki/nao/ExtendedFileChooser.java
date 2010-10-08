/*
 * 
 */

package de.hu_berlin.informatik.ki.nao;

import java.io.File;
import javax.swing.JFileChooser;

/**
 *
 * @author Heinrich Mellmann
 */
public class ExtendedFileChooser extends JFileChooser
{
  @Override
  public File getSelectedFile() {
    return Helper.validateFileName(super.getSelectedFile(), super.getFileFilter());
  }//end getSelectedFile
}//end ExtendedFileChooser
