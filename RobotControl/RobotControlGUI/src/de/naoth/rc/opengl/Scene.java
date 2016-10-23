package de.naoth.rc.opengl;

import java.util.Iterator;

import com.jogamp.opengl.GL3;
import de.naoth.rc.opengl.drawings.GLDrawable2;
import de.naoth.rc.opengl.model.GLClone;
import de.naoth.rc.opengl.model.GLComplex;
import de.naoth.rc.opengl.model.GLModel;
import de.naoth.rc.opengl.model.GLObject;
import de.naoth.rc.opengl.model.GLTexturedModel;
import de.naoth.rc.opengl.representations.GLCache;
import de.naoth.rc.opengl.representations.Primitive;
import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.logging.Level;
import java.util.logging.Logger;

public class Scene implements GLComplex {

    protected final GL3 gl;

    protected ConcurrentLinkedQueue<GLObject> displayQueue;

    protected ConcurrentLinkedQueue<GLDrawable2> drawableBuffer;

    GLCache glCache;

    private final String pathToGLSL = System.getProperty("user.dir").replaceAll("\\\\", "/") + "/src/de/naoth/rc/opengl/glsl/";

    public Scene(GL3 gl, GLCache glCache, ConcurrentLinkedQueue<GLObject> displayQueue) {
        this.gl = gl;

        this.displayQueue = displayQueue;
        this.glCache = glCache;

        this.drawableBuffer = new ConcurrentLinkedQueue();
    }

    public void add(final String args[]) {

        if (!this.glCache.isTouched(args[0])) {

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
                        if (drawing instanceof GLDrawable2) {
                            GLDrawable2 newDrawable = (GLDrawable2) drawing;
                            Primitive[] toCreate = newDrawable.getPrimitives();

                            for (Primitive each : toCreate) {
                                if (each.texture == null) {
                                    glCache.put(each.id, new GLModel(gl, each.mesh, glCache.getShader(each.shader)));
                                } else {
                                    glCache.put(each.id, new GLTexturedModel(gl, each.texture, each.mesh, glCache.getShader(each.shader)));
                                }
                            }
                            drawableBuffer.add(newDrawable);
                        }

                    } catch (ClassNotFoundException ex) {
                        System.err.println(args[0] + " not found");
                    } catch (NoSuchMethodException ex) {
                        Logger.getLogger(Scene.class.getName()).log(Level.SEVERE, null, ex);
                    } catch (SecurityException ex) {
                        Logger.getLogger(Scene.class.getName()).log(Level.SEVERE, null, ex);
                    } catch (InstantiationException ex) {
                        Logger.getLogger(Scene.class.getName()).log(Level.SEVERE, null, ex);
                    } catch (IllegalAccessException ex) {
                        Logger.getLogger(Scene.class.getName()).log(Level.SEVERE, null, ex);
                    } catch (IllegalArgumentException ex) {
                        Logger.getLogger(Scene.class.getName()).log(Level.SEVERE, null, ex);
                    } catch (InvocationTargetException ex) {
                        Logger.getLogger(Scene.class.getName()).log(Level.SEVERE, null, ex);
                    }

                }

            }).start();
        }

    }


    @Override
    public void update() {

        for (Iterator<GLDrawable2> iterator = drawableBuffer.iterator(); iterator.hasNext();) {
            GLDrawable2 each = iterator.next();
            iterator.remove();

            for (Primitive primitive : each.getPrimitives()) {
                if (glCache.containsKey(primitive.id)) {
                    GLClone newGLObject = new GLClone(gl, glCache.get(primitive.id));
                    newGLObject.getModelMatrix().multMatrix(primitive.offset);
                    
                    this.displayQueue.add(newGLObject);
                }
            }

        }
    }

    public void clean() {
        this.drawableBuffer.clear();
    }

    public boolean bufferIsEmpty() {
        return drawableBuffer.isEmpty();
    }
}
