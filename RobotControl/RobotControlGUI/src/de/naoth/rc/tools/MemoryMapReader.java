package de.naoth.rc.tools;

import java.io.File;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.nio.BufferUnderflowException;
import java.nio.MappedByteBuffer;
import java.nio.channels.FileChannel;

/**
 * A wrapper class for either {@link MappedByteBuffer} (fast) or
 * {@link RandomAccessFile} (slow) which has the necessary functionality
 * to read log files.
 * @author thomas
 */
public class MemoryMapReader extends BasicReader
{
  private MappedByteBuffer memoryMapped = null;
  private FileChannel channel = null;
  
  public MemoryMapReader(File file) throws IOException
  {
    channel = FileChannel.open(file.toPath());
    this.memoryMapped = channel.map(FileChannel.MapMode.READ_ONLY, 0, channel.size());
  }
  
  public MemoryMapReader(FileChannel channel) throws IOException
  {
    this.memoryMapped = channel.map(FileChannel.MapMode.READ_ONLY, 0, channel.size());
    this.channel = channel;
  }

  @Override
  public void close() throws IOException {
      if(channel != null) {
          this.memoryMapped = null;
          channel.close();
      }
  }

  @Override
  public int read(byte[] buffer) throws IOException
  {
    if(buffer != null)
    {
      int length = Math.min(buffer.length, memoryMapped.remaining());
      if(length > 0)
      {
        memoryMapped.get(buffer, 0, length);
        return length;
      }
    }
    return 0;
  }

  @Override
  public int read() throws IOException
  {
    try
    {
      return memoryMapped.get();
    }
    catch(BufferUnderflowException ex)
    {
      return -1;
    }
  }

  @Override
  public int skip(int size) throws IOException
  {
    int length = Math.min(size, memoryMapped.remaining());
    int oldPos = memoryMapped.position();
    memoryMapped.position(oldPos+length);
    return length;
  }

  @Override
  public void seek(long size) throws IOException
  {
    memoryMapped.position((int) size);
  }

} //end class MamoryMapReader
