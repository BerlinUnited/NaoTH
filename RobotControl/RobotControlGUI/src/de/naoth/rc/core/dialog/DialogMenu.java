
package de.naoth.rc.core.dialog;

import de.naoth.rc.DialogRegistry;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Map;
import java.util.TreeMap;
import java.util.stream.Collectors;
import javax.swing.JMenu;
import javax.swing.JMenuItem;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class DialogMenu extends JMenu {
    
    private DialogRegistry dialogRegistry;
    private final TreeMap<String, JMenuItem> entries = new TreeMap<>();

    public void setDialogRegistry(DialogRegistry dr) {
        this.dialogRegistry = dr;
    }
    
    public JMenuItem add(final Dialog dialog) {
        String dialogName = dialog.getDisplayName();
        if(this.dialogRegistry != null && !entries.containsKey(dialogName)) {
            JMenuItem newItem = new JMenuItem(dialogName);
            newItem.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent e) {
                    dialogRegistry.dockDialog(dialog);
                }
            });
            entries.put(dialogName, newItem);
            updateMenu();
            return newItem;
        }
        return null;
    }
    
    private void updateMenu() {
        // remove all and "re-insert" everything -> it's easier creating submenus
        this.removeAll();
        createSubmenu(entries, this, "");
    }
    
    private void createSubmenu(Map<String, JMenuItem> entries, JMenu menu, String prefix) {
        TreeMap<String, ArrayList<JMenuItem>> buckets = new TreeMap<>();
        String prevBucket = null;
        for (Map.Entry<String, JMenuItem> entry : entries.entrySet()) {
            String key = entry.getKey();
            JMenuItem value = entry.getValue();
            if(prevBucket == null) {
                prevBucket = key;
                buckets.put(key, new ArrayList<>(Arrays.asList(value)));
            } else {
                // split dialog name in parts beginning with upper case character
                String[] prevBucketParts = prevBucket.split("(?=\\p{Lu})");
                String[] currBucketParts = key.split("(?=\\p{Lu})");
                String currBucket = "";
                // find most overlapping parts
                for (int i = 0; i < prevBucketParts.length; i++) {
                    if(currBucketParts.length > i && prevBucketParts[i].equals(currBucketParts[i])) {
                        currBucket += prevBucketParts[i];
                    } else {
                        break;
                    }
                }
                if(currBucket.isEmpty()) {
                    prevBucket = key;
                    buckets.put(key, new ArrayList<>(Arrays.asList(value)));
                } else {
                    ArrayList<JMenuItem> bucket = buckets.remove(prevBucket);
                    if(bucket == null) { System.out.println(prevBucket+"?!?!"); }
                    bucket.add(value);
                    buckets.put(currBucket, bucket);
                    prevBucket = currBucket;
                }
            }
        }
        // add items to (sub)menu
        buckets.forEach((k,v)->{
            if(v.size() > 1) {
                String prefix_k = prefix + k;
                JMenu submenu = new JMenu(prefix_k);
                Map<String, JMenuItem> n = v.stream().collect(Collectors.toMap(
                    i->{ String h = i.getText().substring(prefix_k.length()); return h; },
                    val -> val
                ));
                // we want nested submenus -> scan submenu for another submenu
                createSubmenu(n, submenu, prefix_k);
                // ... or uncomment for only one level submenu
//                v.forEach(i->submenu.add(i));
                menu.add(submenu);
            } else {
                menu.add(v.get(0));
            }
        });
    }
}
