package de.naoth.rc.components.simspark.scene;

import java.util.concurrent.ConcurrentLinkedQueue;

/**
 * Representation for the simspark scene.
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class SimsparkScene
{
    /**
     * Container holding the simspark scene objects of the connected simspark
     * instance
     */
    public final ConcurrentLinkedQueue<SimsparkSceneObject> data = new ConcurrentLinkedQueue<>();

    /**
     * Adds a scene object to the scenes object list.
     *
     * @param object the new scene object
     */
    public void add(SimsparkSceneObject object)
    {
        data.add(object);
    }
}
