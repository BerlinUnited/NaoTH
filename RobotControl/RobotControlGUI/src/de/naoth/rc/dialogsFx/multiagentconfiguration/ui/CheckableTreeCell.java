package de.naoth.rc.dialogsFx.multiagentconfiguration.ui;

import javafx.beans.property.BooleanProperty;
import javafx.beans.property.ObjectProperty;
import javafx.beans.property.SimpleObjectProperty;
import javafx.beans.value.ObservableValue;
import javafx.scene.control.CheckBox;
import javafx.scene.control.CheckBoxTreeItem;
import javafx.scene.control.TreeCell;
import javafx.scene.control.TreeItem;
import javafx.util.Callback;

/**
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class CheckableTreeCell extends TreeCell<String>
{
    private final CheckBox checkBox;

    private ObservableValue<Boolean> booleanProperty;

    private BooleanProperty indeterminateProperty;

    // --- selected state callback property
    private ObjectProperty<Callback<TreeItem<String>, ObservableValue<Boolean>>>
            selectedStateCallback =
            new SimpleObjectProperty<Callback<TreeItem<String>, ObservableValue<Boolean>>>(
            this, "selectedStateCallback");
    
    public CheckableTreeCell() {
        this.getStyleClass().add("check-box-tree-cell");
        selectedStateCallback.set(item -> {
            if (item instanceof CheckBoxTreeItem<?>) {
                return ((CheckBoxTreeItem<?>)item).selectedProperty();
            }
            return null;
        });

        this.checkBox = new CheckBox();
        this.checkBox.setAllowIndeterminate(false);

        // by default the graphic is null until the cell stops being empty
        setGraphic(null);
    }
    
    
    /**
     * Returns the {@link Callback} that is bound to by the CheckBox shown on screen.
     */
    public final Callback<TreeItem<String>, ObservableValue<Boolean>> getSelectedStateCallback() {
        return selectedStateCallbackProperty().get();
    }
    
    /**
     * Property representing the {@link Callback} that is bound to by the
     * CheckBox shown on screen.
     */
    public final ObjectProperty<Callback<TreeItem<String>, ObservableValue<Boolean>>> selectedStateCallbackProperty() {
        return selectedStateCallback;
    }
    
    @Override
    public void updateItem(String item, boolean empty) {
        super.updateItem(item, empty);
        
        if (empty) {
            setText(null);
            setGraphic(null);
        } else {
            TreeItem<String> treeItem = getTreeItem();

            // update the node content
            setText(treeItem.getValue());
            checkBox.setGraphic(treeItem == null ? null : treeItem.getGraphic());
            
            setGraphic(checkBox);
            
            checkBox.setDisable(!getTreeItem().isLeaf());

            // uninstall bindings
            if (booleanProperty != null) {
                checkBox.selectedProperty().unbindBidirectional((BooleanProperty)booleanProperty);
            }
            if (indeterminateProperty != null) {
                checkBox.indeterminateProperty().unbindBidirectional(indeterminateProperty);
            }

            // install new bindings.
            // We special case things when the TreeItem is a CheckBoxTreeItem
            if (treeItem instanceof CheckBoxTreeItem) {
                CheckBoxTreeItem<String> cbti = (CheckBoxTreeItem<String>) treeItem;
                booleanProperty = cbti.selectedProperty();
                checkBox.selectedProperty().bindBidirectional((BooleanProperty)booleanProperty);

                indeterminateProperty = cbti.indeterminateProperty();
                checkBox.indeterminateProperty().bindBidirectional(indeterminateProperty);
            } else {
                Callback<TreeItem<String>, ObservableValue<Boolean>> callback = getSelectedStateCallback();
                if (callback == null) {
                    throw new NullPointerException(
                            "The CheckBoxTreeCell selectedStateCallbackProperty can not be null");
                }

                booleanProperty = callback.call(treeItem);
                if (booleanProperty != null) {
                    checkBox.selectedProperty().bindBidirectional((BooleanProperty)booleanProperty);
                }
            }
        }
    }
}
