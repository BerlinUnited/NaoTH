package de.naoth.rc.opengl;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.Iterator;

import com.jogamp.opengl.GL3;
import de.naoth.rc.opengl.Shader;
import de.naoth.rc.opengl.drawings.GLDrawable;
import de.naoth.rc.opengl.file.Texture;
import de.naoth.rc.opengl.model.GLClone;
import de.naoth.rc.opengl.model.GLModel;
import de.naoth.rc.opengl.model.GLObject;
import de.naoth.rc.opengl.model.GLTexturedModel;
import de.naoth.rc.opengl.representations.Matrix4;
import de.naoth.rc.opengl.representations.Point3f;
import java.util.Map.Entry;
import java.util.concurrent.ConcurrentLinkedQueue;

public class Scene {
    private final GL3 gl;

    private final ConcurrentLinkedQueue<GLObject> runQueue;

    private final ConcurrentLinkedQueue<GLDrawable> drawableBuffer;

    private final HashMap<String, GLObject> objects;
    private final HashMap<String, Boolean> glDrawableObjectAllreadyCreated;

    private final Shader standardTexturedModelShader;
    private final Shader standardModelShader;

    private final String pathToGLSL = System.getProperty("user.dir").replaceAll("\\\\", "/") + "/src/de/naoth/rc/opengl/glsl/";

    public Scene(GL3 gl) {
        this.gl = gl;

        this.runQueue = new ConcurrentLinkedQueue();
        this.drawableBuffer = new ConcurrentLinkedQueue();

        this.objects = new HashMap();
        this.glDrawableObjectAllreadyCreated = new HashMap();

        standardTexturedModelShader = new Shader(gl, pathToGLSL, "vertex_shader.glsl", "texture_FS.glsl");
        standardModelShader = new Shader(gl, pathToGLSL, "color_VS.glsl", "color_FS.glsl");

        standardTexturedModelShader.setGlobalUniform("light.position", new float[]{0f, 50f, 0f});
        standardTexturedModelShader.setGlobalUniform("light.intensities", new float[]{1f, 1f, 1f});

        standardModelShader.setGlobalUniform("light.position", new float[]{0f, 50f, 0f});
        standardModelShader.setGlobalUniform("light.intensities", new float[]{1f, 1f, 1f});
    }

    public void add(final GLDrawable newModel) {
        final Shader newModelShader = newModel.getShader(gl);

        if (!glDrawableObjectAllreadyCreated.containsKey(newModel.getClass().getName())) {
            glDrawableObjectAllreadyCreated.put(newModel.getClass().getName(), true);

            new Thread(new Runnable() {
                @Override
                public void run() {
                    System.out.println("newThread");
                    GLObject newGLObject;
                    Texture texture = newModel.getTexture();

                    if (texture == null) {
                        if (newModelShader == null) {
                            newGLObject = new GLModel(gl, newModel.getGLData(), standardModelShader);
                        } else {
                            newGLObject = new GLModel(gl, newModel.getGLData(), newModelShader);
                        }

                    } else if (newModelShader == null) {
                        newGLObject = new GLTexturedModel(gl, texture, newModel.getGLData(), standardTexturedModelShader);
                    } else {
                        newGLObject = new GLTexturedModel(gl, texture, newModel.getGLData(), newModelShader);
                    }

                    Point3f scale = newModel.getScale();
                    newGLObject.getModelMatrix().scale(scale.x, scale.y, scale.z);

                    objects.put(newModel.getClass().getName(), newGLObject);

                }

            }).start();
        }

        this.drawableBuffer.add(newModel);

        /*
        else if (objects.containsKey(newModel)) {
            GLObject newGLObject = objects.get(newModel);

                    if (newModelShader == null) {
                        newGLObject = new GLClone(gl, newGLObject);
                    } else {
                        newGLObject = new GLClone(gl, newGLObject, newModelShader);
                    }
                    
            runQueue.add(newGLObject);
        } else {
            drawableBuffer.add(newModel);
        }
         */
    }

    public void add(GLDrawable[] scene) {
        for (GLDrawable each : scene) {
            this.add(each);
        }
    }

    public void draw(Matrix4 cameraMatrix) {
        for(Iterator<GLDrawable> iterator = drawableBuffer.iterator(); iterator.hasNext();) {
            GLDrawable each = iterator.next();
            
            if (objects.containsKey(each.getClass().getName())) {
                iterator.remove();
                Shader shader = each.getShader(gl);
                GLObject newGLObject = objects.get(each.getClass().getName());

                if (shader == null) {
                    newGLObject = new GLClone(gl, newGLObject);
                } else {
                    newGLObject = new GLClone(gl, newGLObject, shader);
                }
                Point3f scale = each.getScale();
                Point3f pos  = each.getPos();
                newGLObject.getModelMatrix().scale(scale.x, scale.y, scale.z);
                newGLObject.getModelMatrix().translate(pos.x, pos.y, pos.z);
                
                
                runQueue.add(newGLObject);
            }
        }

        for (GLObject each : runQueue) {
            each.bindShader();
            each.bind();
            each.display(cameraMatrix);
            each.unbind();
            each.unbindShader();
        }
    }

    public void clean() {
        this.runQueue.clear();
        this.drawableBuffer.clear();
    }

    public void dispose() {
        for (Entry<String, GLObject> each: objects.entrySet()) {
            each.getValue().dispose();
        }
    }
    
    public boolean bufferIsEmpty() {
        return drawableBuffer.isEmpty();
    }
}
