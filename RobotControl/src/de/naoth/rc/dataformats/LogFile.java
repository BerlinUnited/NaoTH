/*
*
*/
package de.naoth.rc.dataformats;

import de.naoth.rc.tools.BasicReader;
import de.naoth.rc.logmanager.LogDataFrame;
import de.naoth.rc.tools.MemoryMapReader;
import de.naoth.rc.tools.RandomAccessReader;
import java.io.EOFException;
import java.io.File;
import java.io.IOException;
import java.io.Serializable;
import java.nio.channels.FileChannel;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author thomas
 * @author heinrich :P
 */
public class LogFile implements Serializable
{
  private final Logger logger;
  private Logger getLogger() { return logger; }
  
  private final File originalFile;
  private final ArrayList<Frame> frameList = new ArrayList<>();
  private transient BasicReader reader = null;

  private ProgressMonitor progressMonitor;
  
  public LogFile(String path) throws IOException
  {
    this(path, null, null);
  }
  
  public LogFile(File originalFile) throws IOException
  {
    this(originalFile, null, null);
  }
  
  public LogFile(String path, Logger logger, ProgressMonitor progressMonitor) throws IOException
  {
    this(new File(path), logger, progressMonitor);
  }
  
  public LogFile(File originalFile, Logger logger, ProgressMonitor progressMonitor) throws IOException
  {
    if(logger == null) {
        this.logger = Logger.getLogger(LogFile.class.getName());
    } else {
        this.logger = logger;
    }
    
    if(progressMonitor != null) {
        this.progressMonitor = progressMonitor;
    } else {
        // defualt progress monitor = print to logger
        this.progressMonitor = new ProgressMonitor() {
            @Override
            public void setMaximum(int v) { }
            @Override
            public void setValue(int v) {
                getLogger().info("" + v);
            }
        };
    }
    
    this.originalFile = originalFile;
    getReader();
  }
  
  public interface ProgressMonitor {
      public abstract void setMaximum(int v);
      public abstract void setValue(int v);
  }
  

  private void scan(BasicReader data_in) throws IOException
  {
    int currentFrameNumber = -1;
    long currentFrameSize = 0;
    long currentFramePos = 0;
    
    // needed for progress report
    long numberOfBytesRead = 0;
    int lastReportedProgress = 0;
    final long logfielSize = this.originalFile.length();
    Frame currentFrame = null;
    this.progressMonitor.setMaximum(100);
    
    try
    {
      while (true)
      {
        long fragmentFrameSize = 0;
        int frameNumber = data_in.readInt();
        fragmentFrameSize += 4;
        
        // plausibility check
        if (frameNumber < currentFrameNumber || frameNumber < 0) {
            throw new IOException("corrupt frame number: " + frameNumber + " after " + currentFrameNumber);
        }
        
        // warning
        if (currentFrameNumber >= 0 && frameNumber - currentFrameNumber > 30) {
          getLogger().warning("frame jump: " + currentFrameNumber + " -> " + frameNumber);
        }
        
        String currentName = data_in.readString();
        fragmentFrameSize += currentName.length() + 1;
        
        int currentSize = data_in.readInt();
        fragmentFrameSize += 4;
        fragmentFrameSize += currentSize;

        if (currentFrameNumber != frameNumber && currentFrameNumber != -1)
        {
          // store completely scanned frame in the list
          if(currentFrame != null) {
              frameList.add(currentFrame);
          }
          
          // create new frame
          currentFrame = new Frame(currentFrameNumber, currentFrameSize, currentFramePos);
          currentFramePos += currentFrameSize;
          currentFrameSize = 0;
        }
        
        currentFrameSize += fragmentFrameSize;
        currentFrameNumber = frameNumber;
        
        long skippedSize = data_in.skip(currentSize);
        
        // report progress
        numberOfBytesRead += fragmentFrameSize;
        int progress = (int)((numberOfBytesRead*100)/logfielSize);
        if(progress > lastReportedProgress) {
            lastReportedProgress = progress;
            this.progressMonitor.setValue(progress);
        }
      } //end while
    } 
    catch (EOFException eof) {
      getLogger().info("End of File");
    } catch(IllegalArgumentException ex) {
      getLogger().log(Level.SEVERE, "Error while scanning " + originalFile.getName(), ex);
    }
  } //end parseLogFile

  public HashMap<String, LogDataFrame> readFrame(int frameId) throws IOException
  {
    if (frameId < 0 || frameId >= this.frameList.size()) {
      return null;
    }

    Frame frame = this.frameList.get(frameId);
    if (frame == null)
    {
      return null;
    }
    // jump to the begin of the frame
    getReader().seek(frame.position);
    int numberOfReadBytes = 0;
    HashMap<String, LogDataFrame> currentFrame = new HashMap<>();
    
    while (numberOfReadBytes < frame.size)
    {
      int frameNumber = getReader().readInt();
      numberOfReadBytes += 4;
      
      // integrity check
      if (frameNumber != frame.number) {
        throw new IOException("corrupt frame number: " + frameNumber + " expected " + frame.number);
      }
      
      String currentName = getReader().readString();
      numberOfReadBytes += currentName.length() + 1;
      
      int currentSize = getReader().readInt();
      numberOfReadBytes += 4;
      
      byte[] buffer = new byte[currentSize];
      numberOfReadBytes += getReader().read(buffer);
      
      // integrity check: check and warn if a representation is logged several times in the same frame
      if(currentFrame.containsKey(currentName)) {
        getLogger().warning("Frame " + frameNumber + " contains several instaces of " + currentName);
      }
      
      LogDataFrame logDataFrame = new LogDataFrame(frameNumber, currentName, buffer);
      currentFrame.put(currentName, logDataFrame);
    } //end while
    
    return currentFrame;
  } //end readFrame

  class Frame implements Serializable
  {
    final int number;
    
    // size of the frame
    final long size;
    
    // starting position within the logfile
    final long position;

    public Frame(int number, long size, long position)
    {
      this.number = number;
      this.size = size;
      this.position = position;
    }
  } //end class Frame

  public int getFrameCount()
  {
    return frameList.size();
  }

  public File getOriginalFile()
  {
    return originalFile;
  }
  
  public void close() {
      if(this.reader != null) {
          try {
            this.reader.close();
          } catch(IOException ex) {
            getLogger().log(Level.SEVERE, "Error closing reader.", ex);
          }
      }
  }

  //TODO: why not create the reader in the constructor?
  private BasicReader getReader() throws IOException
  {
    if (reader == null)
    {
      FileChannel channel = FileChannel.open(originalFile.toPath());
      if (channel.size() >= Integer.MAX_VALUE) {
        reader = new RandomAccessReader(originalFile);
      } else {
        reader = new MemoryMapReader(channel);
      }
      scan(reader);
      return reader;
    } else {
      return reader;
    }
  }

}
