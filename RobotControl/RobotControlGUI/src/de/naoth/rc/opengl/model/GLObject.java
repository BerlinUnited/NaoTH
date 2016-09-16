package de.naoth.rc.opengl.model;

import de.naoth.rc.opengl.representations.Matrix4;

public interface GLObject {
    
    void display(Matrix4 viewPoint);
    
    void reshape();
    
    void dispose();
	
	public Matrix4 getModelMatrix();
    
    public void setShaderUniform(String name, Object object);
}
