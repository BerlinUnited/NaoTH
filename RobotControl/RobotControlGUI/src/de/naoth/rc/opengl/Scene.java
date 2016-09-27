package de.naoth.rc.opengl;

import java.util.HashMap;
import java.util.Iterator;

import com.jogamp.opengl.GL3;
import de.naoth.rc.opengl.drawings.GLDrawable;
import de.naoth.rc.opengl.file.Texture;
import de.naoth.rc.opengl.model.GLClone;
import de.naoth.rc.opengl.model.GLComplex;
import de.naoth.rc.opengl.model.GLModel;
import de.naoth.rc.opengl.model.GLObject;
import de.naoth.rc.opengl.model.GLTexturedModel;
import de.naoth.rc.opengl.representations.GLCache;
import de.naoth.rc.opengl.representations.Point3f;
import java.util.Map.Entry;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;

public class Scene implements GLComplex {

    protected final GL3 gl;

    protected ConcurrentLinkedQueue<GLObject> displayQueue;

    protected ConcurrentLinkedQueue<GLDrawable> drawableBuffer;

    GLCache glCache;

    private final Shader standardTexturedModelShader;
    private final Shader standardModelShader;

    private final String pathToGLSL = System.getProperty("user.dir").replaceAll("\\\\", "/") + "/src/de/naoth/rc/opengl/glsl/";

    public Scene(GL3 gl, GLCache glCache, ConcurrentLinkedQueue<GLObject> displayQueue) {
        this.gl = gl;

        this.displayQueue = displayQueue;
        this.glCache = glCache;

        this.drawableBuffer = new ConcurrentLinkedQueue();

        standardTexturedModelShader = new Shader(gl, pathToGLSL, "vertex_shader.glsl", "texture_FS.glsl");
        standardModelShader = new Shader(gl, pathToGLSL, "color_VS.glsl", "color_FS.glsl");

        standardTexturedModelShader.setGlobalUniform("light.position", new float[]{0f, 50f, 0f});
        standardTexturedModelShader.setGlobalUniform("light.intensities", new float[]{1f, 1f, 1f});

        standardModelShader.setGlobalUniform("light.position", new float[]{0f, 50f, 0f});
        standardModelShader.setGlobalUniform("light.intensities", new float[]{1f, 1f, 1f});
    }

    public void add(final GLDrawable newModel) {
        final Shader newModelShader = newModel.getShader(gl);

        if (!glCache.isTouched(newModel.getClass().getName())) {
            glCache.touch(newModel.getClass().getName());

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

                    glCache.put(newModel.getClass().getName(), newGLObject);
                }

            }).start();
        }
        this.drawableBuffer.add(newModel);
    }

    public void add(GLDrawable[] scene) {
        for (GLDrawable each : scene) {
            this.add(each);
        }
    }

    @Override
    public void update() {
        for (Iterator<GLDrawable> iterator = drawableBuffer.iterator(); iterator.hasNext();) {
            GLDrawable each = iterator.next();

            if (glCache.containsKey(each.getClass().getName())) {
                iterator.remove();
                Shader shader = each.getShader(gl);
                GLObject newGLObject = glCache.get(each.getClass().getName());

                if (shader == null) {
                    newGLObject = new GLClone(gl, newGLObject);
                } else {
                    newGLObject = new GLClone(gl, newGLObject, shader);
                }
                Point3f scale = each.getScale();
                Point3f pos = each.getPos();
                newGLObject.getModelMatrix().scale(scale.x, scale.y, scale.z);
                newGLObject.getModelMatrix().translate(pos.x, pos.y, pos.z);

                this.displayQueue.add(newGLObject);
                
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
