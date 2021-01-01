/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc;

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
    
    @Override
    public UIDefaults getDefaults()
    {
      UIDefaults defaults = super.getDefaults();
      if(parent != null)
      {
        defaults.put("defaultFont", new Font(Font.SANS_SERIF, Font.PLAIN, parent.getFontSize()));
      }
      return defaults;
    }
}
