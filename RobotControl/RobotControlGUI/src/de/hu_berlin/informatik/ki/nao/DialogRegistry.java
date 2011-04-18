/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.hu_berlin.informatik.ki.nao;

import bibliothek.gui.DockFrontend;
import bibliothek.gui.DockStation;
import bibliothek.gui.Dockable;
import bibliothek.gui.dock.DefaultDockable;
import bibliothek.gui.dock.frontend.MissingDockableStrategy;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import javax.swing.JMenu;
import javax.swing.JMenuItem;

/**
 *
 * @author thomas
 */
public class DialogRegistry
{

  private JMenu menu;
  private DockStation station;
  private DockFrontend frontend;
  private ArrayList<String> allDialogNames;

  public DialogRegistry(JMenu menu, DockFrontend frontend, DockStation station)
  {
    this.menu = menu;
    this.station = station;
    this.frontend = frontend;
    this.allDialogNames = new ArrayList<String>();
  }

  public void registerDialog(final Dialog dialog)
  {
    String dialogName = dialog.getDisplayName();
    
    if(menu != null)
    {
      int insertPoint = Collections.binarySearch(allDialogNames, dialogName);
      if(insertPoint < 0)
      {
        JMenuItem newItem = new JMenuItem(dialogName);
        newItem.addActionListener(new ActionListener()
        {

          @Override
          public void actionPerformed(ActionEvent e)
          {
            dockDialog(dialog);
          }
        });
        menu.insert(newItem, -(insertPoint+1));
        allDialogNames.add(-(insertPoint+1), dialogName);
      }
    }
  
  }

  private Dockable createView(String text, final Dialog dialog)
  {
    DefaultDockable result = new DefaultDockable(dialog.getPanel(), text);

    frontend.addDockable(text, result);
    frontend.setHideable(result, true);

    return result;
  }

  public void dockDialog(Dialog dialog)
  {

    String dialogName = dialog.getDisplayName();

    // check if view already exists

    Dockable existing = frontend.getDockable(dialogName);

    if(existing == null)
    {
      Dockable newDockable = createView(dialogName, dialog);
      
      station.drop(newDockable);
    }
    else
    {
      frontend.show(existing);
    }
  }
  

}
