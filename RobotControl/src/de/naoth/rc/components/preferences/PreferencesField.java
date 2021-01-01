package de.naoth.rc.components.preferences;

/**
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public interface PreferencesField {
    public String getPreferenceName();
    public String getPreferenceValue();
    public void setPreferenceValue(String value);
}
