package de.naoth.rc;

import java.util.HashMap;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;

/**
 *
 * @author Heinrich
 */
public class MainMenuBar extends JMenuBar 
{
    private final HashMap<String, JMenu> menus = new HashMap<String, JMenu>();
    private final HashMap<String, JMenuItem> menuItems = new HashMap<String, JMenuItem>();
    
    // NOTE: this was an experiment and was reimplemented in @see DialogFastAccessPanel.java
//    class DialogSearch extends JComboBox <DialogSearch.DialogItem>
//    {
//        private final S20BinaryLookup dialogSearcher;
//        
//        public DialogSearch() 
//        {
//            // setup the dialog search
//            super.setEditable(true);
//            super.setFocusable(true);
//            super.setMaximumSize(new Dimension(100, 100));
//            //this.setModel(new SortedComboBoxModel());
//            this.dialogSearcher = new S20BinaryLookup(this);
//
//            this.addActionListener((e) -> {
//                // NOTE: the command string "comboBoxEdited" can be found in JComboBox.actionPerformed
//                // this ensures that we only react to the final selection.
//                if(e.getActionCommand().equals("comboBoxEdited")) {
//                    
//                    if(dialogSearch.getSelectedItem() instanceof DialogItem) {
//                        ((DialogItem)dialogSearch.getSelectedItem()).menuItem.doClick();
//                    } else {
//                        Logger.getLogger(MainMenuBar.class.getName()).log(Level.INFO, "Diealog doesn't exist: {0}", dialogSearch.getSelectedItem());
//                    }
//                }
//            });
//        }
//
//        // this class provides a mapping from the dialogSearch combo box to the
//        // particular manu entries
//        private class DialogItem 
//        {
//            public JMenuItem menuItem;
//
//            DialogItem(JMenuItem menuItem) {
//                this.menuItem = menuItem;
//            }
//
//            @Override
//            public String toString() {
//                return menuItem.getText();
//            }
//        }
//        
//        /*
//        // In order for the S20BinaryLookup to work the entries in the 
//        // JComboBox must be sorted.
//        class SortedComboBoxModel extends DefaultComboBoxModel 
//        {
//            @Override
//            public void addElement(Object element) {
//                insertElementAt(element, 0);
//            }
//
//            @Override
//            public void insertElementAt(Object element, int index) {
//                int size = getSize();
//                for (index = 0; index < size; index++) {
//                    Comparable c = (Comparable) getElementAt(index).toString();
//                    if (c.compareTo(element.toString()) > 0) {
//                        break;
//                    }
//                }
//                super.insertElementAt(element, index);
//            }
//        }
//        */
//    
//        public void add(JMenuItem menuItem) 
//        {
//            DialogItem dialogItem = new DialogItem(menuItem);
//            
//            int i = 0;
//            for (; i < this.getItemCount(); i++) {
//                String name = this.getItemAt(i).toString();
//                if (name.compareTo(dialogItem.toString()) > 0) {
//                    break;
//                }
//            }
//            this.insertItemAt(dialogItem, i);
//            this.revalidate();
//            this.dialogSearcher.revalidateModel();
//        }
//    }
//    
//    DialogSearch dialogSearch = new DialogSearch();
    

    public MainMenuBar() 
    {   
        // attach a global key listener
        // go to dialof search with ALT+F
        /*
        DefaultKeyboardFocusManager.getCurrentKeyboardFocusManager().addKeyEventPostProcessor(
            (e) -> {
                if(e.getID()        == KeyEvent.KEY_PRESSED && 
                   e.getKeyCode()   == KeyEvent.VK_F && 
                   e.getModifiers() == InputEvent.ALT_MASK) 
                {
                    dialogSearch.requestFocus();
                    return true;
                }
                
                return false;
            });
*/
    }
    
    public JMenuItem addDialog(String name, String category, char mnemonic) {
        JMenu menu = this.menus.get(category);
        if(menu == null) 
        {
            menu = new JMenu(category, false);
            if(Character.isLetter(mnemonic)) {
                menu.setMnemonic(mnemonic);
            }
            this.menus.put(category, menu);
            
            // HACK: make sure th dialogs are in order
            // we assume there is a main manu at index 0 and help at the last index
            //this.remove(dialogSearch);
            
            this.add(menu,1);
            
            //this.add(dialogSearch, this.getComponentCount()-2);
        }
        
        // register a menu entry
        JMenuItem menuItem = menuItems.get(name);
        if (menuItem == null) {
            menuItem = new JMenuItem(name);
            insertMenuItem(menuItem, menu);
            menuItems.put(name, menuItem); // remember, so the dialog is not added twice
            
            //this.dialogSearch.add(menuItem);
        }
        
        return menuItem;
    }
    
    private void insertMenuItem(JMenuItem item, JMenu menu) {
        for(int i = 0; i < menu.getItemCount(); ++i) {
            JMenuItem i_item = menu.getItem(i);
            if(item.getText().compareTo(i_item.getText()) <= 0) {
                menu.insert(item, i);
                return;
            }
        }
        menu.add(item);
    }
}
