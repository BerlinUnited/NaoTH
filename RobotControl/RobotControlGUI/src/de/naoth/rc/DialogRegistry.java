/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc;

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
import java.awt.DefaultKeyboardFocusManager;
import java.awt.Frame;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import javax.swing.JFrame;
import javax.swing.JMenu;
import javax.swing.JMenuItem;
import javax.swing.KeyStroke;

/**
 *
 * @author thomas
 */
public class DialogRegistry {

    private JFrame parent = null;
    private final JMenu menu;
    private final CControl control;

    private final ArrayList<String> allDialogNames = new ArrayList<String>();

    public DialogRegistry(JFrame parent, JMenu menu) {
        this.parent = parent;
        this.menu = menu;

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
            helpButton.addActionListener(new HelpAction(DialogRegistry.this.parent, dialog.getDisplayName()));
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
            this.dialog.dispose();
            this.dialog.destroy();
            this.getContentPane().removeAll();
            this.getControl().removeSingleDockable(this.getTitleText());
        }
    }

    public void registerDialog(final Dialog dialog) {
        String dialogName = dialog.getDisplayName();

        // register a factory
        this.control.addSingleDockableFactory(dialogName, new DialogFactory(dialog));
        
        // register a menu entry 
        if (this.menu != null) {
            int insertPoint = Collections.binarySearch(allDialogNames, dialogName);
            if (insertPoint < 0) {
                JMenuItem newItem = new JMenuItem(dialogName);
                newItem.addActionListener(new ActionListener() {
                    @Override
                    public void actionPerformed(ActionEvent e) {
                        dockDialog(dialog);
                    }
                });
                menu.insert(newItem, -(insertPoint + 1));
                allDialogNames.add(-(insertPoint + 1), dialogName);
            }
        }
    }//end registerDialog

    public void dockDialog(Dialog dialog) {
        // check if view already exists
        DialogDockable dockableDialog = (DialogDockable) this.control.getSingleDockable(dialog.getDisplayName());

        // create a new one if necessary
        if (dockableDialog == null) {
            dockableDialog = new DialogDockable(dialog);
            
            // put the new dialog aside the currently focused one
            CDockable focusedDialog = this.control.getFocusedCDockable();
            if(focusedDialog == null && this.control.getCDockableCount() > 0) {
                focusedDialog = this.control.getCDockable(0);
            }
            if (focusedDialog != null) {
                dockableDialog.setLocation(focusedDialog.getBaseLocation().aside());
            }
            this.control.addDockable(dockableDialog);
        }

        dockableDialog.setVisible(true);
        this.control.getController().setFocusedDockable(dockableDialog.intern(), true);
    }//dockDialog

    public class HelpAction implements ActionListener {

        private String text = null;
        private HelpDialog dlg = null;

        @Override
        public void actionPerformed(ActionEvent ae) {
            this.dlg.showHelp();
        }

        public HelpAction(Frame parent, String title) {
            this.text = Helper.getResourceAsString("/de/naoth/rc/dialogs/" + title + ".html");
            if (this.text == null) {
                this.text = "For this dialog is no help avaliable.";
            }

            this.dlg = new HelpDialog(parent, true, text);
            this.dlg.setDefaultCloseOperation(javax.swing.WindowConstants.HIDE_ON_CLOSE);
            this.dlg.setTitle(title);
            this.dlg.setModal(false);
            //this.dlg.setAlwaysOnTop(true);
            this.dlg.setVisible(false);
        }
    }//end HelpAction
    
    public void setDefaultLayout() {
        // create a default perspective
        CControlPerspective perspectives = control.getPerspectives();
        CPerspective perspective = perspectives.createEmptyPerspective();
        CGridPerspective center = perspective.getContentArea().getCenter();
        CStackPerspective stack = new CStackPerspective();
        
        for(int i = 0; i < this.control.getCDockableCount(); i++) {
            String name = ((DialogDockable)this.control.getCDockable(i)).getTitleText();
            
            if(name.startsWith("Debug Request")) {
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
