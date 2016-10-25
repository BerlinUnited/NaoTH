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
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.logging.Level;
import java.util.logging.Logger;

public class Scene implements GLComplex {

    protected final GL3 gl;

    protected ConcurrentLinkedQueue<GLObject> displayQueue;

    protected ConcurrentLinkedQueue<GLDrawable> drawableBuffer;

    protected GLCache glCache;

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
                        if (drawing instanceof GLDrawable) {
                            GLDrawable newDrawable = (GLDrawable) drawing;
                            Primitive[] toCreate = newDrawable.getPrimitives();

                            for (Primitive each : toCreate) {
                                GLObject newModel;
                                
                                Shader shader = glCache.getShader(each.shader);
                                if(shader == null) {
                                    System.err.println("Shader " + each.shader + " is not registered!");
                                }
                                
                                
                                if (each.texture == null) {
                                    newModel = new GLModel(gl, each.mesh, shader);
                                } else {
                                    newModel = new GLTexturedModel(gl, each.texture, each.mesh, shader);
                                }
                                //set model properties as shader uniforms
                                for(Entry<String, Object> property: each.properties.entrySet()) {
                                    newModel.setShaderUniform(property.getKey(), property.getValue());
                                }
                                glCache.put(each.id, newModel);
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

        for (Iterator<GLDrawable> iterator = drawableBuffer.iterator(); iterator.hasNext();) {
            GLDrawable each = iterator.next();
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
