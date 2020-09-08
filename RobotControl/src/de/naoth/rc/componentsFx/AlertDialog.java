package de.naoth.rc.componentsFx;

import javafx.scene.control.Alert;
import javafx.scene.control.Alert.AlertType;
import javafx.scene.control.ButtonType;
import javafx.stage.Stage;

/**
 * A convenience class to get an alert dialog with one line of code.
 * 
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class AlertDialog
{
    /**
     * Returns the pressed Button of the alert.
     * That can be used to in a condition, eg.:
     * showAlert(AlertType.CONFIRMATION, "OK?", "Is it ok?") == ButtonType.OK
     * 
     * @param type  the alert type
     * @param title the title of the alert
     * @param text  the text of the alert
     * @param onTop the alert "modality"; true if the alert should be always 
     *               on top, false otherwise
     * @return      the button type (depends on the alert type)
     */
    public static ButtonType showAlert(AlertType type, String title, String text, boolean onTop) {
        // NOTE: alert must be (re-)created everytime, otherwise the following HACK doesn't work!
        Alert alert = new Alert(AlertType.NONE);
        alert.setAlertType(type);
        alert.setTitle(title);
        alert.setHeaderText(null);
        alert.setContentText(text);
        // HACK: since it is not possible to set the owner of an javafx alert to
        //       a swing frame, we use the following approach to set the modality!
        ((Stage) alert.getDialogPane().getScene().getWindow()).setAlwaysOnTop(onTop);
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
        return showAlert(AlertType.ERROR, title, text, true);
    }

    /**
     * Shorthand method to show an error alert.
     * 
     * @param title title of the alert
     * @param text  the text of the alert
     * @param onTop the alert "modality"
     * @return      the pressed button type
     */
    public static ButtonType showError(String title, String text, boolean onTop) {
        return showAlert(AlertType.ERROR, title, text, onTop);
    }
    
    /**
     * Shorthand method to show a warning alert.
     * 
     * @param title title of the alert
     * @param text  the text of the alert
     * @return      the pressed button type
     */
    public static ButtonType showWarning(String title, String text) {
        return showAlert(AlertType.WARNING, title, text, true);
    }

    /**
     * Shorthand method to show a warning alert.
     * 
     * @param title title of the alert
     * @param text  the text of the alert
     * @param onTop the alert "modality"
     * @return      the pressed button type
     */
    public static ButtonType showWarning(String title, String text, boolean onTop) {
        return showAlert(AlertType.WARNING, title, text, onTop);
    }
    
    /**
     * Shorthand method to show an info alert.
     * 
     * @param title title of the alert
     * @param text  the text of the alert
     * @return      the pressed button type
     */
    public static ButtonType showInfo(String title, String text) {
        return showAlert(AlertType.INFORMATION, title, text, true);
    }

    /**
     * Shorthand method to show an info alert.
     * 
     * @param title title of the alert
     * @param text  the text of the alert
     * @param onTop the alert "modality"
     * @return      the pressed button type
     */
    public static ButtonType showInfo(String title, String text, boolean onTop) {
        return showAlert(AlertType.INFORMATION, title, text, onTop);
    }
    
    /**
     * Shorthand method to show a confirmation alert.
     * 
     * @param title title of the alert
     * @param text  the text of the alert
     * @return      the pressed button type
     */
    public static ButtonType showConfirmation(String title, String text) {
        return showAlert(AlertType.CONFIRMATION, title, text, true);
    }
    
    /**
     * Shorthand method to show a confirmation alert.
     * 
     * @param title title of the alert
     * @param text  the text of the alert
     * @param onTop the alert "modality"
     * @return      the pressed button type
     */
    public static ButtonType showConfirmation(String title, String text, boolean onTop) {
        return showAlert(AlertType.CONFIRMATION, title, text, onTop);
    }
}
