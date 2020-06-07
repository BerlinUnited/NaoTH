/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.naoth.rc.drawings;

import java.awt.Color;

/**
 *
 * @author Heinrich Mellmann
 */
  public class Colors
  {
    public final static Color none = new Color(153,115,154);
    public final static Color skyblue = new Color(128,128,255);
    public final static Color blue = new Color(0,0,128);
    public final static Color pink = new Color(255,0,255);
    public final static Color yellowOrange = new Color(255,218,74);

    public enum ColorClass 
    {
        none, /*<! all other objects */
        orange, /*<! ball */
        yellow, /*<! yellow goal and flag */
        skyblue, /*<! skyblue goal and flag */
        white, /*<! boundaries and lines, player skin */
        red, /*<! red player */
        blue, /*<! blue player */
        green, /*<! playground and green flag */
        black, /*<! player joints, ball-challenge */
        pink, /*<! pink flag */
        gray, /*<! player */
        yellowOrange, /*<! ball or yellow goal */
        numOfColors
    };
    
    public static Color GetColor(ColorClass colorClass) throws Exception
    {
      switch(colorClass)
      {
        case none: return Colors.none;
        case orange: return Color.orange;
        case yellow: return Color.yellow;
        case skyblue: return Colors.skyblue;
        case white: return Color.white;
        case red: return Color.red;
        case blue: return Colors.blue;
        case green: return Color.green;
        case black: return Color.black;
        case pink: return Colors.pink;
        case gray: return Color.gray;
        case yellowOrange: return Colors.yellowOrange;
        default: 
          throw new Exception("Unknown ColorClass");
      }//end switch
    }//end GetColor
    
    
    public static Color parseColor(String hexString)
    {
        if(hexString.length() == 8)
        {
            int red = Integer.parseInt(hexString.substring(0, 2), 16);
            int green = Integer.parseInt(hexString.substring(2, 4), 16);
            int blueColor = Integer.parseInt(hexString.substring(4, 6), 16);
            int alpha = Integer.parseInt(hexString.substring(6), 16);
            return new Color(red, green, blueColor, alpha);
        }
        else
        {
            int colorNumber = Integer.decode("0x"+hexString);
            return new Color(colorNumber);
        }
    }//end parseColor
  }// end class Colors
