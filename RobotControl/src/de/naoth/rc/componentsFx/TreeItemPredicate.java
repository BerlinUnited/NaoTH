package de.naoth.rc.componentsFx;

import java.util.function.Predicate;
import javafx.scene.control.TreeItem;

/**
 * Found at:
 * https://github.com/eclipse-efx/efxclipse-rt/blob/3.x/modules/ui/org.eclipse.fx.ui.controls/src/main/java/org/eclipse/fx/ui/controls/tree/FilterableTreeItem.java
 * https://github.com/eclipse-efx/efxclipse-rt/blob/master/modules/ui/org.eclipse.fx.ui.controls/src/main/java/org/eclipse/fx/ui/controls/tree/TreeItemPredicate.java
 */
@FunctionalInterface
public interface TreeItemPredicate<T> {

    /**
     * Evaluates this predicate on the given argument.
     *
     * @param parent the parent tree item of the element or null if there is no parent
     * @param value the value to be tested
     * @return {@code true} if the input argument matches the
     * predicate,otherwise {@code false}
     */
    boolean test(TreeItem<T> parent, T value);

    /**
     * Utility method to create a TreeItemPredicate from a given
     * {@link Predicate}
     *
     * @param predicate the predicate
     * @param <T> the element type
     * @return new TreeItemPredicate
     */
    static <T> TreeItemPredicate<T> create(Predicate<T> predicate) {
        return (parent, value) -> predicate.test(value);
    }
}
