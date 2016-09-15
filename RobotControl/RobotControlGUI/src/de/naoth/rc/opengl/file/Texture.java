package de.naoth.rc.opengl.file;

import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.nio.ByteBuffer;

import javax.imageio.ImageIO;

import com.jogamp.opengl.util.GLBuffers;

public class Texture {
	
	String texPath;
	String texName;
	
	BufferedImage image;
	
	public Texture(String path, String texName) {
		this.texPath = path;
		this.texName = texName;
		
		this.image = null;
	}
	
	public Texture(BufferedImage image) {
		this.image = image;
	}
	
	public ByteBuffer getTextureBuffer() {
		if(image == null) {
			try {
	            image = ImageIO.read(new File(texPath + texName));
	        } catch (IOException io) {
	            System.out.println("Unable to find the texture file: " + texName);
	            try {
	            image = ImageIO.read(new File(texPath + "replacement.png"));
	            } catch (IOException e) {
	            System.out.println("Something went terribly wrong!");
	            }
	        }
		}       
       
        //convert texture to ByteBuffer
    	int[] pixels = new int[image.getWidth() * image.getHeight()];
        image.getRGB(0, 0, image.getWidth(), image.getHeight(), pixels, 0, image.getWidth());

        ByteBuffer textureBuffer = GLBuffers.newDirectByteBuffer(image.getWidth() * image.getHeight() * 4); //4 for RGBA, 3 for RGB
        
        for(int y = 0; y < image.getHeight(); y++){
            for(int x = 0; x < image.getWidth(); x++){
                int pixel = pixels[y * image.getWidth() + x];
                textureBuffer.put((byte) ((pixel >> 16) & 0xFF));     // Red component
                textureBuffer.put((byte) ((pixel >> 8) & 0xFF));      // Green component
                textureBuffer.put((byte) (pixel & 0xFF));               // Blue component
                textureBuffer.put((byte) ((pixel >> 24) & 0xFF));    // Alpha component. Only for RGBA
            }
        }
        textureBuffer.flip();
        
        return textureBuffer;
	}
	
	public int getWidth(){
		if(image == null) {
			System.err.println("Texture not loaded, call getTextureBuffer() first.");
			return 0;
		}
		return image.getWidth();
	}
	
	public int getHeight(){
		if(image == null) {
			System.err.println("Texture not loaded, call getTextureBuffer() first.");
			return 0;
		}
		return image.getHeight();
	}
}
