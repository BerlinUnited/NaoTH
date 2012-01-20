/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc;

import bibliothek.gui.DockFrontend;
import bibliothek.gui.Dockable;
import bibliothek.gui.dock.DefaultDockable;
import bibliothek.gui.dock.event.DockFrontendAdapter;
import bibliothek.gui.dock.event.DockableSelectionEvent;
import bibliothek.gui.dock.event.DockableSelectionListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.util.ArrayList;
import java.util.List;

/**
 *
 * @author admin
 */
public class TabKeyController 
{
    private TabDockableSelectionListener tabDockableSelectionListener = new TabDockableSelectionListener();
    private TabDockableAddRemoveListener tabDockableAddRemoveListener = new TabDockableAddRemoveListener();
    private KeyAdapter keyAdapter = new KeyAdapter();
    private DockFrontend frontend;
    
    private ArrayList<Dockable> traversingList = new ArrayList<Dockable>();

            
    public void install(DockFrontend frontend)
    {
        this.frontend = frontend;
        this.frontend.addFrontendListener(tabDockableAddRemoveListener);
        this.frontend.getController().addDockableSelectionListener(tabDockableSelectionListener);
        this.frontend.getController().getKeyboardController().addGlobalListener(keyAdapter);
    }
    
    public void uninstall()
    {
        if(this.frontend != null)
        {
            this.frontend.removeFrontendListener(tabDockableAddRemoveListener);
            this.frontend.getController().removeDockableSelectionListener(tabDockableSelectionListener);
            this.frontend.getController().getKeyboardController().removeGlobalListener(keyAdapter);
        }
    }
    
    
    class KeyAdapter implements KeyListener
    {
                
        private boolean removed = false;
        private int idx = 0;
        
        @Override
        public void keyTyped(KeyEvent e) {

        }

        @Override
        public void keyPressed(KeyEvent e) {
            if(traversingList.size() <= 1) return;


            if((e.getModifiersEx() & KeyEvent.CTRL_DOWN_MASK) == KeyEvent.CTRL_DOWN_MASK
                    &&
               (e.getModifiersEx() & KeyEvent.SHIFT_DOWN_MASK) == KeyEvent.SHIFT_DOWN_MASK
                    &&
                e.getKeyCode() >= KeyEvent.VK_A && e.getKeyCode() <= KeyEvent.VK_Z)
            {
                char c = (char)e.getKeyCode();
                c = Character.toLowerCase(c);
                List<Dockable> dockables =  frontend.listDockables();

                for(Dockable d : dockables)
                {
                  if(!frontend.isHidden(d))
                  {
                     if(d.getTitleText() != null && d.getTitleText().length()>0)
                     {
                         if(c == Character.toLowerCase(d.getTitleText().charAt(0)))
                         {
                             frontend.getController().setFocusedDockable(d, true);
                             return;
                         }
                     }
                  }
                }
            }//end if


            if(e.getKeyCode() == KeyEvent.VK_TAB &&
               (e.getModifiersEx() & KeyEvent.CTRL_DOWN_MASK) == KeyEvent.CTRL_DOWN_MASK)
            {
                if(!removed)
                {
                    //DialogRegistry.this.frontend.removeFrontendListener(tabIterator);
                    frontend.getController().removeDockableSelectionListener(tabDockableSelectionListener);

                    removed = true;
                    idx = traversingList.size()-1;
                }

                int direction = 
                        ((e.getModifiersEx() & KeyEvent.SHIFT_DOWN_MASK) == KeyEvent.SHIFT_DOWN_MASK)?1:-1;

                //Dockable current = DialogRegistry.this.station.getFrontDockable();
                //int idx = traversingList.indexOf(current);

                if(idx != -1)
                {
                    idx += direction;

                    // this is a strange solution for modulo, since %
                    // doesn't work for negative numbers'
                    if(idx > traversingList.size()-1)
                        idx = 0;
                    else if(idx < 0)
                        idx = traversingList.size()-1;


                    Dockable next = traversingList.get(idx);

                    frontend.getController().setFocusedDockable(next, true);
                }
            }
        }

        @Override
        public void keyReleased(KeyEvent e) {
            if(e.getKeyCode() == KeyEvent.VK_CONTROL)
            {
                if(removed)
                {
                    //DialogRegistry.this.frontend.addFrontendListener(tabIterator);
                    frontend.getController().addDockableSelectionListener(tabDockableSelectionListener);
                    if(frontend.getController().getFocusedDockable() != null)
                    {
                        traversingList.remove(frontend.getController().getFocusedDockable());
                        traversingList.add(frontend.getController().getFocusedDockable());
                    }

                    removed = false;
                    idx = 0;
                }
            }
        }
    }//end KeyController
    
    
    
    class TabDockableSelectionListener implements DockableSelectionListener
    {
        @Override
        public void dockableSelected(DockableSelectionEvent event) {
            if(event.getNewSelected() != null)
            {
                traversingList.remove(event.getNewSelected());
                traversingList.add(event.getNewSelected());
            }
        }
    }//end DockableSelectionListener
    
    
    class TabDockableAddRemoveListener extends DockFrontendAdapter
    {
        @Override
        public void added(DockFrontend frontend, Dockable dockable) {
            if(dockable instanceof DefaultDockable && dockable != null)
            {
                traversingList.remove(dockable);
                traversingList.add(dockable);
            }
        }
        
        @Override
        public void hidden(DockFrontend fronend, Dockable dockable) {
            traversingList.remove(dockable);
        }
        
        @Override
        public void shown(DockFrontend frontend, Dockable dockable) {
            if(dockable instanceof DefaultDockable && dockable != null)
            {
                traversingList.remove(dockable);
                traversingList.add(dockable);
            }
        }
        
        @Override
        public void removed(DockFrontend frontend, Dockable dockable) {
            traversingList.remove(dockable);
        }
    }//end TabDockableAddRemoveListener
}//end KeyController
