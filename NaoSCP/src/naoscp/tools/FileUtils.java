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
import java.util.zip.ZipFile;

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
    zipDirectory(fileToZip, target, null, null);
  }

  public static void zipDirectory(File fileToZip, File target, String firstName) throws NaoSCPException
  {
    zipDirectory(fileToZip, target, firstName, null);
  }

  public static void zipDirectory(File fileToZip, File target, String firstName, String comment) throws NaoSCPException
  {
      try {
        FileOutputStream fos = new FileOutputStream(target);
        ZipOutputStream zipOut = new ZipOutputStream(fos);
 
        String name = firstName == null ? fileToZip.getName() : firstName;
        zipFile(fileToZip, name, zipOut);
        if (comment != null) {
          zipOut.setComment(comment);
        }
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
                String childFileName = fileName.isEmpty() ? childFile.getName() : fileName + "/" + childFile.getName();
                zipFile(childFile, childFileName, zipOut);
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

    /**
     * Rename a zip file based on its comment (if set)
     * @param zipFile The zip file to rename
     * @param targetDirectory The directory where the renamed file should be placed (can be null for same directory)
     * @return The new File object if renamed, null if no comment was found or rename failed
     * @throws NaoSCPException if there's an error reading the zip file
     */
    public static File renameZipByComment(File zipFile, File targetDirectory) throws NaoSCPException
    {
        
        // read the comment from the file
        String comment = null;
        try (ZipFile zip = new ZipFile(zipFile)) {
            comment = zip.getComment();
        } catch (IOException ex) {
            throw new NaoSCPException("Error reading zip file: " + ex.getMessage());
        }
        // NOTE: the zip is automatically closed here
        //       if it makes probems, then use zip.close()
        
        if (comment == null || comment.trim().isEmpty()) {
            return null; // No comment found
        }

        // Clean the comment to make it a valid filename
        String cleanComment = comment.trim()
            .replaceAll("[<>:\"/\\|?*]", "_") // Replace invalid filename characters
            .replaceAll("\\s+", "_"); // Replace spaces with underscores

        if (cleanComment.isEmpty()) {
            return null; // Comment was only invalid characters
        }

        // Determine target directory
        File targetDir = targetDirectory != null ? targetDirectory : zipFile.getParentFile();
        if (targetDir == null) {
            targetDir = new File("."); // Current directory if no parent
        }

        // Create new filename with .zip extension
        String newFileName = cleanComment;
        if (!newFileName.toLowerCase().endsWith(".zip")) {
            newFileName += ".zip";
        }

        File newFile = new File(targetDir, newFileName);

        // Handle filename conflicts by adding a number
        int counter = 1;
        while (newFile.exists()) {
            String baseName = cleanComment;
            if (baseName.toLowerCase().endsWith(".zip")) {
                baseName = baseName.substring(0, baseName.length() - 4);
            }
            newFileName = baseName + "_" + counter + ".zip";
            newFile = new File(targetDir, newFileName);
            counter++;
        }

        // Rename the file
        if (zipFile.renameTo(newFile)) {
            return newFile;
        } else {
            throw new NaoSCPException("Failed to rename zip file from " + zipFile.getName() + " to " + newFileName);
        }
    }

    /**
     * Rename a zip file based on its comment in the same directory
     * @param zipFile The zip file to rename
     * @return The new File object if renamed, null if no comment was found or rename failed
     * @throws NaoSCPException if there's an error reading the zip file
     */
    public static File renameZipByComment(File zipFile) throws NaoSCPException
    {
        return renameZipByComment(zipFile, null);
    }
}
