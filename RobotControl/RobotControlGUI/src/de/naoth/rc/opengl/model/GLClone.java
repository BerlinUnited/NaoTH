package de.naoth.rc.opengl.model;

import de.naoth.rc.opengl.Shader;
import de.naoth.rc.opengl.representations.Matrix4;
import com.jogamp.opengl.GL3;

import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;

public class GLClone implements GLObject {

    protected GL3 gl;

    protected GLObject model;

    protected Shader shader;
    protected Map<String, Object> shaderUniforms;

    protected Matrix4 modelMatrix;

    public GLClone(GL3 gl, GLObject model, Shader shader) {
        this.gl = gl;

        this.shaderUniforms = new HashMap<>();
        this.model = model;
        this.shader = shader;

        this.modelMatrix = new Matrix4();
    }
    
    public GLClone(GL3 gl, GLObject model) {
        this.gl = gl;

        this.shaderUniforms = new HashMap<>();
        this.model = model;
        this.shader = model.getShader();

        this.modelMatrix = new Matrix4();

        for (Entry<String, Object> entry : model.getShaderUniforms().entrySet()) {
            shaderUniforms.put(entry.getKey(), entry.getValue());            
        }
        
        this.shaderUniforms.put("modelMatrix", modelMatrix);
    }

    //GLObject impl
    @Override
    public void dispose() {
        this.shader.destroy(gl);
    }

    @Override
    public void display(Matrix4 cameraMatrix) {
        this.model.display(cameraMatrix);
    }

    @Override
    public Matrix4 getModelMatrix() {
        return this.modelMatrix;
    }

    @Override
    public void setShaderUniform(String name, Object object) {
        shaderUniforms.put(name, object);
    }

    @Override
    public void bind() {
        this.model.bind();
    }

    @Override
    public void unbind() {
        this.model.unbind();
    }

    @Override
    public void bindShader() {
        this.shader.bind(gl);

        for (Map.Entry<String, Object> entry : this.shaderUniforms.entrySet()) {
            shader.setUniform(gl, entry.getKey(), entry.getValue());
        }
    }

    @Override
    public void unbindShader() {
        model.unbindShader();
    }

    @Override
    public boolean isReady() {
        return this.model.isReady();
    }
    
    @Override
    public Shader getShader() {
       return this.shader;
    }

    @Override
    public Map<String, Object> getShaderUniforms() {
        return this.shaderUniforms;
    }
}
