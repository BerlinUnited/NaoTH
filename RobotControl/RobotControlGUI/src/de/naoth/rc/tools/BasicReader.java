/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc.tools;

import java.io.EOFException;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.nio.BufferUnderflowException;
import java.nio.MappedByteBuffer;

/**
 * A wrapper class for either {@link MappedByteBuffer} (fast) or
 * {@link RandomAccessFile} (slow) which has the necessary functionality
 * to read log files.
 * @author thomas
 */
public class BasicReader
{
  private MappedByteBuffer memoryMapped = null;
  private RandomAccessFile rafReader = null;

  public BasicReader(MappedByteBuffer memoryMapped)
  {
    this.memoryMapped = memoryMapped;
  }

  public BasicReader(RandomAccessFile rafReader)
  {
    this.rafReader = rafReader;
  }

  public int read(byte[] buffer) throws IOException
  {
    if(memoryMapped == null)
    {
      return this.rafReader.read(buffer);
    }
    else if(buffer != null)
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

  public int read() throws IOException
  {
    if(memoryMapped == null)
    {
      return this.rafReader.read();
    }
    else
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
  }

  public int skip(int size) throws IOException
  {
    if(memoryMapped == null)
    {
      return this.rafReader.skipBytes(size);
    }
    else
    {
      int oldPos = memoryMapped.position();
      memoryMapped.position(oldPos+size);
      return memoryMapped.position()-oldPos;
    }
  }

  public void seek(long size) throws IOException
  {
    if(memoryMapped == null)
    {
      this.rafReader.seek(size);
    }
    else
    {
      memoryMapped.position((int) size);
    }
  }

  public String readString() throws IOException, EOFException
  {
    StringBuilder sb = new StringBuilder();
    int c = this.read();
    while (((char) c) != '\0')
    {
      if (c == -1)
      {
        throw new EOFException();
      }
      sb.append((char) c);
      c = this.read();
    }
    return sb.toString();
  }

  public int readInt() throws IOException
  {
    byte[] buffer = new byte[4];
    int numberOfBytes = this.read(buffer);
    if (numberOfBytes != 4)
    {
      throw new EOFException();
    }
    int result = (0xff000000 & buffer[3] << 32) | (0x00ff0000 & buffer[2] << 16) | (0x0000ff00 & buffer[1] << 8) | (0x000000ff & buffer[0]);
    return result;
  }
  
} //end class BasicReader
