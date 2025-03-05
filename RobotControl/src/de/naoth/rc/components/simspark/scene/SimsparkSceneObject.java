package de.naoth.rc.components.simspark.scene;

import de.naoth.rc.drawings.DrawingCollection;

/**
 * Abstract simspark scene object. The object is identified by the scene index.
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public abstract class SimsparkSceneObject
{
    public int sceneIndex;

    public SimsparkSceneObject(int sceneIndex)
    {
        this.sceneIndex = sceneIndex;
    }

    public abstract void draw(DrawingCollection drawings);
}
