/*
 * 
 */

package naoscp.tools;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author Heinrich Mellmann
 */
public class FileUtils 
{
  public static void writeToFile(String msg, File file) throws IOException
  {
      BufferedWriter writer = new BufferedWriter(new FileWriter(file));
      writer.write(msg);
      writer.close();
  }
  
  public static String readFile(File file) throws IOException
  {
      BufferedReader br = new BufferedReader(new FileReader(file));
      String line = "";
      String fileContent = "";
      while (line != null) {
          line = br.readLine();
          if (line != null) {
              fileContent += line + "\n";
          }
      }
      br.close();
      return fileContent;
  }
  
  /**
   * recursively delete local dir
   * @param dir File dir
   */
  public static void deleteDir(File dir)
  {
    if(dir.isDirectory())
    {
      File fileList[] = dir.listFiles();
      for(int index = 0; index < fileList.length; index++)
      {
        File file = fileList[index];
        deleteDir(file);
      }
    }
    dir.delete();
  }//end deleteDir

  
  /**
   * recursively copy local files, skips .svn
   *
   * @param src File source
   * @param dest File destination
   */
  public static void copyFiles(File src, File dest)
  {
    if( ! src.exists() ||  ! src.canRead() || src.getName().equals(".svn") || src.getName().equals(".bzr") || src.getName().equals(".hg") || src.getName().equals(".git"))
    {
      return;
    }
    if(src.isDirectory())
    {
      if( ! dest.exists())
      {
        if( ! dest.mkdirs())
        {
          Logger.getGlobal().log(Level.SEVERE, "copyFiles: Could not create direcotry: " + dest.getAbsolutePath() + ".");
          return;
        }
      }
      String list[] = src.list();
      for(int i = 0; i < list.length; i ++)
      {
        File dest1 = new File(dest, list[i]);
        File src1 = new File(src, list[i]);
        copyFiles(src1, dest1);
      }
    }
    else if(dest.isDirectory()) // copy file to a directory
    {
        copyFiles(src, new File(dest,src.getName()));
    }
    else // copy file to file
    {
      try
      {
        Logger.getGlobal().log(Level.INFO, "copy " + src.getName());
        byte[] buffer = new byte[4096]; //Buffer 4K at a time (you can change this).
        int bytesRead;
        
        FileInputStream fin = new FileInputStream(src);
        FileOutputStream fout = new FileOutputStream(dest);

        while((bytesRead = fin.read(buffer)) >= 0)
        {
          fout.write(buffer, 0, bytesRead);
        }
        fin.close();
        fout.close();
      }
      catch(IOException e)
      {
        e.printStackTrace(System.err);
        Logger.getGlobal().log(Level.SEVERE, "copyFiles: Unable to copy file: " + src.getAbsolutePath() + " to " + dest.
        getAbsolutePath() + ".");
      }
    }
  }//end copyFiles
}
