package de.naoth.rc.componentsFx;

import com.sun.javafx.scene.control.skin.ComboBoxListViewSkin;
import java.util.Arrays;
import java.util.List;
import javafx.scene.control.ComboBox;
import javafx.scene.control.ListView;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyEvent;

/**
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class AutoCompleteComboBox
{
    public static void install(ComboBox box) {
        
        // show the dropdown list of available choices
        box.getEditor().focusedProperty().addListener((ob,o,n) -> {
            if (n) { box.show(); }
        });
        
        // prevent the dropdown list from stealing key inputs!
        box.addEventHandler(KeyEvent.KEY_PRESSED, (k) -> {
            box.hide();
        });
        
        // handle the key event appropiatly
        box.addEventHandler(KeyEvent.KEY_RELEASED, (event) -> {
            // ignore certain keys
            List ignore = Arrays.asList(
                    KeyCode.UP, KeyCode.DOWN, KeyCode.RIGHT, KeyCode.LEFT, 
                    KeyCode.HOME, KeyCode.END, KeyCode.TAB, KeyCode.CONTROL, 
                    KeyCode.ALT, KeyCode.ESCAPE, KeyCode.SHIFT, KeyCode.ENTER
            );
            if (ignore.contains(event.getCode()) || event.isControlDown()) {
                return;
            }
            
            String searchTerm = box.getEditor().getText().toLowerCase();
            int startSelection = searchTerm.length();
            
            // the backspace reduces the search string, but keeps the last result,
            // if only one character is left and instead just moves the selection
            if (event.getCode() == KeyCode.BACK_SPACE) {
                int n = searchTerm.length() - box.getEditor().getSelectedText().length();
                if (n > 1) {
                    searchTerm = searchTerm.substring(0, searchTerm.length()-1);
                    startSelection--;
                } else if (n == 1) {
                    startSelection = 0;
                }
            }
            
            // search for the avialable choices - ignore case
            Object found = null;
            for (Object item : box.getItems()) {
                if (item.toString().toLowerCase().startsWith(searchTerm)) {
                    found = item;
                    break;
                }
            }
            
            if (found == null || searchTerm.isEmpty()) {
                box.hide();
            } else {
                // set the name of the found item and select the none-entered part
                box.getEditor().setText(found.toString());
                box.getEditor().selectRange(box.getEditor().getText().length(), startSelection);
                
                // make the current item visible without selecting!
                ListView view = ((ComboBoxListViewSkin) box.getSkin()).getListView();
                view.getFocusModel().focus(box.getItems().indexOf(found));
                view.scrollTo(found);
            }

            // show the dropdown list again (hidden on key press)
            box.show();
        });
    }
}
