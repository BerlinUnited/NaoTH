package de.naoth.rc.dialogsFx.moduleconfiguration;

import javafx.collections.transformation.FilteredList;
import javafx.fxml.FXML;
import javafx.scene.control.ListCell;
import javafx.scene.control.ListView;
import javafx.scene.control.ToggleButton;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class ModuleConfigurationRepresentations
{
    /** The current module configuration */
    private ModuleConfiguration mConfig;

    /** Some UI elements */
    @FXML private ToggleButton btnErrors;
    @FXML private ToggleButton btnWarnings;
    @FXML private ToggleButton btnInfos;
    @FXML private ListView<Representation> infoList;
    
    /**
     * Default constructor for the FXML loader.
     */
    public ModuleConfigurationRepresentations() {}

    /**
     * Constructor for custom initialization.
     * 
     * @param config the current module configuration
     */
    public ModuleConfigurationRepresentations(ModuleConfiguration config) {
        setModuleConfiguration(config);
    }
    
    /**
     * Sets the current module configuration and updates the internal binding.
     * 
     * @param config the new module configuration
     */
    public void setModuleConfiguration(ModuleConfiguration config) {
        this.mConfig = config;
        // The bindings can only be installed, when a module configuration object is present.
        updateBindings();
    }
    
    /**
     * Installs the bindings to properties of the module configuration object.
     */
    private void updateBindings() {
        // show only a filtered & sorted version of the representations
        FilteredList<Representation> filteredRepresentationList = new FilteredList<>(mConfig.getRepresentationsProperty().get(), (r) -> showRepresentationInfo(r));
        infoList.setItems(filteredRepresentationList.sorted((r1, r2) -> {
            // sort by representation info first
            return r1.getInfo() == r2.getInfo() ? r1.compareTo(r2) : r2.getInfo().compareTo(r1.getInfo());
        }));
        infoList.getSelectionModel().selectedItemProperty().addListener((ob, o, n) -> {
            // NOTE: make sure the clear selection (below) doesn't trigger an active property change!
            if (n != null) { mConfig.getActiveProperty().set(n); }
        });
        
        // HACK: force re-evaluation of the filtered list
        btnErrors.setOnAction((e) -> { filteredRepresentationList.setPredicate((r) -> showRepresentationInfo(r)); });
        btnWarnings.setOnAction((e) -> { filteredRepresentationList.setPredicate((r) -> showRepresentationInfo(r)); });
        btnInfos.setOnAction((e) -> { filteredRepresentationList.setPredicate((r) -> showRepresentationInfo(r)); });
        
        // set binding to the active module/representation
        mConfig.getActiveProperty().addListener((ob, o, n) -> {
            // clear the representation selection of the representation info list, if another module/representation is selected
            if (infoList.getSelectionModel().getSelectedItem() != n) {
                infoList.getSelectionModel().clearSelection();
            }
        });
    }

    /**
     * Gets called, after the FXML file was loaded.
     */
    @FXML
    private void initialize() {
        infoList.setCellFactory((f) -> new RepresentationListCell());
    }
    
    /**
     * The filter function to filter the representation info view based on the 
     * current active configuration (info state of the representation and selected
     * buttons [info, warning, error])
     * 
     * @param r the representation to check
     * @return true, if the representation should be shown, false otherwise
     */
    private boolean showRepresentationInfo(Representation r) {
        return r.getInfo() == Representation.Info.ERROR && btnErrors.isSelected()
            || r.getInfo() == Representation.Info.WARNING && btnWarnings.isSelected()
            || r.getInfo() == Representation.Info.INFO && btnInfos.isSelected();
    }
    
    /**
     * Class handling, how the representation is shown in the list view based on
     * the "info state" of the representation.
     */
    public class RepresentationListCell extends ListCell<Representation>
    {
        @Override
        protected void updateItem(Representation item, boolean empty) {
            super.updateItem(item, empty);
            
            if (item == null || empty) {
                setGraphic(null);
                setText(null);
            } else {
                switch (item.getInfo()) {
                    case INFO:
                        setText("NOTE: " + item + " is provided and required");
                        getStyleClass().add("info");
                        getStyleClass().removeAll("warning", "error");
                        break;
                    case WARNING:
                        setText("WARNING: " + item + " is provided but not required (" + item.getProvide().size() + ")");
                        getStyleClass().add("warning");
                        getStyleClass().removeAll("info", "error");
                        break;
                    case ERROR:
                        setText("ERROR: " + item + " is required but not provided (" + item.getRequire().size() + ")");
                        getStyleClass().add("error");
                        getStyleClass().removeAll("info", "warning");
                        break;
                    case EMPTY:
                        // NOTE: should never happen: not provided and not required
                    default:
                        setText(null);
                        setGraphic(null);
                        break;
                }
            }
        }
    }
}
