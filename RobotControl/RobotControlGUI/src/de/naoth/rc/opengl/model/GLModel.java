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

public class GLModel implements GLObject{
	protected String name;
	
    protected GL3 gl;

    protected GLData glData;

    private Shader shader;
    private Map<String, Object> shaderUniforms;
    
    private Matrix4 modelMatrix;

    protected int[] vbo; //Vertex Buffer Object
    protected int[] ibo; //Index Buffer Object
    protected int[] tbo; //Texture Buffer Object
    
    private boolean initialized = false;
    
    public GLModel(GL3 gl, Shader shader) {
    	name = String.valueOf(System.currentTimeMillis());
    	
        this.shaderUniforms = new HashMap<>();
        vbo = new int[1];
        ibo = new int[1];
        tbo = new int[1];
        
        this.gl = gl;
        
        this.modelMatrix = new Matrix4();

        this.shader = shader;
        setShaderUniform("modelMatrix", modelMatrix);
    }
    
    public GLModel(GL3 gl, GLData glData, Shader shader) {
    	this(gl, shader);

        this.glData = glData;
    }
    
    public GLModel(GL3 gl, final GLFile glFile, Shader shader) {
    	this(gl, shader);
    	      	   
        glData = glFile.buildGLData();
    }

    protected void initializeBuffers() {
        // generate Index Buffer Object
        gl.glGenBuffers(1, IntBuffer.wrap(ibo));
        gl.glBindBuffer(GL3.GL_ELEMENT_ARRAY_BUFFER, ibo[0]);
        gl.glBufferData(GL3.GL_ELEMENT_ARRAY_BUFFER, glData.getIndexCount() * 4, glData.getIndexBuffer(), GL3.GL_STATIC_DRAW);
        gl.glBindBuffer(GL3.GL_ELEMENT_ARRAY_BUFFER, 0);

        // generate vertex buffer object
        gl.glGenBuffers(1, IntBuffer.wrap(vbo));
        gl.glBindBuffer(GL3.GL_ARRAY_BUFFER, vbo[0]);
        gl.glBufferData(GL3.GL_ARRAY_BUFFER, (glData.getNormalCount() + glData.getVertexCount()) * 4, glData.getDataBuffer(), GL3.GL_STATIC_DRAW);
        gl.glBindBuffer(GL3.GL_ARRAY_BUFFER, 0);
    }
   
    
    /*
     * GLDrawable impl
     */
    
    @Override
    public void display(Matrix4 cameraMatrix) {
    	if(!initialized) {
    		initializeBuffers();
    		initialized = true;
    	}
    	
    	this.shader.setGlobalUniform("cameraMatrix", cameraMatrix);
    	this.shader.bind(gl);
    	
        bindBuffer();

        for (Map.Entry<String, Object> entry: this.shaderUniforms.entrySet()) {
            shader.setUniform(gl, entry.getKey(), entry.getValue());
        }
        // draw the triangles
        gl.glDrawElements(GL3.GL_TRIANGLES, glData.getIndexCount(), GL3.GL_UNSIGNED_INT, 0);

        unbindBuffer();
        this.shader.unbind(gl);
    }
    
    @Override
    public void reshape() {
    	// TODO Auto-generated method stub	
    }
    
    @Override
    public void dispose() {
        shader.destroy(gl);
    }
    
    protected void bindBuffer() {
        // index buffer
        gl.glBindBuffer(GL3.GL_ELEMENT_ARRAY_BUFFER, ibo[0]);

        // vertex+normal+texture buffer
        gl.glBindBuffer(GL3.GL_ARRAY_BUFFER, vbo[0]);
        
        gl.glEnableVertexAttribArray(0);
        gl.glVertexAttribPointer(0, 3, GL3.GL_FLOAT, false, 24, 0);
        gl.glEnableVertexAttribArray(1);
        gl.glVertexAttribPointer(1, 3, GL3.GL_FLOAT, false, 24, 12);
    }

    protected void unbindBuffer() {
        // index buffer
        gl.glBindBuffer(GL3.GL_ELEMENT_ARRAY_BUFFER, 0);
        // vertex+normal buffer
        gl.glBindBuffer(GL3.GL_ARRAY_BUFFER, 0);

        gl.glDisableVertexAttribArray(0);
        gl.glDisableVertexAttribArray(1);
    }
    
    /*
     * getter and setter funktions
     */
    
    public GLData getData() {
    	return glData;
    }

    @Override
    public Matrix4 getModelMatrix() {
        return modelMatrix;
    }

    @Override
    public void setShaderUniform(String name, Object object)  {
        shaderUniforms.put(name, object);
    }
    
    public boolean isReady(){
    	return initialized;
    }
}