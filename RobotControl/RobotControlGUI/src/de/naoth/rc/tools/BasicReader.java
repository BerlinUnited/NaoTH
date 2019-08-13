
package de.naoth.rc.tools;

import java.io.EOFException;
import java.io.IOException;

/**
 *
 * @author Heinrich 
 */
public abstract class BasicReader 
{
    public abstract void close() throws IOException;
    public abstract int read(byte[] buffer) throws IOException;
    public abstract int read() throws IOException;
    public abstract int skip(int size) throws IOException;
    public abstract void seek(long size) throws IOException;
    
    
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
      int result = (0xff000000 & buffer[3] << 32) | 
                   (0x00ff0000 & buffer[2] << 16) | 
                   (0x0000ff00 & buffer[1] << 8)  | 
                   (0x000000ff & buffer[0]);
      return result;
    }
}
