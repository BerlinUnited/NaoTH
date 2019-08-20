package de.naoth.rc.tools;

import java.io.File;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.nio.MappedByteBuffer;

/**
 * A wrapper class for either {@link MappedByteBuffer} (fast) or
 * {@link RandomAccessFile} (slow) which has the necessary functionality
 * to read log files.
 * @author thomas
 */
public class RandomAccessReader extends BasicReader
{
  private RandomAccessFile rafReader = null;

  public RandomAccessReader(File file) throws IOException
  {
    this.rafReader = new RandomAccessFile(file, "r");
  }
  
  @Override
  public void close() throws IOException {
      if(rafReader != null) {
        rafReader.close();
      }
  }

  @Override
  public int read(byte[] buffer) throws IOException
  {
    return this.rafReader.read(buffer);
  }

  @Override
  public int read() throws IOException
  {
    return this.rafReader.read();
  }

  @Override
  public int skip(int size) throws IOException
  {
    return this.rafReader.skipBytes(size);
  }

  @Override
  public void seek(long size) throws IOException
  {
    this.rafReader.seek(size);
  }
  
} //end class RandomAccessReader
