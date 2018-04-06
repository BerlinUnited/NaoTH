package de.naoth.rc.components.preferences;

/**
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class PreferencesCheckbox extends javax.swing.JCheckBox implements PreferencesField
{
    @Override
    public String getPreferenceName() {
        return getName();
    }

    @Override
    public String getPreferenceValue() {
        return Boolean.toString(isSelected());
    }

    @Override
    public void setPreferenceValue(String value) {
        setSelected(Boolean.valueOf(value));
    }
}
