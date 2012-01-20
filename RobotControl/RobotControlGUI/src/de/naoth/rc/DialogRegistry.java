/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.naoth.rc;

import bibliothek.gui.DockFrontend;
import bibliothek.gui.DockStation;
import bibliothek.gui.Dockable;
import bibliothek.gui.dock.DefaultDockable;
import bibliothek.gui.dock.action.DefaultDockActionSource;
import bibliothek.gui.dock.action.LocationHint;
import bibliothek.gui.dock.action.actions.SimpleButtonAction;
import bibliothek.gui.dock.event.DockFrontendAdapter;
import bibliothek.gui.dock.event.DockFrontendListener;
import bibliothek.gui.dock.event.DockableSelectionEvent;
import bibliothek.gui.dock.event.DockableSelectionListener;
import java.awt.Frame;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import javax.swing.JMenu;
import javax.swing.JMenuItem;
import javax.swing.KeyStroke;

/**
 *
 * @author thomas
 */
public class DialogRegistry
{

  private Frame parent = null;
  private JMenu menu;
  private DockStation station;
  private DockFrontend frontend;
  private ArrayList<String> allDialogNames;
  
  
  private TabKeyController tabKeyController = new TabKeyController();
  
  public DialogRegistry(Frame parent, JMenu menu, DockFrontend frontend, DockStation station)
  {
    this.parent = parent;
    this.menu = menu;
    this.station = station;
    this.frontend = frontend;
    this.allDialogNames = new ArrayList<String>();
    
    this.tabKeyController.install(this.frontend);
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
            dockDialog(dialog, true);
          }
        });
        menu.insert(newItem, -(insertPoint+1));
        allDialogNames.add(-(insertPoint+1), dialogName);
      }
    }
  }//end registerDialog

  private Dockable createView(String text, final Dialog dialog)
  {
    //
    DefaultDockable result = new DefaultDockable(dialog.getPanel(), text);

    frontend.addDockable(text, result);
    frontend.setHideable(result, true);
    frontend.addFrontendListener(new DisposeListener(result, dialog));

    DefaultDockActionSource actions = new DefaultDockActionSource( new LocationHint( LocationHint.DOCKABLE, LocationHint.LEFT ));
    actions.add(new HelpAction(this.parent, text));
    result.setActionOffers(actions);

    
    return result;
  }//end createView

  
  public void dockDialog(Dialog dialog, boolean smart_place)
  {

    String dialogName = dialog.getDisplayName();

    // check if view already exists

    Dockable existing = frontend.getDockable(dialogName);

    if(existing == null)
    {
      Dockable newDockable = createView(dialogName, dialog);
      
      
      if(!smart_place || station.getDockableCount() == 0)
          station.drop(newDockable);
      else if (station.getFrontDockable() != null)
        station.drop(newDockable, station.getDockableProperty(station.getFrontDockable(), newDockable));
      else
        station.drop(newDockable, station.getDockableProperty(station.getDockable(0), newDockable));
    
    
    }
    else
    {
      frontend.show(existing);
      frontend.getController().setFocusedDockable(existing, true);
    }
  }//dock Dialog

  public class HelpAction extends SimpleButtonAction
  {
    private String text = null;
    private HelpDialog dlg = null;
    
    public HelpAction(Frame parent, String title)
    {
      //setText("Run...");
      setIcon(new javax.swing.ImageIcon(getClass().getResource("/toolbarButtonGraphics/general/Help16.gif")));
      setTooltip("show ");
      //this.setAccelerator(KeyStroke.getKeyStroke("pressed F1"));
      this.setAccelerator(KeyStroke.getKeyStroke("control pressed H"));
      
      
      this.text = Helper.getResourceAsString("/de/naoth/rc/dialogs/"+title+".html");
      if(this.text == null)
        this.text = "For this dialog is no help avaliable.";

      this.dlg = new HelpDialog(parent, true, text);
      this.dlg.setDefaultCloseOperation(javax.swing.WindowConstants.HIDE_ON_CLOSE);
      this.dlg.setTitle(title);
      this.dlg.setModal(false);
      //this.dlg.setAlwaysOnTop(true);
      this.dlg.setVisible(false);
    }

    @Override
    public void action(Dockable dockable)
    {
      this.dlg.showHelp();
      //System.out.println("kabum");
    }
  }//end HelpAction
  
  
  
  class DisposeListener extends DockFrontendAdapter
  {
    private Dialog dialog;
    private Dockable observed;
    
    public DisposeListener(Dockable observed, Dialog dialog)
    {
        this.observed = observed;
        this.dialog = dialog;
    }
      
    @Override
    public void shown( DockFrontend frontend, Dockable dockable ){
        if( dockable == observed ){
            
        }
    }

    @Override
    public void hidden( DockFrontend fronend, Dockable dockable ){
        if( dockable == observed ){
            dialog.dispose();
            dialog.destroy();
            frontend.remove(observed);
            frontend.removeFrontendListener(this);
        }
    }
  }//end class DisposeListener


}//end class DialogRegistry
