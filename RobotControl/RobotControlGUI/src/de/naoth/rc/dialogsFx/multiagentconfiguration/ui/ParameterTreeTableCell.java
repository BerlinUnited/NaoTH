package de.naoth.rc.dialogsFx.multiagentconfiguration.ui;

import de.naoth.rc.dialogsFx.multiagentconfiguration.Parameter;
import javafx.scene.control.TextField;
import javafx.scene.control.cell.TextFieldTreeTableCell;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyEvent;
import javafx.util.converter.DefaultStringConverter;

/**
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class ParameterTreeTableCell extends TextFieldTreeTableCell<Parameter, String>
{
    public ParameterTreeTableCell() {
        setConverter(new DefaultStringConverter());
    }

    @Override
    public void startEdit() {
        if(tableRowProperty().getValue().getTreeItem().isLeaf()) {
            super.startEdit();
            installTabKeyHandler();
        }
    }
    
    /**
     * Installs a key listener on the textfield of the editing cell.
     * The key listener handles the tab key and jumps to the next/previous cell in editing state.
     */
    private void installTabKeyHandler() {
        if(getGraphic() instanceof TextField) {
            TextField f = (TextField) getGraphic();
            f.setOnKeyPressed((event) -> {
                if(event.getCode() == KeyCode.TAB) {
                    // commit value
                    commitEdit(f.getText());
                    // go to next/previous cell
                    if(event.isShiftDown()) {
                        getTreeTableView().getSelectionModel().selectAboveCell();
                    } else {
                        getTreeTableView().getSelectionModel().selectBelowCell();
                    }
                    // got into editing state
                    getTreeTableView().edit(getTreeTableView().getSelectionModel().getSelectedCells().get(0).getRow(), getTableColumn());
                    // consume tab key event
                    event.consume();
                }
            });
        }
    }
}
