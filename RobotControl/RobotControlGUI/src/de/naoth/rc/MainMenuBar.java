package de.naoth.rc;

import de.naoth.rc.components.S20BinaryLookup;
import java.awt.Color;
import java.awt.DefaultKeyboardFocusManager;
import java.awt.Dimension;
import java.awt.event.FocusEvent;
import java.awt.event.FocusListener;
import java.awt.event.InputEvent;
import java.awt.event.KeyEvent;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashMap;
import java.util.Vector;
import javax.swing.DefaultComboBoxModel;
import javax.swing.JComboBox;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;

/**
 *
 * @author Heinrich
 */
public class MainMenuBar extends JMenuBar {
    
    private final HashMap<String, JMenu> menus = new HashMap<String, JMenu>();
    private final HashMap<String, JMenuItem> menuItems = new HashMap<String, JMenuItem>();
    
    private final S20BinaryLookup dialogSearcher;
    private JComboBox<DialogItem> dialogSearch = new JComboBox<>();
    
    class DialogItem {
        public JMenuItem menuItem;
        
        DialogItem(JMenuItem menuItem) {
            this.menuItem = menuItem;
        }
        
        @Override
        public String toString() {
            return menuItem.getText();
        }
        
    }
    
    class SortedComboBoxModel extends DefaultComboBoxModel 
    {
        public SortedComboBoxModel() {
          super();
        }
        public SortedComboBoxModel(Object[] items) {
          Arrays.sort(items);
          int size = items.length;
          for (int i = 0; i < size; i++) {
            super.addElement(items[i]);
          }
          setSelectedItem(items[0]);
        }

        public SortedComboBoxModel(Vector items) {
          Collections.sort(items);
          int size = items.size();
          for (int i = 0; i < size; i++) {
            super.addElement(items.elementAt(i));
          }
          setSelectedItem(items.elementAt(0));
        }

        @Override
        public void addElement(Object element) {
          insertElementAt(element, 0);
        }

        @Override
        public void insertElementAt(Object element, int index) {
          int size = getSize();
          for (index = 0; index < size; index++) {
            Comparable c = (Comparable) getElementAt(index).toString();
            if (c.compareTo(element.toString()) > 0) {
              break;
            }
          }
          super.insertElementAt(element, index);
        }
      }
    
    public MainMenuBar() {
        this.dialogSearch.setModel(new SortedComboBoxModel());
        this.dialogSearch.setEditable(true);
        
        this.dialogSearch.addFocusListener(new FocusListener() {
            @Override
            public void focusGained(FocusEvent e) {
                dialogSearch.setBackground(Color.GREEN);
            }

            @Override
            public void focusLost(FocusEvent e) {
                dialogSearch.setBackground(Color.GRAY);
            }
        });
        
        this.dialogSearch.setMaximumSize(new Dimension(100, 100));
        
        this.setFocusable(true);
        this.dialogSearcher = new S20BinaryLookup(this.dialogSearch);

        this.dialogSearch.addActionListener((e) -> {
            // NOTE: the command string "comboBoxEdited" can be found in JComboBox.actionPerformed
            // this ensures that we only react to the final selection.
            if(e.getActionCommand().equals("comboBoxEdited")) {
                ((DialogItem)dialogSearch.getSelectedItem()).menuItem.doClick();
            }
        });
        
        //this.add(dialogSearch);
        
        DefaultKeyboardFocusManager.getCurrentKeyboardFocusManager()
            .addKeyEventPostProcessor((e) -> {
                if(e.getID() == KeyEvent.KEY_PRESSED && 
                   e.getKeyCode() == KeyEvent.VK_F && 
                    e.getModifiers() == InputEvent.ALT_MASK) 
                {
                    dialogSearch.requestFocus();
                    return true;
                }
                
                return false;
            });
    }
    
    public JMenuItem addDialog(String name, String category, char mnemonic) {
        JMenu menu = this.menus.get(category);
        if(menu == null) {
            menu = new JMenu(category, false);
            if(Character.isLetter(mnemonic)) {
                menu.setMnemonic(mnemonic);
            }
            this.menus.put(category, menu);
            
            this.remove(dialogSearch);
            
            this.add(menu,1);
            
            this.add(dialogSearch, this.getComponentCount()-2);
        }
        
        // register a menu entry
        JMenuItem menuItem = menuItems.get(name);
        if (menuItem == null) {
            menuItem = new JMenuItem(name);
            insertMenuItem(menuItem, menu);
            menuItems.put(name, menuItem); // remember, so the dialog is not added twice
            
            this.dialogSearch.addItem(new DialogItem(menuItem));
            this.dialogSearch.revalidate();
            this.dialogSearcher.revalidateModel();
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
