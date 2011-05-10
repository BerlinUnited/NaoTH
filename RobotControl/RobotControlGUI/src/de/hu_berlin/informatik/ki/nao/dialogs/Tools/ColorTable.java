/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.hu_berlin.informatik.ki.nao.dialogs.Tools;

import java.awt.Color;

/**
 *
 * @author Heinrich Mellmann
 */
public class ColorTable
{
    private char[][][] colorClasses;

    public ColorTable()
    {
      colorClasses = new char[64][64][64];
    }

    public ColorTable(ColorTable copy)
    {
      colorClasses = new char[64][64][64];
      createFromCharArray(copy.getCharArray());
    }

    // set a color cube
    public void setColor(char x, char y, char z, Colors.ColorClass newColorClass, int size)
    {
        for(int i = Math.max(x - size,0); i <= Math.min(x + size, 63); i++)
            for(int j = Math.max(y - size,0); j <= Math.min(y + size, 63); j++)
                for(int k = Math.max(z - size,0); k <= Math.min(z + size, 63); k++)
                  setColor((char)i, (char)j, (char)k, newColorClass);
    }//end setColor

    // check the size outside
    public void setColor(char x, char y, char z, Colors.ColorClass newColorClass)
    {
      if(x < 64 && y < 64 && z < 64)
      {
        colorClasses[x][y][z] = (char) newColorClass.ordinal();
      }
    }//end setColor

    
    // get the array only once to save time
    private Colors.ColorClass[] colorClassValues = Colors.ColorClass.values();
    public Colors.ColorClass getColor(char x, char y, char z)
    {
      if(x < 64 && y < 64 && z < 64)
      {
        char val = colorClasses[x][y][z];
        if(val >= 0 && val < colorClassValues.length)
        {
          return colorClassValues[colorClasses[x][y][z]];
        }
        else
        {
          return Colors.ColorClass.none;
        }
      }
      else
      {
        return Colors.ColorClass.none;
      }
    }//end getColor

    public void clearColor(Colors.ColorClass colorToDelete)
    {
      byte c = (byte)colorToDelete.ordinal();
      for (int x = 0; x<64; x++)
        for (int y = 0; y<64; y++)
          for (int z = 0; z<64; z++)
            if (colorClasses[x][y][z]==c)
              colorClasses[x][y][z]=(char)Colors.ColorClass.none.ordinal();
    }//end clearColor

    public void createFromCharArray(char[] bytes)
    {
      int pos = 0;
      //WARNING!! The colortable is not in the YUV colorspace. 
      //It's in the YVU colorspace.

      for(int x = 0; x < 64; x++)
      {
        for(int y = 0; y < 64; y++)
        {
          for(int z = 0; z < 64; z++)
          {
            colorClasses[x][y][z] = bytes[pos++];
          }
        }
      }
    }//end createFromCharArray

    public Color convertToYUV(Color c)
    {
      int y =        (int)( 0.2990 * c.getRed()+ 0.5870 * c.getGreen() + 0.1140 * c.getBlue());
      int cb = 128 + (int)(-0.1687 * c.getRed() - 0.3313 * c.getGreen() + 0.5000 * c.getBlue()); //officially U
      int cr = 128 + (int)( 0.5000 * c.getRed() - 0.4187 * c.getGreen() - 0.0813 * c.getBlue()); //officially V
      if(y < 0) y = 0;   else if(y > 255) y = 255;
      if(cb < 0) cb = 0; else if(cb > 255) cb = 255;
      if(cr < 0) cr = 0; else if(cr > 255) cr = 255;
      return new Color(y,cb,cr);
    }//end convertToYUV

    public char[] getCharArray()
    {
      char[] colorTable = new char[64 * 64 * 64];
      int pos = 0;
      for(int x = 0; x < 64; x++)
      {
        for(int y = 0; y < 64; y++)
        {
          for(int z = 0; z < 64; z++)
          {
             colorTable[pos++] = (char)colorClasses[x][y][z];
          }
        }
      }
      return colorTable;
    }//end getCharArray

    public void dilate(boolean strel[][][], Colors.ColorClass color, boolean eraseOtherColors)
    {
        int strelX=strel.length;
        int strelY=strel[0].length;
        int strelZ=strel[0][0].length;

        for(char x=0; x<colorClasses.length; x++)
            for(char y=0; y<colorClasses[0].length; y++)
                for(char z=0; z<colorClasses[0][0].length; z++)
                    if(getColor(x, y, z)==color)
                    for(int a=0; a<strelX; a++)
                        for(int b=0; b<strelY; b++)
                            for(int c=0; c<strelZ; c++)
                            {
                                int currentX=x - strelX/2 + a;
                                int currentY=y - strelY/2 + b;
                                int currentZ=z - strelZ/2 + c;

                                //process current Point
                                if(currentX<0 || currentX>=colorClasses.length ||
                                   currentY<0 || currentX>=colorClasses[0].length ||
                                   currentZ<0 || currentZ>=colorClasses[0][0].length)
                                    break;

                                if((getColor((char)a, (char)b, (char)c)==Colors.ColorClass.none || eraseOtherColors) && strel[a][b][c])
                                    setColor((char)a, (char)b, (char)c, color);
                            }


    }

    public void erode(boolean strel[][][], Colors.ColorClass color)
    {
        int strelX=strel.length;
        int strelY=strel[0].length;
        int strelZ=strel[0][0].length;

        for(char x=0; x<colorClasses.length; x++)
            for(char y=0; y<colorClasses[0].length; y++)
                for(char z=0; z<colorClasses[0][0].length; z++)
                    if(getColor(x, y, z)==color)
                    {
                        boolean innerPoint=true;
                        for(int a=0; a<strelX; a++)
                            for(int b=0; b<strelY; b++)
                                for(int c=0; c<strelZ; c++)
                                {
                                    int currentX=x - strelX/2 + a;
                                    int currentY=y - strelY/2 + b;
                                    int currentZ=z - strelZ/2 + c;

                                    //process current Point
                                    if(currentX<0 || currentX>=colorClasses.length ||
                                       currentY<0 || currentX>=colorClasses[0].length ||
                                       currentZ<0 || currentZ>=colorClasses[0][0].length)
                                        break;

                                    innerPoint=innerPoint && (getColor((char)a, (char)b, (char)c)==color || !strel[a][b][c]);
                                }

                        if(!innerPoint) setColor((char)x, (char)y, (char)z, Colors.ColorClass.none);

                    }
    }

    public static boolean[][][] getStrelSphere(int radius)
    {
        boolean[][][] strel=new boolean[2*radius+1][2*radius+1][2*radius+1];
        for(int x=0; x<=radius*2; x++)
            for(int y=0; y<=radius*2; y++)
                for(int z=0; z<=radius*2; z++)
                    strel[x][y][z]=Math.sqrt((double)((x-radius)*(x-radius)+(y-radius)*(y-radius)+(z-radius)*(z-radius)))<=radius;

        return strel;
    }

    public static boolean[][][] getStrelCube(int length)
    {
        boolean[][][] strel = new boolean[length][length][length];
        for(int x=0; x<length; x++)
            for(int y=0; y<length; y++)
                for(int z=0; z<length; z++)
                    strel[x][y][z]=true;

        return strel;
    }


    public void open(boolean strel[][][], Colors.ColorClass color)
    {
        erode(strel, color);
        dilate(strel, color, true);
    }
    
    public void close(boolean strel[][][], Colors.ColorClass color, boolean eraseOtherColors)
    {
        dilate(strel, color, eraseOtherColors);
        erode(strel, color);
    }

  public int[] getDimension()
  {
    int[] dim = new int[3];
    dim[0] = colorClasses.length;
    dim[1] = colorClasses[0].length;
    dim[2] = colorClasses[0][0].length;
    return dim;
  }

}//end class ColorTable
