/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc;

import bibliothek.gui.dock.common.CControl;
import bibliothek.gui.dock.common.event.CControlListener;
import bibliothek.gui.dock.common.event.CFocusListener;
import bibliothek.gui.dock.common.intern.AbstractCDockable;
import bibliothek.gui.dock.common.intern.CDockable;
import java.awt.KeyEventPostProcessor;
import java.awt.event.KeyEvent;
import java.util.ArrayList;

/**
 *
 * @author admin
 */
public class TabKeyController implements KeyEventPostProcessor
{
    private final TabDockableSelectionListener tabDockableSelectionListener = new TabDockableSelectionListener();
    private final TabDockableAddRemoveListener tabDockableAddRemoveListener = new TabDockableAddRemoveListener();
    
    private final ArrayList<CDockable> traversingList = new ArrayList<CDockable>();
    private final CControl control;
       
    public TabKeyController(CControl control)
    {
        this.control = control;
        this.control.addFocusListener(tabDockableSelectionListener);
        this.control.addControlListener(tabDockableAddRemoveListener);
    }
    
    private boolean removed = false;
    private int idx = 0;
    
    @Override
    public boolean postProcessKeyEvent(KeyEvent e) 
    {
        if(this.control.getCDockableCount() == 0) {
            return false;
        }
        
        if(traversingList.size() <= 1) return false;
        
        
        if(  e.getKeyCode() == KeyEvent.VK_TAB &&
            (e.getModifiersEx() & KeyEvent.CTRL_DOWN_MASK) == KeyEvent.CTRL_DOWN_MASK)
        {
            
            if(e.getID() == KeyEvent.KEY_PRESSED)
            {
                if(!removed)
                {
                    this.control.removeFocusListener(tabDockableSelectionListener);
                    removed = true;
                    idx = traversingList.size()-1;
                }
                
                // move backwards in the history
                int direction = 
                    ((e.getModifiersEx() & KeyEvent.SHIFT_DOWN_MASK) == KeyEvent.SHIFT_DOWN_MASK)?1:-1;


                if(idx != -1)
                {
                    idx += direction;

                    // this is a strange solution for modulo, since %
                    // doesn't work for negative numbers'
                    if(idx > traversingList.size()-1) {
                        idx = 0;
                    } else if(idx < 0) {
                        idx = traversingList.size()-1;
                    }

                    CDockable next = traversingList.get(idx);
                    if(next instanceof AbstractCDockable){
                        ((AbstractCDockable)next).toFront();
                    }
                }       
            }
        }
        
        
        if(e.getID() == KeyEvent.KEY_RELEASED && e.getKeyCode() == KeyEvent.VK_CONTROL) {
            if(removed)
            {
                this.control.addFocusListener(tabDockableSelectionListener);
                if(this.control.getFocusedCDockable() != null)
                {
                    traversingList.remove(this.control.getFocusedCDockable());
                    traversingList.add(this.control.getFocusedCDockable());
                }

                removed = false;
                idx = 0;
            }
        }
        
        //System.out.println(e);
        return true;
    }
    
    
    class TabDockableSelectionListener implements CFocusListener
    {
        @Override
        public void focusGained(CDockable cd) {
            if(cd != null)
            {
                traversingList.remove(cd);
                traversingList.add(cd);
            }
        }

        @Override
        public void focusLost(CDockable cd) {

        }
    }//end DockableSelectionListener
    
    
    class TabDockableAddRemoveListener implements CControlListener
    {
        @Override
        public void added(CControl cc, CDockable cd) {
            if(cd != null)
            {
                traversingList.remove(cd);
                traversingList.add(cd);
            }
        }
        
        @Override
        public void closed(CControl cc, CDockable cd) {
            traversingList.remove(cd);
        }
        
        @Override
        public void opened(CControl cc, CDockable cd) {
            if(cd != null)
            {
                traversingList.remove(cd);
                traversingList.add(cd);
            }
        }
        
        @Override
        public void removed(CControl cc, CDockable cd) {
            traversingList.remove(cd);
        }
    }//end TabDockableAddRemoveListener
}//end KeyController
