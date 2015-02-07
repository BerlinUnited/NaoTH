/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc.tools;

import java.io.EOFException;
import java.io.IOException;
import java.io.InputStream;
import java.io.RandomAccessFile;

/**
 *
 * @author thomas
 */
public class BasicReader
{
  private InputStream streamReader = null;
  private RandomAccessFile rafReader = null;

  public BasicReader(InputStream streamReader)
  {
    this.streamReader = streamReader;
  }

  public BasicReader(RandomAccessFile rafReader)
  {
    this.rafReader = rafReader;
  }

  public int read(byte[] buffer) throws IOException
  {
    return (streamReader == null) ? this.rafReader.read(buffer) : this.streamReader.read(buffer);
  }

  public int read() throws IOException
  {
    return (streamReader == null) ? this.rafReader.read() : this.streamReader.read();
  }

  public long skip(int size) throws IOException
  {
    return (streamReader == null) ? this.rafReader.skipBytes(size) : this.streamReader.skip(size);
  }

  public void seek(long size) throws IOException
  {
    this.rafReader.seek(size);
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
