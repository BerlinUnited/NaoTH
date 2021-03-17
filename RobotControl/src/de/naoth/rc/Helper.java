/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc;

import de.naoth.rc.components.ExceptionDialog;
import de.naoth.rc.core.server.MessageServer;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.UnsupportedEncodingException;
import java.net.URI;
import java.net.URISyntaxException;
import java.nio.file.FileSystem;
import java.nio.file.FileSystems;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.Collections;
import java.util.List;
import java.util.function.Predicate;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.stream.Collectors;
import java.util.stream.Stream;
import javax.swing.JOptionPane;
import javax.swing.SwingUtilities;

/**
 *
 * @author thomas
 */
public class Helper
{

  public static void handleException(final String message, final Exception ex)
  {
    // log
    Logger.getLogger("RobotControl").log(Level.SEVERE, message, ex);
    // show
    if (SwingUtilities.isEventDispatchThread())
    {
      ExceptionDialog dlg = new ExceptionDialog(null, message, ex);
      dlg.setVisible(true);
    } else
    {
      SwingUtilities.invokeLater(new Runnable()
      {

        @Override
        public void run()
        {
          ExceptionDialog dlg = new ExceptionDialog(null, message, ex);
          dlg.setVisible(true);
        }
      });
    }
  }

  public static void handleException(Exception ex)
  {
    handleException(null, ex);
  }

  /**
   * Checks if a we are connected to a server and if not displays an error
   * message to the user.
   */
  public static boolean checkConnected(MessageServer server)
  {
    if (server.isConnected())
    {
      return true;
    } else
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

    if (stream != null)
    {
      try
      {
        while (stream.available() > 0)
        {
          builder.append((char) stream.read());
        }
      } catch (IOException e)
      {
        Helper.handleException(e);
      }
      return builder.toString();
    }//end if

    return null;
  }//end getResourceAsString

    public static List<Path> getFiles(String location) {
        return getFiles(location, null);
    }

    public static List<Path> getFiles(String location, Predicate<Path> filter) {
        try {
            URI res = Helper.class.getResource(location).toURI();
            
            // handle current execution location (jar, file)
            Path resPath;
            if (res.getScheme().equals("jar")) {
                FileSystem fs = FileSystems.newFileSystem(res, Collections.<String, Object>emptyMap());
                resPath = fs.getPath(location);
            } else {
                resPath = Paths.get(res);
            }

            Stream<Path> s = Files.walk(resPath, 1);
            
            if (filter != null) { s = s.filter(filter); }

            return s.sorted().collect(Collectors.toList());
        } catch (IOException | URISyntaxException e) {
            Logger.getLogger(HelpDialog.class.getName()).log(Level.SEVERE, null, e);
        }

        return Collections.emptyList();
    }

  /**
   * Validates the name of a given file, i.e., if the file name doesn't have the
   * proper ending, it will be appended.
   *
   * @param file the file which name has to be validated
   * @param filter a file filter containing the definition of the file ending.
   * @return a file with validated file name
   */
  public static File validateFileName(final File file, final javax.swing.filechooser.FileFilter filter)
  {
    if (file == null || filter == null || filter.accept(file))
    {
      return file;
    }
    // remove wrong file extension if any
    String fileName = file.getName();
    int index = fileName.lastIndexOf(".");
    if (index > 0)
    {
      fileName = fileName.substring(0, index);
    }

    final String extension = filter.toString();

    if (extension.matches("(\\w)*"))
    {
      String newFileName = fileName + "." + extension;
      File newFile = new File(file.getParent(), newFileName);
      return newFile;
    }//end if

    return file;
  }//end validateFileName

  /**
   * Hashes a string using SHA-256.
   */
  public static String calculateSHAHash(String s) throws NoSuchAlgorithmException, UnsupportedEncodingException
  {
    MessageDigest md = MessageDigest.getInstance("SHA-256");
    md.update(s.getBytes("UTF-8"));
    byte[] digest = md.digest();

    String hashVal = "";
    for (byte b : digest)
    {
      hashVal += String.format("%02x", b);
    }

    return hashVal;
  }

}//end class Helper
