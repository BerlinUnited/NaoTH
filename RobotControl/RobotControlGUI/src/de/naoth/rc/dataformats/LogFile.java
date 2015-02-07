/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc.dataformats;

import de.naoth.rc.tools.BasicReader;
import de.naoth.rc.logmanager.LogDataFrame;
import java.io.EOFException;
import java.io.File;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.util.ArrayList;
import java.util.HashMap;

/**
 *
 * @author thomas
 */
public class LogFile
{
  private int minFrame = Integer.MAX_VALUE;
  private int maxFrame = Integer.MIN_VALUE;
  private ArrayList<Frame> frameList = new ArrayList<Frame>();
  private HashMap<Integer, Integer> framePosition = new HashMap<Integer, Integer>();
  private BasicReader reader = null;
 
  public LogFile(File file) throws IOException
  {
    //FileInputStream file_input = new FileInputStream (file);
    //DataInputStream inputStream = new DataInputStream (file_input);
    reader = new BasicReader(new RandomAccessFile(file, "r"));
    scan(reader);
    //this.raf = new RandomAccessFile(this.openedFile, "r");
  }

  private void scan(BasicReader data_in) throws IOException
  {
    int currentFrameNumber = -1;
    int currentFrameSize = 0;
    int currentFramePos = 0;
    while (true)
    {
      try
      {
        int fragmentFrameSize = 0;
        int frameNumber = data_in.readInt();
        fragmentFrameSize += 4;
        // plausibility check
        if (currentFrameNumber > -1 && (frameNumber < currentFrameNumber || frameNumber < 0))
        {
          throw new IOException("corrupt frame number: " + frameNumber + " after " + currentFrameNumber);
        }
        if (frameNumber - currentFrameNumber > 30)
        {
          System.out.println("frame jump: " + currentFrameNumber + " -> " + frameNumber);
        }
        String currentName = data_in.readString();
        fragmentFrameSize += currentName.length() + 1;
        int currentSize = data_in.readInt();
        fragmentFrameSize += 4;
        fragmentFrameSize += currentSize;
        this.minFrame = Math.min(this.minFrame, frameNumber);
        this.maxFrame = Math.max(this.maxFrame, frameNumber);
        if (currentFrameNumber != frameNumber && currentFrameNumber != -1)
        {
          Frame frame = new Frame(currentFrameNumber, currentFrameSize, currentFramePos);
          framePosition.put(frameList.size(), currentFrameNumber);
          frameList.add(frame);
          currentFramePos += currentFrameSize;
          currentFrameSize = 0;
        } //end if
        currentFrameSize += fragmentFrameSize;
        currentFrameNumber = frameNumber;
        long skippedSize = data_in.skip(currentSize);
      } catch (EOFException eof)
      {
        System.out.println("End of File");
        break;
      }
    } //end while
  } //end parseLogFile

  public HashMap<String, LogDataFrame> readFrame(int frameId) throws IOException
  {
    Frame frame = this.frameList.get(frameId);
    if (frame == null)
    {
      return null;
    }
    // jump to the begin of the frame
    reader.seek(frame.position);
    int numberOfReadBytes = 0;
    HashMap<String, LogDataFrame> currentFrame = new HashMap<>();
    while (numberOfReadBytes < frame.size)
    {
      int frameNumber = reader.readInt();
      numberOfReadBytes += 4;
      if (frameNumber != frame.number)
      {
        throw new IOException("corrupt frame number: " + frameNumber + " expected " + frame.number);
      }
      String currentName = reader.readString();
      numberOfReadBytes += currentName.length() + 1;
      int currentSize = reader.readInt();
      numberOfReadBytes += 4;
      byte[] buffer = new byte[currentSize];
      numberOfReadBytes += reader.read(buffer);
      LogDataFrame logDataFrame = new LogDataFrame(frameNumber, currentName, buffer);
      currentFrame.put(currentName, logDataFrame);
    } //end while
    return currentFrame;
  } //end readFrame

  class Frame
  {

    final int number;
    final int size;
    final int position;

    public Frame(int number, int size, int position)
    {
      this.number = number;
      this.size = size;
      this.position = position;
    }
  } //end class Frame

  public int getMinFrame()
  {
    return minFrame;
  }

  public int getMaxFrame()
  {
    return maxFrame;
  }

  public int getFrameCount()
  {
    return frameList.size();
  }

  public int getFrameNumber(int n)
  {
    Frame frame = this.frameList.get(n);
    if (frame == null)
    {
      return -1;
    }
    return frame.number;
  }
  
}
