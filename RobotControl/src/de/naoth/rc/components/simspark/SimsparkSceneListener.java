package de.naoth.rc.components.simspark;

import de.naoth.rc.components.simspark.scene.SimsparkScene;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public interface SimsparkSceneListener
{
    /**
     * Gets called, when the scene got updated.
     *
     * @param state the new scene
     */
    void newSimsparkScene(SimsparkScene state);
}
