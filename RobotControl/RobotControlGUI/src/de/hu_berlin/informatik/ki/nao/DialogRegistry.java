/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.hu_berlin.informatik.ki.nao;

import java.util.HashMap;
import java.util.Map;
import net.xeoh.plugins.base.Plugin;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.events.PluginLoaded;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 *
 * @author thomas
 */
@PluginImplementation
public class DialogRegistry implements Plugin
{

  @InjectPlugin
  public RobotControlGUI main;

  private Map<String, Dialog> dialogs;

  public DialogRegistry()
  {
    dialogs = new HashMap<String, Dialog>();
  }

  @PluginLoaded
  public void registerDialog(Dialog dialog)
  {
    if(dialog != null)
    {
      dialogs.put(dialog.getClass().getSimpleName(), dialog);
    }
  }

  public Map<String, Dialog> getDialogs()
  {
    return dialogs;
  }

}
