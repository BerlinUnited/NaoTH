package de.naoth.rc.opengl.file;

import java.awt.image.BufferedImage;
import java.nio.ByteBuffer;
import java.nio.FloatBuffer;
import java.nio.IntBuffer;
import java.util.ArrayList;

import com.jogamp.opengl.util.GLBuffers;
import de.naoth.rc.opengl.representations.Point2f;
import de.naoth.rc.opengl.representations.Point3f;

public class GLData {
	
	private String objName;
	
    protected IntBuffer glIndexBuffer;
    protected FloatBuffer glDataBuffer;
    
    protected int vertexCount;
    protected int normalCount;
    protected int textureCount;
    
    
    public GLData (FloatBuffer glDataBuffer, IntBuffer glIndexBuffer, int vertexCount, int normalCount, int textureCount){
    	this.glDataBuffer = glDataBuffer;
    	this.glIndexBuffer = glIndexBuffer;
    	
    	this.vertexCount = vertexCount;
    	this.normalCount = normalCount;
    	this.textureCount = textureCount;
    }
    
    public GLData(ArrayList<Point3f> glVertexData, ArrayList<Point3f> glNormalData, ArrayList<Point2f> glTextureData, ArrayList<Integer> glIndexData){
    	
    	vertexCount = glVertexData.size() * 3;
        normalCount = glNormalData.size() * 3;
        textureCount = glTextureData.size() * 2;
        
        //build index buffer
        glIndexBuffer = IntBuffer.allocate(glIndexData.size());
        for (int index: glIndexData) {
            glIndexBuffer.put(index);
        }
        glIndexBuffer.rewind();
        
    	//build vertex+normal+texture buffer
        glDataBuffer = FloatBuffer.allocate(vertexCount + normalCount + textureCount);
        for (int i = 0; i<glVertexData.size(); i++) {
        	glDataBuffer.put(glVertexData.get(i).x);
        	glDataBuffer.put(glVertexData.get(i).y);
        	glDataBuffer.put(glVertexData.get(i).z);
        	glDataBuffer.put(glNormalData.get(i).x);
        	glDataBuffer.put(glNormalData.get(i).y);
        	glDataBuffer.put(glNormalData.get(i).z);
        	glDataBuffer.put(glTextureData.get(i).x);
        	glDataBuffer.put(glTextureData.get(i).y);
        }
        glDataBuffer.rewind();
    }
    
    public GLData(ArrayList<Point3f> glVertexData, ArrayList<Point3f> glNormalData, ArrayList<Integer> glIndexData){
    	
    	vertexCount = glVertexData.size() * 3;
        normalCount = glNormalData.size() * 3;
        textureCount = 0;
        
        //build index buffer
        glIndexBuffer = IntBuffer.allocate(glIndexData.size());
        for (int index: glIndexData) {
            glIndexBuffer.put(index);
        }
        glIndexBuffer.rewind();
        
    	//build vertex+normal+texture buffer
        glDataBuffer = FloatBuffer.allocate(vertexCount + normalCount + textureCount);
        for (int i = 0; i<glVertexData.size(); i++) {
        	glDataBuffer.put(glVertexData.get(i).x);
        	glDataBuffer.put(glVertexData.get(i).y);
        	glDataBuffer.put(glVertexData.get(i).z);
        	glDataBuffer.put(glNormalData.get(i).x);
        	glDataBuffer.put(glNormalData.get(i).y);
        	glDataBuffer.put(glNormalData.get(i).z);
        }
        glDataBuffer.rewind();
    }
    

    protected ByteBuffer loadTextureImage(BufferedImage image) {
    	int[] pixels = new int[image.getWidth() * image.getHeight()];
        image.getRGB(0, 0, image.getWidth(), image.getHeight(), pixels, 0, image.getWidth());

        ByteBuffer buffer = GLBuffers.newDirectByteBuffer(image.getWidth() * image.getHeight() * 4); //4 for RGBA, 3 for RGB
        
        for(int y = 0; y < image.getHeight(); y++){
            for(int x = 0; x < image.getWidth(); x++){
                int pixel = pixels[y * image.getWidth() + x];
                buffer.put((byte) ((pixel >> 16) & 0xFF));     // Red component
                buffer.put((byte) ((pixel >> 8) & 0xFF));      // Green component
                buffer.put((byte) (pixel & 0xFF));               // Blue component
                buffer.put((byte) ((pixel >> 24) & 0xFF));    // Alpha component. Only for RGBA
            }
        }

        buffer.flip();
        
        return buffer;
    }
    
    //getter and setter
    
    public FloatBuffer getDataBuffer() {
        return glDataBuffer;
    }
    
    public IntBuffer getIndexBuffer() {
        return glIndexBuffer;
    }

    public int getVertexCount() {
        return vertexCount;
    }

    public int getNormalCount() {
        return normalCount;
    }

    public int getTextureCount() {
        return textureCount;
    }

    public int getIndexCount() {
        return glIndexBuffer.capacity();
    }
    
    public String getName() {
    	return this.objName;
    }
    
    public void setName(String objName) {
    	this.objName = objName;
    }
}
