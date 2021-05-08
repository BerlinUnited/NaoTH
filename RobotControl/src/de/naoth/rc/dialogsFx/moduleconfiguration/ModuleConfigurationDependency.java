package de.naoth.rc.dialogsFx.moduleconfiguration;

import java.util.stream.Collectors;
import javafx.fxml.FXML;
import javafx.scene.control.Button;
import javafx.scene.layout.Priority;
import javafx.scene.layout.VBox;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class ModuleConfigurationDependency
{
    /** The current module configuration */
    private ModuleConfiguration mConfig;

    /** Some UI elements */
    @FXML private VBox dependencySelection;
    @FXML private VBox dependencyRequire;
    @FXML private VBox dependencyProvide;

    /**
     * Default constructor for the FXML loader.
     */
    public ModuleConfigurationDependency() {}

    /**
     * Constructor for custom initialization.
     * 
     * @param config the current module configuration
     */
    public ModuleConfigurationDependency(ModuleConfiguration config) {
        setModuleConfiguration(config);
    }
    
    /**
     * Sets the current module configuration and updates the internal binding.
     * 
     * @param config the new module configuration
     */
    public void setModuleConfiguration(ModuleConfiguration config) {
        this.mConfig = config;
        
        // set binding to the active module/representation
        mConfig.getActiveProperty().addListener((ob) -> { updateDependencyPanel(); });
    }

    /**
     * Updates the dependency view based on the current selection (module/representation).
     */
    private void updateDependencyPanel() {
        Type current = mConfig.getActiveProperty().get();
        if (current != null) {
            // clear dependency graph first
            dependencySelection.getChildren().clear();
            dependencyRequire.getChildren().clear();
            dependencyProvide.getChildren().clear();
            // set the button for the current selection
            dependencySelection.getChildren().add(createNodeButton(current));
            // with representation, the dependency is reversed!: provide <> require ; default require <> provide
            VBox dependencyLeft = current instanceof Representation ? dependencyProvide : dependencyRequire;
            VBox dependencyRight = current instanceof Representation ? dependencyRequire : dependencyProvide;
            dependencyLeft.getChildren().addAll(current.getRequire().stream().map((r) -> { return createNodeButton(r); }).collect(Collectors.toList()));
            dependencyRight.getChildren().addAll(current.getProvide().stream().map((r) -> { return createNodeButton(r); }).collect(Collectors.toList()));

        }
    }
    
    /**
     * Creates a button for the dependency view.
     * Based on the given type (module/representation) the button is styled slightly different.
     * 
     * @param n the type, for which the button should be generated for (module/representation)
     * @return the generated button
     */
    private Button createNodeButton(Type n) {
        Button b = new Button(n.getName());
        b.setMaxWidth(Double.MAX_VALUE);
        b.setMaxHeight(Double.MAX_VALUE);
        b.setOnAction((a) -> { mConfig.getActiveProperty().set(n); });
        VBox.setVgrow(b, Priority.ALWAYS);
        // set the color based on the node type
        if (n instanceof Module) {
            b.getStyleClass().add("module");
            // mark disabled modules
            ((Module) n).getActiveProperty().addListener((ob, _o, _n) -> {
                if (_n) { b.getStyleClass().remove("inactive"); } 
                else { b.getStyleClass().add("inactive"); }
            });
        } else if (n instanceof Representation) {
            b.getStyleClass().add("representation");
        }
        
        return b;
    }
}
