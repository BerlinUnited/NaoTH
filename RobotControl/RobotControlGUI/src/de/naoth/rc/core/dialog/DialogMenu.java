
package de.naoth.rc.core.dialog;

import de.naoth.rc.DialogRegistry;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.ArrayList;
import java.util.Collections;
import javax.swing.Action;
import javax.swing.JMenu;
import javax.swing.JMenuItem;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class DialogMenu extends JMenu {
    
    private final DialogRegistry dialogRegistry;
    private final ArrayList<String> allDialogNames = new ArrayList<String>();

    public DialogMenu(DialogRegistry dr) {
        this.dialogRegistry = dr;
    }
    
    public JMenuItem add(final Dialog dialog) {
        String dialogName = dialog.getDisplayName();
        int insertPoint = Collections.binarySearch(allDialogNames, dialogName);
        if (insertPoint < 0) {
            JMenuItem newItem = new JMenuItem(dialogName);
            newItem.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent e) {
                    dialogRegistry.dockDialog(dialog);
                }
            });
            this.insert(newItem, -(insertPoint + 1));
            allDialogNames.add(-(insertPoint + 1), dialogName);
            return newItem;
        }
        return null;
    }
}
