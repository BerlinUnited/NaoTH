/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc;

import java.awt.Color;
import java.awt.Font;
import javax.swing.UIDefaults;
import javax.swing.plaf.nimbus.NimbusLookAndFeel;

/**
 *
 * @author thomas
 */
public class CustomNimbusLookAndFeel extends NimbusLookAndFeel
{
    private RobotControl parent;
    public CustomNimbusLookAndFeel(RobotControl parent)
    {
        this.parent = parent;
    }
    
    // reference
    // https://docs.oracle.com/javase%2Ftutorial%2Fuiswing%2F%2F/lookandfeel/_nimbusDefaults.html
    @Override
    public UIDefaults getDefaults()
    {
      UIDefaults defaults = super.getDefaults();
      if(parent != null)
      {
        defaults.put("defaultFont", new Font(Font.SANS_SERIF, Font.PLAIN, parent.getFontSize()));
        
        /*
        // NOTE: add custom modifications to NimbusLookAndFeel
        defaults.put("nimbusSelectionBackground", new Color(240, 250, 255));

        // don't change color of the expansion icon
        defaults.put("Tree[Enabled+Selected].collapsedIconPainter", defaults.get("Tree[Enabled].collapsedIconPainter"));
        defaults.put("Tree[Enabled+Selected].expandedIconPainter",  defaults.get("Tree[Enabled].expandedIconPainter"));

        // don't change the color of the text on selection
        defaults.put("Tree.selectionForeground", defaults.get("Tree.foreground"));
        defaults.put("Tree:TreeCell[Focused+Selected].textForeground", defaults.get("Tree.foreground"));
        defaults.put("Tree:TreeCell[Enabled+Selected].textForeground", defaults.get("Tree.foreground"));
        */
      }
      return defaults;
    }
}
