package de.naoth.rc.opengl.model;

import de.naoth.rc.opengl.Shader;
import de.naoth.rc.opengl.file.GLData;
import de.naoth.rc.opengl.file.GLFile;
import de.naoth.rc.opengl.representations.Matrix4;
import com.jogamp.opengl.GL3;

import java.lang.Thread.State;
import java.nio.IntBuffer;
import java.util.HashMap;
import java.util.Map;

public class GLModel implements GLDrawable, GLObject, Runnable{
    protected GL3 gl;

    protected GLData glData;

    private Shader shader;
    private Map<String, Object> shaderUniforms;
    
    private Matrix4 modelMatrix;

    protected int[] vbo; //Vertex Buffer Object
    protected int[] ibo; //Index  Buffer Object
    protected int[] tbo; //Texture Buffer Object
	
    protected GLFile glFile;
    
    Thread thread;
    
    private boolean ready = false;

    GLModel() {
        this.shaderUniforms = new HashMap<>();
        vbo = new int[1];
        ibo = new int[1];
        tbo = new int[1];
    }
    
    public GLModel(GL3 gl, GLFile glFile, Shader shader) {
        this();
        this.glFile = glFile;
        this.gl = gl;
        
        this.modelMatrix = new Matrix4();

        this.shader = shader;
        setShaderUniform("modelMatrix", modelMatrix);
        //setShaderUniform("tex_1", 0);
        
        thread = new Thread(this);
        thread.start();
    }
    
    @Override
    public void run() {
    	 this.glData = glFile.buildGLData();
    }

    protected void initializeBuffers(boolean transparency) {
        // generate Index Buffer Object
        gl.glGenBuffers(1, IntBuffer.wrap(ibo));
        gl.glBindBuffer(GL3.GL_ELEMENT_ARRAY_BUFFER, ibo[0]);
        gl.glBufferData(GL3.GL_ELEMENT_ARRAY_BUFFER, glData.getIndexCount() * 4, glData.getIndexBuffer(), GL3.GL_STATIC_DRAW);
        gl.glBindBuffer(GL3.GL_ELEMENT_ARRAY_BUFFER, 0);

        // generate vertex buffer object
        gl.glGenBuffers(1, IntBuffer.wrap(vbo));
        gl.glBindBuffer(GL3.GL_ARRAY_BUFFER, vbo[0]);
        gl.glBufferData(GL3.GL_ARRAY_BUFFER, (glData.getNormalCount() + glData.getVertexCount() + glData.getTextureCount()) * 4, glData.getDataBuffer(), GL3.GL_STATIC_DRAW);
        gl.glBindBuffer(GL3.GL_ARRAY_BUFFER, 0);

        // texture
        gl.glActiveTexture(GL3.GL_TEXTURE0);
        gl.glGenTextures(1, IntBuffer.wrap(tbo));
        gl.glBindTexture(GL3.GL_TEXTURE_2D, tbo[0]);
        gl.glTexParameteri(GL3.GL_TEXTURE_2D, GL3.GL_TEXTURE_MIN_FILTER, GL3.GL_LINEAR);
        if(transparency) gl.glTexImage2D(GL3.GL_TEXTURE_2D, 0, GL3.GL_RGBA, glData.getImageWidth(), glData.getImageHeight(), 0, GL3.GL_RGBA, GL3.GL_UNSIGNED_BYTE, glData.getTextureImage());
        //TODO non transparency
        else {
        	//gl.glTexImage2D(GL3.GL_TEXTURE_2D, 0, GL3.GL_RGBA, image.getWidth(), image.getHeight(), 0, GL3.GL_RGB, GL3.GL_UNSIGNED_BYTE, getTextureImage());
        	System.err.println("RGB png is currently not supported!");
        }
        gl.glBindTexture(GL3.GL_TEXTURE_2D, 0);
        
        ready = true;
    }
   
    
    /*
     * GLDrawable impl
     */
    
    @Override
    public void display(Matrix4 cameraMatrix) {
    	if(!(thread.getState() == State.TERMINATED)) {
    		return;
    	}
    	if(!ready) initializeBuffers(true);
    	
    	this.shader.bind(gl);
    	this.shader.setGlobalUniform("cameraMatrix", cameraMatrix);
    	
        bindBuffer(gl);

        for (Map.Entry<String, Object> entry: this.shaderUniforms.entrySet()) {
            shader.setUniform(gl, entry.getKey(), entry.getValue());
        }
        // draw the triangles
        gl.glDrawElements(GL3.GL_TRIANGLES, glData.getIndexCount(), GL3.GL_UNSIGNED_INT, 0);

        unbindBuffer(gl);
        this.shader.unbind(gl);
    }
    
    @Override
    public void reshape() {
    	if(!ready) return;
    	// TODO Auto-generated method stub	
    }
    
    @Override
    public void dispose() {
    	if(!ready) return;
        shader.destroy(gl);
    }
    
    protected void bindBuffer(GL3 gl) {
        // texture image buffer
        gl.glActiveTexture(GL3.GL_TEXTURE0);
        gl.glBindTexture(GL3.GL_TEXTURE_2D, tbo[0]);

        // index buffer
        gl.glBindBuffer(GL3.GL_ELEMENT_ARRAY_BUFFER, ibo[0]);

        // vertex+normal+texture buffer
        gl.glBindBuffer(GL3.GL_ARRAY_BUFFER, vbo[0]);

        gl.glEnableVertexAttribArray(0);
        gl.glVertexAttribPointer(0, 3, GL3.GL_FLOAT, false, 32, 0);
        gl.glEnableVertexAttribArray(1);
        gl.glVertexAttribPointer(1, 3, GL3.GL_FLOAT, false, 32, 12);
        gl.glEnableVertexAttribArray(2);
        gl.glVertexAttribPointer(2, 2, GL3.GL_FLOAT, false, 32, 24);
    }

    protected void unbindBuffer(GL3 gl) {
        // texture image buffer
        gl.glBindTexture(GL3.GL_TEXTURE_2D, 0);
        // index buffer
        gl.glBindBuffer(GL3.GL_ELEMENT_ARRAY_BUFFER, 0);
        // vertex+normal+texture buffer
        gl.glBindBuffer(GL3.GL_ARRAY_BUFFER, 0);

        gl.glDisableVertexAttribArray(0);
        gl.glDisableVertexAttribArray(1);
        gl.glDisableVertexAttribArray(2);
    }
    
    /*
     * getter and setter funktions
     */
    
    public GLData getData() {
    	return glData;
    }

    public Matrix4 getModelMatrix() {
        return modelMatrix;
    }

    public void setShaderUniform(String name, Object object)  {
        shaderUniforms.put(name, object);
    }
    
    public boolean isReady(){
    	return ready;
    }
}