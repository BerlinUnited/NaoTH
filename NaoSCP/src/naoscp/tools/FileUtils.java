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
import java.util.zip.ZipEntry;
import java.util.zip.ZipOutputStream;

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
      FileReader fr = new FileReader(file);
      BufferedReader br = new BufferedReader(fr);
      StringBuilder sb = new StringBuilder();

      while (br.ready()) {
          sb.append((char)br.read());
      }
      
      br.close();
      return sb.toString();
  }
  
  /**
   * recursively delete local dir
   * @param dir File dir
   */
  public static void deleteDir(File dir) throws NaoSCPException
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
    if(!dir.delete()) {
        throw new NaoSCPException("could not delete " + dir.getAbsolutePath());
    }
  }//end deleteDir

  
  /**
   * recursively copy local files, skips .svn
   *
   * @param src File source
   * @param dest File destination
   */
  public static void copyFiles(File src, File dest) throws NaoSCPException
  {
    // ignore versioning files
    if(src.getName().equals(".svn") || src.getName().equals(".bzr") || src.getName().equals(".hg") || src.getName().equals(".git"))
    {
      return;
    }
    
    if(!src.exists()) {
        throw new NaoSCPException("copyFiles: source path doesn't exist: " + src.getAbsolutePath());
    }
    
    if(src.isDirectory())
    {
      if( ! dest.exists())
      {
        if( ! dest.mkdirs())
        {
          throw new NaoSCPException("copyFiles: Could not create directory: " + dest.getAbsolutePath() + ".");
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
        Logger.getGlobal().log(Level.FINE, "copy " + src.getName());
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
        throw new NaoSCPException("copyFiles: Unable to copy file: " + src.getAbsolutePath() + " to " + dest.
        getAbsolutePath() + ".");
      }
    }
  }//end copyFiles
  
  
  public static void zipDirectory(File fileToZip, File target) throws NaoSCPException
  {
      try {
        FileOutputStream fos = new FileOutputStream(target);
        ZipOutputStream zipOut = new ZipOutputStream(fos);
 
        zipFile(fileToZip, fileToZip.getName(), zipOut);
        zipOut.close();
        fos.close();
      } catch (IOException ex) {
          throw new NaoSCPException("copyFiles: Could not create direcotry: ");
      }
    }
 
    private static void zipFile(File fileToZip, String fileName, ZipOutputStream zipOut) throws IOException 
    {
        if (fileToZip.isHidden()) {
            return;
        }
        if (fileToZip.isDirectory()) {
            File[] children = fileToZip.listFiles();
            for (File childFile : children) {
                zipFile(childFile, fileName + "/" + childFile.getName(), zipOut);
            }
            return;
        }
        FileInputStream fis = new FileInputStream(fileToZip);
        ZipEntry zipEntry = new ZipEntry(fileName);
        zipOut.putNextEntry(zipEntry);
        byte[] bytes = new byte[1024];
        int length;
        while ((length = fis.read(bytes)) >= 0) {
            zipOut.write(bytes, 0, length);
        }
        fis.close();
    }

}
