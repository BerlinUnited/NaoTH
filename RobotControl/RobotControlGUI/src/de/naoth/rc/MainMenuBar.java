package de.naoth.rc;

import java.util.HashMap;
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
    
    
    public JMenuItem addDialog(String name, String category, char mnemonic) {
        JMenu menu = this.menus.get(category);
        if(menu == null) {
            menu = new JMenu(category, false);
            if(Character.isLetter(mnemonic)) {
                menu.setMnemonic(mnemonic);
            }
            this.menus.put(category, menu);
            this.add(menu,1);
        }
        
        // register a menu entry
        JMenuItem menuItem = menuItems.get(name);
        if (menuItem == null) {
            menuItem = new JMenuItem(name);
            insertMenuItem(menuItem, menu);
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
