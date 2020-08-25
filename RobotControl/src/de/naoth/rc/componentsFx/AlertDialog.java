package de.naoth.rc.componentsFx;

import javafx.scene.control.Alert;
import javafx.scene.control.Alert.AlertType;
import javafx.scene.control.ButtonType;

/**
 * A convenience class to get an alert dialog with one line of code.
 * 
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class AlertDialog
{
    private static final Alert alert = new Alert(AlertType.NONE);
   
    /**
     * Returns the pressed Button of the alert.
     * That can be used to in a condition, eg.:
     * showAlert(AlertType.CONFIRMATION, "OK?", "Is it ok?") == ButtonType.OK
     * 
     * @param type  the alert type
     * @param title the title of the alert
     * @param text  the text of the alert
     * @return      the button type (depends on the alert type)
     */
    public static ButtonType showAlert(AlertType type, String title, String text) {
        alert.setAlertType(type);
        alert.setTitle(title);
        alert.setHeaderText(null);
        alert.setContentText(text);
        // if no button was pressed, the dialog got canceled (ESC, close)
        return alert.showAndWait().orElse(ButtonType.CANCEL);
    }
    
    /**
     * Shorthand method to show an error alert.
     * 
     * @param title title of the alert
     * @param text  the text of the alert
     * @return      the pressed button type
     */
    public static ButtonType showError(String title, String text) {
        return showAlert(AlertType.ERROR, title, text);
    }
    
    /**
     * Shorthand method to show a warning alert.
     * 
     * @param title title of the alert
     * @param text  the text of the alert
     * @return      the pressed button type
     */
    public static ButtonType showWarning(String title, String text) {
        return showAlert(AlertType.WARNING, title, text);
    }
    
    /**
     * Shorthand method to show an info alert.
     * 
     * @param title title of the alert
     * @param text  the text of the alert
     * @return      the pressed button type
     */
    public static ButtonType showInfo(String title, String text) {
        return showAlert(AlertType.INFORMATION, title, text);
    }
    
    /**
     * Shorthand method to show a confirmation alert.
     * 
     * @param title title of the alert
     * @param text  the text of the alert
     * @return      the pressed button type
     */
    public static ButtonType showConfirmation(String title, String text) {
        return showAlert(AlertType.CONFIRMATION, title, text);
    }
}
