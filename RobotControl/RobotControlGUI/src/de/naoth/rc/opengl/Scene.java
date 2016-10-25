package de.naoth.rc.opengl;

import java.util.Iterator;

import com.jogamp.opengl.GL3;
import de.naoth.rc.opengl.drawings.GLDrawable;
import de.naoth.rc.opengl.model.GLClone;
import de.naoth.rc.opengl.model.GLComplex;
import de.naoth.rc.opengl.model.GLModel;
import de.naoth.rc.opengl.model.GLObject;
import de.naoth.rc.opengl.model.GLTexturedModel;
import de.naoth.rc.opengl.representations.GLCache;
import de.naoth.rc.opengl.representations.Primitive;
import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.util.Map.Entry;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.logging.Level;
import java.util.logging.Logger;

public class Scene implements GLComplex {

    protected final GL3 gl;

    protected ConcurrentLinkedQueue<GLObject> drawableDisplayQueue;

    protected ConcurrentLinkedQueue<String[]> drawableReadyQueue;

    protected ConcurrentHashMap<String, GLDrawable> drawables;

    protected GLCache glCache;

    public Scene(GL3 gl, GLCache glCache, ConcurrentLinkedQueue<GLObject> displayQueue) {
        this.gl = gl;

        this.drawableDisplayQueue = displayQueue;
        this.glCache = glCache;

        this.drawableReadyQueue = new ConcurrentLinkedQueue();
        this.drawables = new ConcurrentHashMap();
    }

    public void add(final String args[]) {

        if (!this.glCache.isTouched(args[0])) {
            //TODO: glCache should probably only be used for primitive models, to avoid misunderstandings
            this.glCache.touch(args[0]);

            new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        Class drawingClass = Class.forName(args[0]);
                        // get the constructor which takes String[] as parameter
                        Constructor constructor = drawingClass.getConstructor(String[].class);
                        // create new Drawing
                        Object drawing = constructor.newInstance(new Object[]{args});
                        if (drawing instanceof GLDrawable) {
                            GLDrawable newDrawable = (GLDrawable) drawing;

                            //Create model data from primitives
                            Primitive[] toCreate = newDrawable.getPrimitives();
                            for (Primitive each : toCreate) {

                                GLObject newModel;

                                Shader shader = glCache.getShader(each.shader);
                                if (shader == null) {
                                    System.err.println("Shader " + each.shader + " is not registered!");
                                }

                                if (each.texture == null) {
                                    newModel = new GLModel(gl, each.mesh, shader);
                                } else {
                                    newModel = new GLTexturedModel(gl, each.texture, each.mesh, shader);
                                }
                                //set model properties as shader uniforms
                                for (Entry<String, Object> property : each.properties.entrySet()) {
                                    newModel.setShaderUniform(property.getKey(), property.getValue());
                                }
                                //Model is created: Cache it!
                                glCache.put(each.id, newModel);
                            }

                            //GLDrawable is created: Cache it!
                            drawables.put(args[0], newDrawable);
                            //GLDrawable is ready to draw
                            drawableReadyQueue.add(args);
                        }

                    } catch (ClassNotFoundException ex) {
                        System.err.println(args[0] + " not found");
                    } catch (NoSuchMethodException | SecurityException | InstantiationException | IllegalAccessException | IllegalArgumentException | InvocationTargetException ex) {
                        Logger.getLogger(Scene.class.getName()).log(Level.SEVERE, null, ex);
                    }

                }

            }).start();
        } else {
            //GLDrawable allready created, it goes directly to the ready queue
            drawableReadyQueue.add(args);
        }
    }

    @Override
    public void update() {

        for (Iterator<String[]> iterator = drawableReadyQueue.iterator(); iterator.hasNext();) {
            String[] args = iterator.next();
            
            if (this.drawables.containsKey(args[0])) {
                iterator.remove();

                GLDrawable toEnque = drawables.get(args[0]);

                toEnque.update(args);

                for (Primitive primitive : toEnque.getPrimitives()) {
                    GLClone newGLObject = new GLClone(gl, glCache.get(primitive.id), primitive.properties);
                    newGLObject.getModelMatrix().multMatrix(primitive.offset);

                    this.drawableDisplayQueue.add(newGLObject);
                }

            }

        }
    }

    public void clean() {
        this.drawableReadyQueue.clear();
    }

    public boolean bufferIsEmpty() {
        return drawableReadyQueue.isEmpty();
    }
}
