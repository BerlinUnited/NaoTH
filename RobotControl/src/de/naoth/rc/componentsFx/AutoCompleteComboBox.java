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
            List ignore = Arrays.asList(KeyCode.UP, KeyCode.DOWN, KeyCode.RIGHT, KeyCode.LEFT, KeyCode.HOME, KeyCode.END, KeyCode.TAB, KeyCode.CONTROL, KeyCode.ALT, KeyCode.ESCAPE, KeyCode.BACK_SPACE, KeyCode.SHIFT, KeyCode.ENTER);
            if (ignore.contains(event.getCode()) || event.isControlDown()) {
                return;
            }
            
            // TODO: better handling of Backspace key!
            
            // search for the avialable choices - ignore case
            String s = box.getEditor().getText().toLowerCase();
            Object found = box.getItems().stream().filter((m) -> { return m.toString().toLowerCase().startsWith(s); }).findFirst().orElse(null);

            if (found == null || s.isEmpty()) {
                box.hide();
            } else {
                // set the name of the found item and select the none-entered part
                String current = box.getEditor().getText();
                box.getEditor().setText(found.toString());
                box.getEditor().selectRange(current.length(), box.getEditor().getText().length());
                
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
