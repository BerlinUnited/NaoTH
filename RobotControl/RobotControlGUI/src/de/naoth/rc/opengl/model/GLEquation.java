package de.naoth.rc.opengl.model;


import java.nio.FloatBuffer;

import com.jogamp.opengl.GL3;
import de.naoth.rc.opengl.Computable;
import de.naoth.rc.opengl.Shader;

public class GLEquation extends GLModel {
	
	protected Computable equation;
	
	public GLEquation(GL3 gl, Computable equation, Shader shader, int start, int end, int step) {
		super(gl, shader);
		
		this.equation = equation;
		
		FloatBuffer glDataBuffer = FloatBuffer.allocate(((end-start)/step)*3);
		
		for(int x = start; x<=end; x+=step){
			for(int z = start; z<=end; z+=step){
				glDataBuffer.put(x);
				glDataBuffer.put(equation.compute(x, z));
				glDataBuffer.put(z);
			}
		}
		
		glDataBuffer.rewind();
		
	}
 
}


