package de.naoth.rc.dialogs.multiagentconfiguration.ui;

import de.naoth.rc.dialogs.multiagentconfiguration.Parameter;
import javafx.scene.control.cell.TextFieldTreeTableCell;
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
        }
    }
}
