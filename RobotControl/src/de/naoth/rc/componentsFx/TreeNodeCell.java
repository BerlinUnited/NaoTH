package de.naoth.rc.componentsFx;

import javafx.scene.control.CheckBox;
import javafx.scene.control.Tooltip;
import javafx.scene.control.cell.CheckBoxTreeCell;

/**
 * Extended CheckBoxTreeCell
 * Sets the tooltip from the TreeNodeItem and disables the checkbox for non-leaf nodes.
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class TreeNodeCell<T extends Object> extends CheckBoxTreeCell<T>
{
    @Override
    public void updateItem(T item, boolean empty) {
        super.updateItem(item, empty);
        
        if (empty || item == null) {
            setText(null);
            setGraphic(null);
        } else {
            TreeNodeItem node = (TreeNodeItem) getTreeItem();
            
            setText(item.toString());
            
            // set tooltip if available
            if (node.getTooltip() != null) {
                setTooltip(new Tooltip(node.getTooltip()));
            }
            
            // disable checkbox of non-leaf nodes
            if (getGraphic() instanceof CheckBox) {
                ((CheckBox)getGraphic()).setDisable(!node.isLeaf());
            }
        }
    }
}
