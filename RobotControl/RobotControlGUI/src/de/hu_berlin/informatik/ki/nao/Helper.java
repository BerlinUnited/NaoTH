/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package de.hu_berlin.informatik.ki.nao;

import de.hu_berlin.informatik.ki.nao.server.MessageServer;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.UnsupportedEncodingException;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.JOptionPane;

/**
 *
 * @author thomas
 */
public class Helper
{

  public static void handleException(String message, Exception ex)
  {
    // log
    Logger.getLogger("RobotControl").log(Level.SEVERE, message, ex);
    // show
    ExceptionDialog dlg = new ExceptionDialog(null, message, ex);
    dlg.setVisible(true);
  }

  public static void handleException(Exception ex)
  {
    handleException(null, ex);
  }

  /**
   * Checks if a  we are connected to a server and if not displays an
   * error message to the user.
   */
  public static boolean checkConnected(MessageServer server)
  {
    if(server.isConnected())
    {
      return true;
    }
    else
    {
      JOptionPane.showMessageDialog(null,
        "Not connected. Please connect first to a robot.", "WARNING",
        JOptionPane.WARNING_MESSAGE);
      return false;
    }
  }//end checkConnected


  /**
   * Reads a resource file as string.
   */
  public static String getResourceAsString(String name)
  {
    InputStream stream = (new Object()).getClass().getResourceAsStream(name);
    StringBuilder builder = new StringBuilder();

    if(stream != null)
    {
      try{
        while (stream.available() > 0)
        {
          builder.append((char)stream.read());
        }
      }catch(IOException e)
      {
        Helper.handleException(e);
      }
      return builder.toString();
    }//end if
    
    return null;
  }//end getResourceAsString

  /**
   * Validates the name of a given file, i.e., if the file name doesn't have
   * the proper ending, it will be appended.
   * @param file the file which name has to be validated
   * @param filter a file filter containing the definition of the file ending.
   * @return a file with validated file name
   */
  public static File validateFileName(final File file, final javax.swing.filechooser.FileFilter filter)
  {
    if(file == null || filter == null || filter.accept(file))
    {
      return file;
    }
    // remove wrong file extension if any
    String fileName = file.getName();
    int index = fileName.lastIndexOf(".");
    if(index > 0)
    {
      fileName = fileName.substring(0, index);
    }

    final String extension = filter.toString();
    
    if(extension.matches("(\\w)*"))
    {
      String newFileName = fileName + "." + extension;
      File newFile = new File(file.getParent(), newFileName);
      return newFile;
    }//end if

    return file;
  }//end validateFileName

  /** Hashes a string using SHA-256. */
  public static String calculateSHAHash(String s) throws NoSuchAlgorithmException, UnsupportedEncodingException
  {
    MessageDigest md = MessageDigest.getInstance("SHA-256");
    md.update(s.getBytes("UTF-8"));
    byte[] digest = md.digest();

    String hashVal = "";
    for(byte b : digest)
    {
      hashVal += String.format("%02x", b);
    }

    return hashVal;
  }
  
}//end class Helper
