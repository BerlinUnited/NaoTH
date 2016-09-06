package de.naoth.rc.opengl.model;

import de.naoth.rc.opengl.Shader;
import de.naoth.rc.opengl.representations.Matrix4;
import com.jogamp.opengl.GL3;

import java.util.HashMap;
import java.util.Map;


public class GLDouble implements GLDrawable, GLObject {
	private GL3 gl;
	
    private GLModel model;
    
    private Shader shader;
    private Map<String, Object> shaderUniforms;
    
    private Matrix4 modelMatrix;
    
    public GLDouble(GL3 gl, GLModel model, Shader shader){
    	this.gl = gl;
    	
    	this.shaderUniforms = new HashMap<>();
        this.model = model;
        this.shader = shader;
        
        this.modelMatrix = new Matrix4();
        
        setShaderUniform("modelMatrix", modelMatrix);
    }
    
    //GLDrawable impl
    
    @Override
    public void dispose() {
    	if(!model.isReady()) return;
        shader.destroy(gl);
    }

	@Override
    public void display(Matrix4 cameraMatrix) {
		if(!model.isReady()) return;
		
    	this.shader.bind(gl);
    	this.shader.setGlobalUniform("cameraMatrix", cameraMatrix);
    	
        model.bindBuffer(gl);

        for (Map.Entry<String, Object> entry : this.shaderUniforms.entrySet()) {
            shader.setUniform(gl, entry.getKey(), entry.getValue());
        }
        // draw the triangles
        gl.glDrawElements(GL3.GL_TRIANGLES, model.getData().getIndexCount(), GL3.GL_UNSIGNED_INT, 0);

        model.unbindBuffer(gl);
        this.shader.unbind(gl);
    }

	@Override
	public void reshape() {
		if(!model.isReady()) return;
		// TODO Auto-generated method stub	
	}
	
	//GLObject impl
    
	@Override
    public Matrix4 getModelMatrix() {
        return modelMatrix;
    }
	
	@Override
    public void setShaderUniform(String name, Object object)  {
        shaderUniforms.put(name, object);
    }
}
