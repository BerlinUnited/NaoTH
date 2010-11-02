/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.hu_berlin.informatik.ki.nao;

import java.awt.Color;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.LinkedList;
import java.util.Set;
import javax.swing.Action;
import javax.swing.JMenu;
import javax.swing.JMenuItem;
import javax.swing.JPanel;
import javax.swing.border.LineBorder;
import org.flexdock.docking.Dockable;
import org.flexdock.docking.DockingConstants;
import org.flexdock.docking.DockingPort;
import org.flexdock.docking.event.DockingEvent;
import org.flexdock.docking.event.DockingListener;
import org.flexdock.view.View;
import org.flexdock.view.Viewport;
import org.flexdock.view.actions.DefaultCloseAction;

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

  public void registerDialog(final Dialog dialog)
  {
    String dialogName = dialog.getClass().getSimpleName();
    
    if(menu != null)
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
      menu.add(newItem);
    }
  
  }

  private View createView(String id, String text, final Dialog dialog)
  {
    final View result = new View(id, text);
    dialog.getPanel().setBorder(new LineBorder(Color.GRAY, 1));
    result.setContentPane(dialog.getPanel());

    DefaultCloseAction closeAction = new DefaultCloseAction()
    {

      @Override
      public void actionPerformed(ActionEvent e)
      {
        super.actionPerformed(e);
        viewChronologicalOrder.remove(result);

      }
    };
    closeAction.putValue(Action.NAME, DockingConstants.CLOSE_ACTION);

    result.addAction(closeAction);
    result.addAction(DockingConstants.PIN_ACTION);

    return result;
  }

  public void dockDialog(Dialog dialog)
  {

    String dialogName = dialog.getClass().getSimpleName();

    // check if view already exists
    Set<Dockable> dockables = dock.getDockables();
    boolean isAlreadyDocked = false;
    for(Dockable d : dockables)
    {
      if(dialogName.equals(d.getPersistentId()))
      {
        isAlreadyDocked = true;
        d.getDockingProperties().setActive(true);
        break;
      }
    }

    if(!isAlreadyDocked)
    {
      View newView = createView(dialogName, dialogName, dialog);

      if(viewChronologicalOrder.isEmpty())
      {
        dock.dock((Dockable) newView, Viewport.EAST_REGION);
      }
      else
      {
        viewChronologicalOrder.getLast().dock(newView);
      }
      viewChronologicalOrder.addLast(newView);
    }
  }
  

}
