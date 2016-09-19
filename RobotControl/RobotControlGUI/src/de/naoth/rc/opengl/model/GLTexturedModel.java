package de.naoth.rc.opengl.model;

import java.nio.ByteBuffer;
import java.nio.IntBuffer;

import com.jogamp.opengl.GL3;
import de.naoth.rc.opengl.Shader;
import de.naoth.rc.opengl.file.GLData;
import de.naoth.rc.opengl.file.GLFile;
import de.naoth.rc.opengl.file.Texture;

public class GLTexturedModel extends GLModel{
	
	protected ByteBuffer textureBuffer;
	protected int texHeight;
	protected int texWidth;
	
	
	public GLTexturedModel(GL3 gl, final Texture texture, final GLFile glFile, Shader shader) {
		super(gl, shader);
		
        glData = glFile.buildGLData();
        		
        textureBuffer = texture.getTextureBuffer();
        texHeight = texture.getHeight();
        texWidth = texture.getWidth();

	}
    
    	public GLTexturedModel(GL3 gl, final Texture texture, final GLData modelData, Shader shader) {
		super(gl, shader);
		     	   
        glData = modelData;
        	
        textureBuffer = texture.getTextureBuffer();
        texHeight = texture.getHeight();
        texWidth = texture.getWidth();
	}
	
	
	public ByteBuffer getTexture() {
		return this.textureBuffer;
	}
	
	@Override
    protected void initializeBuffers() {
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
        gl.glTexImage2D(GL3.GL_TEXTURE_2D, 0, GL3.GL_RGBA, this.texWidth, this.texHeight, 0, GL3.GL_RGBA, GL3.GL_UNSIGNED_BYTE, this.textureBuffer);
        gl.glBindTexture(GL3.GL_TEXTURE_2D, 0);
    }
    
	@Override
    public void bind() {
        if(!super.initialized) {
    		initializeBuffers();
    		super.initialized = true;
    	}
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
	
	@Override
    public void unbind() {
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
    
}
