/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.hu_berlin.informatik.ki.nao;

import java.awt.Color;
import java.util.LinkedList;
import javax.swing.JMenu;
import javax.swing.JMenuItem;
import javax.swing.JPanel;
import javax.swing.border.LineBorder;
import org.flexdock.docking.Dockable;
import org.flexdock.docking.DockingPort;
import org.flexdock.view.View;
import org.flexdock.view.Viewport;

/**
 *
 * @author thomas
 */
public class DialogRegistry
{

  private JMenu menu;
  private DockingPort dock;
  private LinkedList<View> viewChronologicalOrder;

  public DialogRegistry(JMenu menu, DockingPort dock)
  {
    this.viewChronologicalOrder = new LinkedList<View>();
    this.menu = menu;
    this.dock = dock;
  }

  public void registerDialog(Dialog dialog)
  {
    String dialogName = dialog.getClass().getSimpleName();
    View newView = createView(dialogName, dialogName, dialog.getPanel());
    if(menu != null)
    {
      JMenuItem newItem = new JMenuItem(dialogName);
      menu.add(newItem);
    }
    
//    if(viewChronologicalOrder.isEmpty())
//    {
//      dock.dock((Dockable) newView, Viewport.EAST_REGION);
//    }
//    else
//    {
//      viewChronologicalOrder.getLast().dock(newView);
//    }
  }

  private View createView(String id, String text, JPanel panel)
  {
    final View result = new View(id, text);
    panel.setBorder(new LineBorder(Color.GRAY, 1));
    result.setContentPane(panel);

    return result;
  }

}
