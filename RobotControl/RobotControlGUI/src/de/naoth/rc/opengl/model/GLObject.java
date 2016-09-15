package de.naoth.rc.opengl.model;

import de.naoth.rc.opengl.representations.Matrix4;

public interface GLObject extends GLDrawable {
	
	public Matrix4 getModelMatrix();
    
    public void setShaderUniform(String name, Object object);
}
