/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc;

import de.naoth.rc.core.dialog.Dialog;
import bibliothek.gui.dock.common.CControl;
import bibliothek.gui.dock.common.DefaultSingleCDockable;
import bibliothek.gui.dock.common.SingleCDockableFactory;
import bibliothek.gui.dock.common.action.CButton;
import bibliothek.gui.dock.common.event.CDockableAdapter;
import bibliothek.gui.dock.common.intern.CDockable;
import bibliothek.gui.dock.common.perspective.CControlPerspective;
import bibliothek.gui.dock.common.perspective.CGridPerspective;
import bibliothek.gui.dock.common.perspective.CPerspective;
import bibliothek.gui.dock.common.perspective.CStackPerspective;
import bibliothek.gui.dock.common.perspective.SingleCDockablePerspective;
import bibliothek.gui.dock.common.theme.ThemeMap;
import de.naoth.rc.core.dialog.RCDialog;
import java.awt.DefaultKeyboardFocusManager;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.io.IOException;
import javax.swing.JFrame;
import javax.swing.JMenuItem;
import javax.swing.KeyStroke;

/**
 *
 * @author thomas
 */
public class DialogRegistry {

    private JFrame parent = null;
    private final CControl control;
    private final MainMenuBar menuBar;
    private final DialogFastAccessPanel accessPanel;
    private final HelpDialog help;

    public DialogRegistry(JFrame parent, MainMenuBar menuBar, DialogFastAccessPanel accessPanel, HelpDialog help) {
        this.parent = parent;
        this.menuBar = menuBar;
        this.accessPanel = accessPanel;
        this.help = help;

        this.control = new CControl(this.parent);
        control.setTheme(ThemeMap.KEY_ECLIPSE_THEME);
        this.parent.add(control.getContentArea());
        
        // install a global tab listener
        DefaultKeyboardFocusManager.getCurrentKeyboardFocusManager().
            addKeyEventPostProcessor(new TabKeyController(this.control));
    }
    
    private class DialogFactory implements SingleCDockableFactory {

        private final Dialog dialog;

        public DialogFactory(Dialog dialog) {
            this.dialog = dialog;
        }

        @Override
        public DialogDockable createBackup(String id) {
            return new DialogDockable(dialog);
        }
    }

    private class DialogDockable extends DefaultSingleCDockable {
 
        private final Dialog dialog;
        public DialogDockable(Dialog dialog) {
            super(dialog.getDisplayName());
            this.dialog = dialog;
            setTitleText(dialog.getDisplayName());
            add(dialog.getPanel());
            
            this.setCloseable(true);
            this.setMinimizable(false);
            this.setExternalizable(false);

            // TODO: clean this up
            CButton helpButton = new CButton(null, new javax.swing.ImageIcon(getClass().getResource("/toolbarButtonGraphics/general/Help16.gif")));
            helpButton.setAccelerator(KeyStroke.getKeyStroke("control pressed H"));
            helpButton.setTooltip("show help for " + dialog.getDisplayName());
            helpButton.addActionListener((e) -> { help.showHelp(dialog.getDisplayName()); });
            this.addAction(helpButton);
            
            this.addCDockableStateListener(new CDockableAdapter() {
                @Override
                public void visibilityChanged(CDockable cd) {
                    if(!isVisible()) {
                        dispose();
                    }
                }
            });
        }
        
        public void dispose()
        {
            this.dialog.destroy();
            this.getContentPane().removeAll();
            this.getControl().removeSingleDockable(this.getTitleText());
        }
    }

    public void registerDialog(final Dialog dialog) {
        
        String name = dialog.getDisplayName();
        String category = dialog.getCategory();
        char mnemonic = 0;
        
        // register a factory
        this.control.addSingleDockableFactory(name, new DialogFactory(dialog));
        
        
        // process anotation RCDialog
        if (dialog.getClass().isAnnotationPresent(RCDialog.class)) {
            RCDialog annotation = (RCDialog)dialog.getClass().getAnnotation(RCDialog.class);
            
            if(!annotation.name().isEmpty()) {
                name = annotation.name();
            }
            
            category = annotation.category().name();
            mnemonic = annotation.category().mnemonic();
        } 
        
        // create menu item (should never return null)
        
        ActionListener action = (ActionEvent e) -> {
            dockDialog(dialog);
        };
        
        JMenuItem item = menuBar.addDialog(name, category, mnemonic);
        item.addActionListener(action);
        
        this.accessPanel.registerDialog(name, category, action);
    }//end registerDialog

    
    public void dockDialog(Dialog dialog) {
        // check if view already exists
        DialogDockable dockableDialog = (DialogDockable) this.control.getSingleDockable(dialog.getDisplayName());

        // create a new one if necessary
        if (dockableDialog == null) {
            dockableDialog = new DialogDockable(dialog);
            
            // add the dialog to the controller
            // NOTE: this needs to be done before we can modify its location
            this.control.addDockable(dockableDialog);
            
            // put the new dialog aside the currently focused one
            CDockable focusedDialog = this.control.getFocusedCDockable();
            
            // no dialg is focussed => chose the first if possible
            if(focusedDialog == null && this.control.getCDockableCount() > 0) {
                focusedDialog = this.control.getCDockable(0);
            }
            
            if (focusedDialog != null) {
                dockableDialog.setLocationsAside(focusedDialog);
            }
        }

        dockableDialog.setVisible(true);
        this.control.getController().setFocusedDockable(dockableDialog.intern(), true);
    }//dockDialog

    
    protected void disposeOnClose() {
        this.control.destroy();
    }

    public void setDefaultLayout() {
        // create a default perspective
        CControlPerspective perspectives = control.getPerspectives();
        CPerspective perspective = perspectives.createEmptyPerspective();
        CGridPerspective center = perspective.getContentArea().getCenter();
        CStackPerspective stack = new CStackPerspective();
        
        for(int i = 0; i < this.control.getCDockableCount(); i++) {
            String name = ((DialogDockable)this.control.getCDockable(i)).getTitleText();
            
            if(name.startsWith("Debug Request") || name.startsWith("DebugRequest")) {
                center.gridAdd( 2, 0, 1, 1, new SingleCDockablePerspective( name ) );
            } else {
                stack.add(new SingleCDockablePerspective( name ));
            }
        }
        
        stack.setSelection( stack.getDockable( 0 ) );
        center.grid().addDockable(0, 0, 2, 1, stack );
        
        perspectives.setPerspective( "default", perspective );
        perspective.storeLocations();
        control.load( "default" );
    }
    
    public void loadFromFile(File in) throws IOException {
        this.control.readXML(in);
    }
    
    public void saveToFile(File out) throws IOException {
        this.control.writeXML(out);
    }
}//end class DialogRegistry
