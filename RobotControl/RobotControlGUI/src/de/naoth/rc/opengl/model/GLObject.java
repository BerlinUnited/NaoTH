package de.naoth.rc.opengl.model;

import de.naoth.rc.opengl.Shader;
import de.naoth.rc.opengl.representations.Matrix4;
import java.util.Map;

public interface GLObject {
    
    void bind();
    
    void unbind();
    
    void bindShader();
    
    void unbindShader();
    
    void display(Matrix4 viewPoint);
    
    void dispose();
	
	public Matrix4 getModelMatrix();
    
    public Shader getShader();
    
    public void setShaderUniform(String name, Object object);
    
    public Map<String, Object> getShaderUniforms();
    
    public boolean isReady();
}
