/*
 * 
 */

package de.naoth.rc.components;

import java.io.File;
import javax.swing.JFileChooser;
import javax.swing.JOptionPane;

/**
 *
 * @author Heinrich Mellmann
 */
public class ExtendedFileChooser extends JFileChooser
{

    public ExtendedFileChooser()
    {
        super();
    }
    
    @Override
    public void approveSelection() 
    {
        if(this.getDialogType() == SAVE_DIALOG && !validateFile(this.getSelectedFile()))
        {
            return;
        }
        super.approveSelection();
    }//end approveSelection
    
    /**
     * checks whether a file exists
     * @param file
     * @return
     */
    private boolean validateFile(File file)
    {
        //do your validation here
        if (file.exists())
        {
            int result = JOptionPane.showConfirmDialog(this, 
                    "File exists,overwrite?", 
                    "Overwrite File?", JOptionPane.YES_NO_OPTION);

            if (result == JOptionPane.YES_OPTION)
            {
                return true;
            }
            return false;
        }//end if

        return true;
    }//end validateFile
    
}//end ExtendedFileChooser
