package de.naoth.rc.opengl.model;

import de.naoth.rc.opengl.Shader;
import de.naoth.rc.opengl.representations.Matrix4;

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
    
    public boolean isReady();
}
