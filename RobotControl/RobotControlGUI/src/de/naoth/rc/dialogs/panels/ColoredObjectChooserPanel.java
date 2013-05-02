/*
 * ColoredObjectChooserPanel.java
 *
 * 
 */

package de.naoth.rc.dialogs.panels;

import de.naoth.rc.Helper;
import de.naoth.rc.dialogs.Tools.Colors;
import java.awt.Color;
import java.awt.Component;
import java.awt.Graphics;
import java.util.Enumeration;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.AbstractButton;
import javax.swing.Icon;
import javax.swing.JToggleButton;

/**
 *
 * @author CNR
 */
public class ColoredObjectChooserPanel extends javax.swing.JPanel {

    /** Creates new form ColorChooserPanel */
    public ColoredObjectChooserPanel() {
      initComponents();
      
      try{
        for(Colors.ColorClass c: Colors.ColorClass.values())
        {
          if
          (
            c == Colors.ColorClass.green || 
            c == Colors.ColorClass.white || 
            c == Colors.ColorClass.none || 
            c == Colors.ColorClass.orange || 
            c == Colors.ColorClass.yellow ||
            c == Colors.ColorClass.skyblue || 
            c == Colors.ColorClass.red || 
            c == Colors.ColorClass.blue
          )
          {
            ColorButton button = null;
            switch(c)
            {
              case green:
//                button = new ColorButton(c, "Field", 46);
                break;
                
              case white:
                button = new ColorButton(c, "Lines", 49);
                break;
                
              case orange:
                button = new ColorButton(c, "Ball", 41);
                break;
                
              case yellow:
                button = new ColorButton(c, "Left Goal Post", 95);
                break;
                
              case skyblue:
                button = new ColorButton(c, "Right Goal Post", 104);
                break;
                
              case red:
                button = new ColorButton(c, "Red Team Player", 110);
                break;
                
              case blue:
                button = new ColorButton(c, "Blue Team Player", 113);
                break;
                
              default:
                button = new ColorButton(Colors.ColorClass.none, "Off", 41);
                button.setSelected(true);
                break;                
           