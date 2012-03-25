package de.naoth.me.core;

import java.io.*;


/**
 *
 * @author claas
 */
public class Scanner
{
    private Character LineComment = '#';
    private Character nextChar = ' ';
    //private int bufferLength = 0;
    private int lineNumber = 1;
    public String buffer = "";
    private Reader inputStream = null;

    //Constructor
    public Scanner(Reader input)
    {
        inputStream = input;
    }
    //end Constructor

    public void setLineCommentChar(Character LineCommentChar)
    {
      this.LineComment = LineCommentChar;
    }
    
    public boolean endOfFile()
    {
        return eof();
    }
    //end endOfFile

    public void getNextToken()
    {
      skipSpace();
      //bufferLength = 0;
      buffer = "";

      if(eof())
      {
        buffer += nextChar;
        //buffer[bufferLength++] = nextChar;
        //buffer[bufferLength] = '\0';
        return;
      }//end if

      if(!isTokenChar(nextChar))
      {
        buffer += nextChar;
        //buffer[bufferLength++] = nextChar;
        //buffer[bufferLength] = '\0';
        getNextChar();
        return;
      }//end if

      while(isTokenChar(nextChar))
      {
        buffer += nextChar;
        //buffer[bufferLength++] = nextChar;
        //buffer[bufferLength] = '\0';
        getNextChar();
      }//end while
    }
    //end getNextToken


    /*
     * Return true if c is a token character,
     * i.e., 0-1, a-z, A-Z, or one of the character '.','-' or '_'
     */
    public boolean isTokenChar(char c)
    {
      return Character.isLetterOrDigit(c) || c == '.' || c == '-' || c == '_';
    }
    //end isTolenChar


    public boolean isToken(String token)
    {
//      System.out.println("a:" + (buffer.compareTo(token) == 0) + ":e");
      return buffer.compareTo(token) == 0;
    }
    //end isToken

    private boolean eof()
    {
      return nextChar == -1; //EOF
    }
    //end eof

    private void skipSpace()
    {
      while(Character.isWhitespace(nextChar) || (nextChar == LineComment && LineComment != null) )
      {
        if(nextChar == '\n')
          lineNumber++;

        if(nextChar == LineComment && LineComment != null)
          skipLine();
        else
          getNextChar();
      }
    }
    //end skipSpace


    private void skipLine()
    {
      while(!eof() && nextChar != '\n')
      {
        getNextChar();
      }//end while

      getNextChar();
      lineNumber++;
    }
    //end skipLine


    public void getNextChar() 
    {
      try
      {
        if(!eof())
          nextChar = (char) inputStream.read();
      }
      catch (IOException e)
      {
        nextChar = (char) -1;
      }
      //else
        //nextChar = -1; //EOF
    }
    //end getNextChar


    public int getLineNumber()
    {
      return lineNumber;
    }
    //end getLineNumber
}
